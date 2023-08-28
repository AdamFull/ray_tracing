cmake_minimum_required(VERSION 3.19.1)
project(utllogger)

file(GLOB LOGGER_SOURCES *.cpp *.c *.cc *.h *.hpp)
file(GLOB LOGGER_OUTPUT_SOURCES output/*.cpp output/*.c output/*.cc output/*.h output/*.hpp)

add_subdirectory(backward-cpp)

add_library(${PROJECT_NAME} STATIC ${LOGGER_SOURCES} ${LOGGER_OUTPUT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC backward)

target_include_directories(${PROJECT_NAME} 
PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)