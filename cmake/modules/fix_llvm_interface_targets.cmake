# This file should be used with CMake's include command

get_property(_already_fixed GLOBAL PROPERTY FIXED_LLVM_LIBRARY_TARGET_INTERFACES DEFINED)

if(NOT ${_already_fixed})
  if(NOT DEFINED LLVM_AVAILABLE_LIBS)
      message(WARNING "Call to fix LLVM library target interfaces, but can't find list of libraries")
  else()
      # Fix each library
      foreach(_llvm_lib ${LLVM_AVAILABLE_LIBS})
          # Assume these are CMake targets
          get_target_property(_existing_interface_include ${_llvm_lib} INTERFACE_INCLUDE_DIRECTORIES)
          message("Existing interface_include_dirs: ${_existing_interface_include}")
          if(NOT ${_existing_interface_include})
              set_target_properties(${_llvm_lib} PROPERTIES
                  INTERFACE_INCLUDE_DIRECTORIES ${LLVM_INCLUDE_DIRS}
              )
          endif()
          unset(_existing_interface_include)
      endforeach()
      unset(_llvm_lib)

      define_property(
          GLOBAL PROPERTY FIXED_LLVM_LIBRARY_TARGET_INTERFACES
          BRIEF_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
          FULL_DOCS "Flag to determine if we've already fixed LLVM library target interfaces"
      )
  endif()
endif()

unset(_already_fixed)
