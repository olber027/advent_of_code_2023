cmake_minimum_required(VERSION 3.20)

project(Day_14 CXX)

add_executable(${PROJECT_NAME}_Part_1 Part_1.cpp)
target_link_libraries(${PROJECT_NAME}_Part_1 PUBLIC smart_string)

add_executable(${PROJECT_NAME}_Part_2 Part_2.cpp)
target_link_libraries(${PROJECT_NAME}_Part_2 PUBLIC smart_string)
