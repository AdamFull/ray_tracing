cmake_minimum_required(VERSION 3.18.0)

#set(CMAKE_C_COMPILER "icx")
#set(CMAKE_CXX_COMPILER "icx")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ipo -mavx -axCORE-AVX2,CORE-AVX512 -w")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")

project(ray_tracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)

file(GLOB_RECURSE ECS_SOURCE_FILES ray_tracer/ecs/*.cpp ray_tracer/ecs/*.hpp)
file(GLOB_RECURSE RENDER_SOURCE_FILES ray_tracer/render/*.cpp ray_tracer/render/*.hpp)
file(GLOB_RECURSE RESOURCES_SOURCE_FILES ray_tracer/resources/*.cpp ray_tracer/resources/*.hpp)
file(GLOB ENGINE_SOURCE_FILES ray_tracer/*.cpp ray_tracer/*.hpp)
file(GLOB MAIN_SOURCES *.cpp *.hpp *.h)

set(MAIN_PROJECT_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_PREFIX_PATH "D:\\Program Files (x86)\\Intel\\oneAPI\\oidn\\2.0.0\\")

#find_package(OpenImageDenoise REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/lib/utility)

#add_library(math_x86_64 OBJECT ray_tracer/math_x86_64.asm)

add_executable(${PROJECT_NAME} 
    ${MAIN_SOURCES}
    ${ECS_SOURCE_FILES} 
    ${RENDER_SOURCE_FILES} 
    ${RESOURCES_SOURCE_FILES} 
    ${ENGINE_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/math_x86_64.asm
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(ray_tracer PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} utility_lib) # OpenImageDenoise)
target_include_directories(${PROJECT_NAME} PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/
${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/lib/utility
${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/
${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/lib/glm
)

#target_compile_options(${PROJECT_NAME} PRIVATE "/fp:fast" "/Qpar" "/Qpar-report:1")

#add_compile_options(/fp:fast)
#add_compile_options(Qpar)
#add_compile_options(/Qvec-report:2)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    GLM_FORCE_PURE
    MAIN_PROJECT_DIRECTORY_PATH="${MAIN_PROJECT_DIRECTORY_PATH}"
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common.h)

add_custom_target(copy_embedded_files ALL)

add_custom_command(
    TARGET copy_embedded_files POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/meshes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/meshes"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config.json" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMENT "Copying embedded files to runtime output directory..."
)