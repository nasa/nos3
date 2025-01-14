if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Clang detected. ProjectSettings will invoke GCC Compile Flags")
    set(CLANG_OVERRIDE True)
endif()

include(CheckCCompilerFlag)

set(ITC_C_FLAGS "${ITC_C_FLAGS}"
                #"-std=c99"
                "-Wall"
                "-Wextra"
                "-Wpedantic" # should discuss this
                #"-Werror"
                #"-Werror=format"
                "-Wformat=2"
                #"-Wcast-align" # should discuss this 
                #"-Wcast-qual"
                "-Wno-discarded-qualifiers"
                "-Winline"
                "-Wpointer-arith"
                "-Wredundant-decls"
                "-Wwrite-strings"
                "-Wuninitialized"
                "-Winit-self"
                "-Wswitch-default"
                #"-Wsuggest-attribute"
                #"-Wsuggest-attribute=const"
                #"-Wsuggest-attribute=noreturn"
                "-Wfloat-equal"
                "-Wno-packed"           # should discuss this
                "-Wno-unused-parameter" # should discuss this
                #"-Wno-unused-variable"  # should discuss this
                "-Wvariadic-macros"
                "-Wvla"
                "-Wstrict-overflow"
                "-Wstrict-overflow=5"
                "-fdiagnostics-show-option"
                #"-Wstack-protector"
                #"-fstack-protector-all"
                #"-fsanitize=address"
                #"-fstack-check"
                #"-Weverything"
                "-pedantic-errors"
                "-fprofile-arcs" # code coverage
                "-ftest-coverage" # ^
                )

#if(${TGTNAME} STREQUAL cpu1)
#    set(ITC_C_FLAGS "${ITC_C_FLAGS}"
#           "-Wformat=0")
#endif()

# Not Compatable with Clang
if(CMAKE_COMPILER_IS_GNUCC)
    set(ITC_C_FLAGS "${ITC_C_FLAGS}"
                    "-Wlogical-op"
                    "-Wunsafe-loop-optimizations")
endif(CMAKE_COMPILER_IS_GNUCC)

string(REPLACE ";" " " ITC_C_FLAGS "${ITC_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ITC_C_FLAGS}")
