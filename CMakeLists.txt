cmake_minimum_required(VERSION 3.28)
project(Demo CXX)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packages
find_package(symengine REQUIRED)


# Target
add_executable(${PROJECT_NAME} src/main.cpp)

# Link
target_link_libraries(${PROJECT_NAME} PUBLIC
   symengine
)
