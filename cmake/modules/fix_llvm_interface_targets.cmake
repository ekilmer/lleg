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
      set(_did_fix FALSE)
      # Fix each library. Assume these are CMake targets
      foreach(_llvm_lib ${LLVM_AVAILABLE_LIBS})
          # Interface include
          get_target_property(_existing_interface_include ${_llvm_lib} INTERFACE_INCLUDE_DIRECTORIES)
          if(NOT _existing_interface_include)
              set_target_properties(${_llvm_lib} PROPERTIES
                  INTERFACE_INCLUDE_DIRECTORIES ${LLVM_INCLUDE_DIRS}
              )
              set(_did_fix TRUE)
          endif()
          unset(_existing_interface_include)

          # Compile options. Exceptions and RTTI
          if(NOT LLVM_ENABLE_EH)
              if(MSVC)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS _HAS_EXCEPTIONS=0)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS /EHs-c-)
              else()
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>")
              endif()
              set(_did_fix TRUE)
          endif()
          if(NOT LLVM_ENABLE_RTTI)
              if(MSVC)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS _HAS_EXCEPTIONS=0)
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS /GR-)
              else()
                  set_property(TARGET ${_llvm_lib} APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>")
              endif()
              set(_did_fix TRUE)
          endif()
      endforeach()
      unset(_llvm_lib)

      if(_did_fix)
          message(STATUS "Fixed LLVM library target interface properties")
      endif()
      unset(_did_fix)
      define_property(
          GLOBAL PROPERTY FIXED_LLVM_LIBRARY_TARGET_INTERFACES
          BRIEF_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
          FULL_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
      )
  endif()
endif()

unset(_already_fixed)
