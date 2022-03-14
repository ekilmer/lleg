# This file should be used with CMake's include command.
#
# This file fixes up the imported LLVM targets to propagate necessary flags and
# information that make using LLVM with CMake less annoying
#
# These flags _should_ have been propagated in the
# 'llvm/cmake/modules/AddLLVM.cmake' file to the targets that LLVM builds, but
# alas here we are

get_property(_already_fixed GLOBAL PROPERTY FIXED_LLVM_LIBRARY_TARGET_INTERFACES DEFINED)

if(NOT _already_fixed)
  if(NOT DEFINED LLVM_AVAILABLE_LIBS)
      message(WARNING "Call to fix LLVM library target interfaces, but can't find list of libraries")
  else()
      # Fix each library target.
      foreach(_llvm_lib ${LLVM_AVAILABLE_LIBS})
          # Interface include
          set_target_properties(${_llvm_lib} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LLVM_INCLUDE_DIRS})

          # LLVM Definitions
          string (REPLACE "-D" "" _llvm_defs "${LLVM_DEFINITIONS}")
          string (REPLACE " " ";" _llvm_defs "${_llvm_defs}")
          foreach(_llvm_def ${_llvm_defs})
              set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS ${_llvm_def})
          endforeach()

          # Standard compile options for Exceptions and RTTI
          if(NOT LLVM_ENABLE_EH)
              if(MSVC)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS _HAS_EXCEPTIONS=0)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS /EHs-c-)
              else()
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>")
              endif()
          endif()
          if(NOT LLVM_ENABLE_RTTI)
              if(MSVC)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS /GR-)
              else()
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>")
              endif()
          endif()
      endforeach()
      unset(_llvm_lib)

      message(STATUS "Fixed LLVM library target interface properties")
      define_property(
          GLOBAL PROPERTY FIXED_LLVM_LIBRARY_TARGET_INTERFACES
          BRIEF_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
          FULL_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
      )
  endif()
endif()

unset(_already_fixed)
