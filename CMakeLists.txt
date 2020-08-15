cmake_minimum_required(VERSION 3.12)
set(CURRENT_TARGET_NAME OpenFXS)

#
# Project
#
project(${CURRENT_TARGET_NAME} VERSION 1.4 LANGUAGES CXX)

#
# CXX 1x
#
set (CMAKE_CXX_STANDARD 17)
set (BUILD_SHARED_LIBS OFF)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++17-extensions")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdeclspec -fms-extensions")
# disable developer warnings
set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)

file (GLOB COMMON_SRC ${COMMON_SRC}
        ./OpenFXS/Library/*.cpp
        )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenFX/include
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenFXS/include
        ./
)

add_library(${CURRENT_TARGET_NAME} ${COMMON_SRC})

FILE(GLOB PUBLIC_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenFX/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenFXS/include/*.h
        )

install(TARGETS ${CURRENT_TARGET_NAME} DESTINATION lib)
install(FILES   ${PUBLIC_INCLUDE_FILES} DESTINATION include/openfx)
