cmake_minimum_required(VERSION 3.14)

project(examples)

add_executable(single-producer-single-consumer single-producer-single-consumer.cpp)
add_executable(single-producer-multiple-consumer single-producer-multiple-consumer.cpp)
add_executable(multiple-producer-single-consumer multiple-producer-single-consumer.cpp)
add_executable(multiple-producer-multiple-consumer multiple-producer-multiple-consumer.cpp)

target_link_libraries(single-producer-single-consumer PRIVATE libcpp-channel)
target_link_libraries(single-producer-multiple-consumer PRIVATE libcpp-channel)
target_link_libraries(multiple-producer-single-consumer PRIVATE libcpp-channel)
target_link_libraries(multiple-producer-multiple-consumer PRIVATE libcpp-channel)
