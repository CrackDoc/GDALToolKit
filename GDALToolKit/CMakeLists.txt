﻿# CMakeList.txt: CMakeProject 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


#-----------------TODO 自定义编译选项
# set(CMAKE_CXX_FLAGS “-std=c++11 -O2 -g”)

STRING( REGEX REPLACE ".*/(.*)" "\\1" PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} ) 

#-----------------TODO 添加模块变量
set(_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/include)
set(_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/src)
SET(IDE_PATH D:/IDE/Utility/v${MSVC_TOOLSET_VERSION}/x64)
SET(GDAL_PATH ${IDE_PATH}/libgdal3.2)
SET(GEOS_PATH ${IDE_PATH}/libgeos)
SET(PROJ_PATH ${IDE_PATH}/libproj)
SET(LIBCURL_PATH ${IDE_PATH}/libcurl)
SET(EXPAT_PATH ${IDE_PATH}/expat)
SET(XERCES_PATH ${IDE_PATH}/xerces)
SET(IXPUBLIC_PATH ${IDE_PATH}/IxPublic)
SET(EXTENTSTRUCTRUE_PATH ${IDE_PATH}/ExtendStructure)


# 添加执行EXE目录
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/include _INC_EXPATH)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/src _SRC_EXPATH)

#-----------------TODO添加头文件的搜索目录
include_directories(${_INC_PATH})
include_directories(${_SRC_PATH})
include_directories(${GDAL_PATH}/include)
include_directories(${GEOS_PATH}/include)
include_directories(${PROJ_PATH}/include)
include_directories(${LIBCURL_PATH}/include)
include_directories(${EXPAT_PATH}/include)
include_directories(${XERCES_PATH}/include)
include_directories(${IXPUBLIC_PATH}/include)
include_directories(${EXTENTSTRUCTRUE_PATH}/include)

#-----------------TODO 添加lib文件的搜索目录
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
link_directories(${GDAL_PATH}/lib)
link_directories(${GEOS_PATH}/Win32/lib)
link_directories(${PROJ_PATH}/lib)
link_directories(${LIBCURL_PATH}/Win32/lib)
link_directories(${EXPAT_PATH}/Win32/lib)
link_directories(${XERCES_PATH}/lib)
link_directories(${IXPUBLIC_PATH}/Win32/lib)
link_directories(${EXTENTSTRUCTRUE_PATH}/Win32/lib)


#-----------------TODO EXE编译连接
#add_executable (${PROJECT_NAME} ${_INC_EXPATH} ${_SRC_EXPATH})


#-----------------TODO 创建工程内的库
#-----------------STATIC表示创建静态库，目标文件为 libmylib.a，如果是 SHARED， 则为创建动态库.
add_library(${PROJECT_NAME} SHARED ${_INC_EXPATH} ${_SRC_EXPATH})

#-----------------TODO 连接lib 文件
#MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
#IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
#	MESSAGE(STATUS "current platform: Linux ")
#ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
#	MESSAGE(STATUS "current platform: Windows")
#ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
#	MESSAGE(STATUS "current platform: FreeBSD")
#ELSE ()
#	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
#ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
 
#MESSAGE(STSTUS "###################################")

IF (WIN32)
	target_link_libraries(${PROJECT_NAME} gdal_i.lib)
	target_link_libraries(${PROJECT_NAME} geos_i.lib)
	target_link_libraries(${PROJECT_NAME} geos.lib)
	target_link_libraries(${PROJECT_NAME} libcurl.lib)
	target_link_libraries(${PROJECT_NAME} libeay32.lib)
	target_link_libraries(${PROJECT_NAME} ssleay32.lib)
	target_link_libraries(${PROJECT_NAME} proj.lib)
	target_link_libraries(${PROJECT_NAME} expat.lib)
	target_link_libraries(${PROJECT_NAME} xerces.lib)
	target_link_libraries(${PROJECT_NAME} IxPublic.lib)
	target_link_libraries(${PROJECT_NAME} ExtendStructure.lib)
ELSEIF (Linux)
#	target_link_libraries(${PROJECT_NAME} libcurl.so)
ELSEIF (UNIX)
#	MESSAGE(STATUS "Now is UNIX-like OS's.")
ENDIF ()

#-----------------TODO 执行外部命令
#set(THRIFT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/mythrift.thrift)
#exec_program("thrift --gen cpp -o ${CMAKE_CURRENT_SOURCE_DIR} ${THRIFT_FILE}")

#-----------------TODO: 如有需要，请添加测试并安装目标。
