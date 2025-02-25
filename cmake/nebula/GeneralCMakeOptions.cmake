############ Nebula defined options immutable during configure stage #############
message(">>>> Options of ${PROJECT_NAME} <<<<")
option(ENABLE_TESTING                   "Build unit tests" ON)
option(ENABLE_CCACHE                    "Use ccache to speed up compiling" ON)
option(ENABLE_WERROR                    "Regard warnings as errors" ON)
option(ENABLE_JEMALLOC                  "Use jemalloc as memory allocator" ON)
option(ENABLE_ASAN                      "Build with AddressSanitizer" OFF)
option(ENABLE_UBSAN                     "Build with UndefinedBehaviourSanitizer" OFF)
option(ENABLE_TSAN                      "Build with ThreadSanitizer" OFF)
option(ENABLE_STATIC_ASAN               "Statically link against libasan" OFF)
option(ENABLE_STATIC_UBSAN              "Statically link against libubsan" OFF)
option(ENABLE_FUZZY_TESTING             "Enable Fuzzy tests" OFF)
option(ENABLE_FRAME_POINTER             "Build with frame pointer" OFF)
option(ENABLE_STRICT_ALIASING           "Build with -fstrict-aliasing" OFF)
option(ENABLE_COVERAGE                  "Build with coverage report" OFF)
option(ENABLE_PIC                       "Build with -fPIC" OFF)
option(ENABLE_COMPRESSED_DEBUG_INFO     "Compress debug info to reduce binary size" ON)
option(ENABLE_CLANG_TIDY                "Enable clang-tidy if present" OFF)
option(ENABLE_GDB_SCRIPT_SECTION        "Add .debug_gdb_scripts section" OFF)
option(DISABLE_CXX11_ABI                "Whether to disable cxx11 abi" OFF)
option(ENABLE_STANDALONE_VERSION        "Enable standalone version build" OFF)

get_cmake_property(variable_list VARIABLES)
foreach(_varname ${variable_list})
    string(REGEX MATCH "^ENABLE" matched ${_varname})
    if(matched)
        print_option(${_varname})
    endif()
endforeach()
