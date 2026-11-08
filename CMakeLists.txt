cmake_minimum_required(VERSION 3.20)
project(JobScheduler)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings for targets. Exclude googletests as warnings are for only the code which ownership is ours
add_library(JobSchedulerWarnings INTERFACE)
if (MSVC)
    target_compile_options(JobSchedulerWarnings INTERFACE /W4 /permissive-)
else ()
    target_compile_options(JobSchedulerWarnings INTERFACE -Wall -Wextra -Wpedantic)
endif ()

option(JOBSCHEDULER_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
if(JOBSCHEDULER_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(JobSchedulerWarnings INTERFACE /WX)
    else()
        target_compile_options(JobSchedulerWarnings INTERFACE -Werror)
    endif()
endif()

add_library(JobScheduler STATIC
        src/Temporary.cpp
        src/Job.cpp
        src/JobState.cpp
) # command, make the project as a static library and need at least one source file to compile

target_include_directories(JobScheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/) # command, makes the JobScheduler as a public directory to include headers using same name

# creating a executable for demo app
add_executable(Demo app/Demo.cpp)
target_link_libraries(Demo PRIVATE JobScheduler)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)

#Windows only:- settings applied to MSVC based compilers
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executables
add_executable(Tests
    tests/DemoTest.cpp
    tests/JobIdTest.cpp
    tests/JobStateTest.cpp
)

# Link test executable with library and GTest
target_link_libraries(Tests PRIVATE JobScheduler GTest::gtest_main)

# Linking all targets with JobSchedulerWarnings
target_link_libraries(JobScheduler PRIVATE JobSchedulerWarnings)
target_link_libraries(Demo PRIVATE JobSchedulerWarnings)
target_link_libraries(Tests PRIVATE JobSchedulerWarnings)

# Enable ctest
enable_testing()
include(GoogleTest)
gtest_discover_tests(Tests)