cmake_minimum_required(VERSION 3.31)
project(GreedyCoinChangeProblem)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(GreedyCoinChangeProblem main.cpp
        GreedyCoinChangeProblem.cpp
        GreedyCoinChangeProblem.h
        GreedyCoinChangeProblemImprovedVersion.cpp
        GreedyCoinChangeProblemImprovedVersion.h)

add_executable(GreedyCoinChangeTests
        GreedyCoinChangeProblemTests.cpp
        GreedyCoinChangeProblem.cpp
        GreedyCoinChangeProblem.h)

add_executable(GreedyCoinChangeValidation
        GreedyCoinChangeValidation.cpp
        GreedyCoinChangeProblem.cpp
        GreedyCoinChangeProblem.h)

add_executable(GreedyCoinChangeImprovedVersionTests
        GreedyCoinChangeProblemImprovedVersionTests.cpp
        GreedyCoinChangeProblemImprovedVersion.cpp
        GreedyCoinChangeProblemImprovedVersion.h)

add_executable(GreedyCoinChangeImprovedValidation
        GreedyCoinChangeProblemImprovedValidation.cpp
        GreedyCoinChangeProblemImprovedVersion.cpp
        GreedyCoinChangeProblemImprovedVersion.h)
