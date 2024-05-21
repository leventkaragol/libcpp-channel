cmake_minimum_required(VERSION 3.14)

project(examples)

add_executable(examples single-producer-single-consumer.cpp)

target_link_libraries(examples PRIVATE libcpp-channel)
