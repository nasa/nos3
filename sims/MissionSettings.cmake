message(STATUS "Setting up Mission Settings")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type set, assuming Debug")
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(ARCHITECTURE_STRING "amd64")

set(ITC_C_FLAGS "")     #Used for C
set(ITC_CCXX_FLAGS "")  #Works for both C/C++
set(CLANG_OVERRIDE "")
set(BOOST_LIBRARYDIR /usr/lib/amd64-linux-gnu)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Clang detected. MissionSettings will invoke GCC Compile Flags")
    set(CLANG_OVERRIDE True)
endif()

#######GNU Compiler Settings########
if(CMAKE_COMPILER_IS_GNUCXX OR CLANG_OVERRIDE)
    include(CheckCCompilerFlag)

    #not enabling just yet for gcc
	#add_definitions(-Werror) #Turns all warnings into errors

    #Options not available on versions of GCC 3.4.6

    set(ITC_CCXX_FLAGS "${ITC_CCXX_FLAGS} -fdiagnostics-show-option")

    message(STATUS "Setting compiler options...")
    #set(ITC_CCXX_FLAGS "${ITC_CCXX_FLAGS} -fPIC")
    #set(CMAKE_SHARED_LINKER_FLAGS "-fpic")
    #set(CMAKE_EXE_LINKER_FLAGS "-fpic")
    
    CHECK_C_COMPILER_FLAG(-fvisibility=hidden HAVE_VISIBILITY)

endif(CMAKE_COMPILER_IS_GNUCXX OR CLANG_OVERRIDE)

#Removing Visibility check. the core-linux C++ util needs to expose it's symbols in order for cFE to work.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ITC_CCXX_FLAGS} ${ITC_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ITC_CCXX_FLAGS} ${ITC_C_FLAGS}")

