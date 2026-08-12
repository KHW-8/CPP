cmake_minimum_required(VERSION 3.28)
project(Demo CXX)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

# Packages
# find_package(Boost REQUIRED)
# find_package(OpenCV REQUIRED)
# find_package(Qt6 REQUIRED)
find_package(serial REQUIRED)

# Target
add_executable(${PROJECT_NAME} src/main.cpp)

# Link
target_link_libraries(${PROJECT_NAME} PUBLIC
    # boost::boost
    # opencv::opencv
    # qt::qt
    serial::serial
)
