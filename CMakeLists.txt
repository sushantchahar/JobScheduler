cmake_minimum_required(VERSION 3.20)
project(JobScheduler)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(JobScheduler STATIC
        app/Demo.cpp
) # command, make the project as a static library and need at least one source file to compile

target_include_directories(JobScheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/) # command, makes the JobScheduler as a public directory to include headers using same name

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
add_executable(Tests tests/DemoTest.cpp)

# Link test executable with library and GTest
target_link_libraries(Tests PRIVATE JobScheduler GTest::gtest_main)

# Enable ctest
enable_testing()
include(GoogleTest)
gtest_discover_tests(Tests)