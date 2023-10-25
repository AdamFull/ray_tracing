cmake_minimum_required(VERSION 3.18)
project(utility_lib LANGUAGES CXX)

file(GLOB UTILITY_MEMORY_SOURCES *.cpp *.hpp *.h)

add_subdirectory(logger)

add_library(${PROJECT_NAME} ${UTILITY_MEMORY_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC utllogger)