cmake_minimum_required(VERSION 3.18)

project(stl_parser)

add_library(stl_parser SHARED src/stl_parser.cpp)
target_include_directories(stl_parser PUBLIC include)
target_compile_definitions(stl_parser PRIVATE -DDLL_EXPORT)

add_executable(ut_stl_parser src/main.cpp)

target_link_libraries(ut_stl_parser stl_parser)
target_include_directories(ut_stl_parser PUBLIC include)