# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

STRING( REGEX REPLACE ".*/(.*)" "\\1" PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} ) 

project (${PROJECT_NAME})

# 包含子项目。
add_subdirectory (${PROJECT_NAME})
add_subdirectory("UnitTest")