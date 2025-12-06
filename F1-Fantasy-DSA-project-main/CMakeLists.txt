cmake_minimum_required(VERSION 3.10)
project(F1_Fantasy_DSA)

set(CMAKE_CXX_STANDARD 20)

add_executable(F1_Fantasy_DSA
        src/main.cpp
        src/Driver.cpp
        src/Driver.h
        src/Constructors.cpp
        src/Constructors.h
        src/User.cpp
        src/User.h
        src/Team.cpp
        src/Team.h
        src/Sorting.cpp
        src/Sorting.h
        src/AVLTree.cpp
        src/AVLTree.h
        src/Trie.cpp
        src/Trie.h
        src/RaceSimulator.cpp
        src/RaceSimulator.h
        src/TeamOptimizer.cpp
        src/TeamOptimizer.h
        src/Leaderboard.cpp
        src/Leaderboard.h
        src/PerformanceAnalyzer.cpp
        src/PerformanceAnalyzer.h
        src/AlertSystem.cpp
        src/AlertSystem.h)