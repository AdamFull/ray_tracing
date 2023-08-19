cmake_minimum_required(VERSION 3.18.0)
project(ray_tracer)

file(GLOB_RECURSE ECS_SOURCE_FILES ray_tracer/ecs/*.cpp ray_tracer/ecs/*.hpp)
file(GLOB_RECURSE RENDER_SOURCE_FILES ray_tracer/render/*.cpp ray_tracer/render/*.hpp)
file(GLOB_RECURSE RESOURCES_SOURCE_FILES ray_tracer/resources/*.cpp ray_tracer/resources/*.hpp)
file(GLOB ENGINE_SOURCE_FILES ray_tracer/*.cpp ray_tracer/*.hpp)

find_package(benchmark CONFIG REQUIRED)

enable_language(ASM_MASM)

#add_library(math_x86_64 OBJECT ray_tracer/math_x86_64.asm)

add_executable(${PROJECT_NAME} 
    main.cpp 
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

target_link_libraries(${PROJECT_NAME} benchmark::benchmark benchmark::benchmark_main)
target_include_directories(${PROJECT_NAME} PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/
${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/
${CMAKE_CURRENT_SOURCE_DIR}/ray_tracer/lib/glm
)

target_compile_options(${PROJECT_NAME} PRIVATE "/fp:fast")

target_compile_definitions(${PROJECT_NAME} PUBLIC
    GLM_FORCE_PURE
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common.h)