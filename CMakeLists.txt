cmake_minimum_required(VERSION 3.5)

project(compiler_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB ROOT_SRC src/*.cpp)
file(GLOB ROOT_HEADER src/*.h)
file(GLOB ROOT_INLINE src/*.inl)

add_executable(compiler_cpp
    ${ROOT_SRC}
    ${ROOT_HEADER}
    ${ROOT_INLINE})


set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
