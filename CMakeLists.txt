cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpp_password_manager)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_password_manager main.cpp
        CredentialManager.h
        CredentialManager.cpp
        utils.cpp
        utils.h
)

add_executable(run_tests
        test.cpp
        CredentialManager.h
        CredentialManager.cpp
        utils.cpp
        utils.h
)
