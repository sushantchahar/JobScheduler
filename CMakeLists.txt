cmake_minimum_required(VERSION 3.20) # command, set the minimum cmake version
project(JobScheduler) # command, set the project name
set(CMAKE_CXX_STANDARD 20) # command, set the C++20 as standard version

add_library(JobScheduler STATIC
        app/Demo.cpp
) # command, make the project as a static library and need at least one source file to compile

target_include_directories(JobScheduler PUBLIC ${CMAKE_SOURCE_DIR}/include/) # command, makes the JobScheduler as a global directory to include headers using same name
