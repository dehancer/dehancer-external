cmake_minimum_required(VERSION 3.12)
set(CURRENT_TARGET_NAME dehancer_external)

#
# Project
#
project(${CURRENT_TARGET_NAME} VERSION 1.5.0 LANGUAGES CXX C)
option(PRINT_DEBUG OFF)

#
# CXX 1x
#
set (CMAKE_CXX_STANDARD 17)
set (BUILD_SHARED_LIBS OFF)

# disable developer warnings
set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    set(STATIC_LINKING OFF)
    message(STATUS "Linux configuration...")
    if (DEHANCER_GPU_METAL)
        message(FATAL_ERROR "Linux GPU Layer does not suppor Metal SDK")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -W -Wall -Wextra -Wno-ignored-qualifiers -Wvariadic-macros")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -Wno-unused-parameter")
elseif(APPLE)
    set(STATIC_LINKING ON)
    message(STATUS "Apple configuration...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -W -Wall -Wextra -Wno-ignored-qualifiers -Wvariadic-macros -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -Wno-unused-parameter")
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -EHsc -Wno-deprecated-declarations")
    add_definitions(/MT /DWIN32=1 /D_ITERATOR_DEBUG_LEVEL=0)
    string(TOLOWER  ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    if (${BUILD_TYPE} STREQUAL "debug")
        add_definitions(/MTd)
    endif()
endif ()


if(PRINT_DEBUG)
    if (WIN32)
        add_definitions(/DPRINT_DEBUG=1 /DDEBUG_BUILD=1)
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPRINT_DEBUG=1 -DDEBUG_BUILD=1")
    endif ()
endif()

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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/OpenFX/include/ DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/openfx/")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/OpenFXS/include/ DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/openfx/")

install(TARGETS ${CURRENT_TARGET_NAME} DESTINATION lib)
install(FILES   ${PUBLIC_INCLUDE_FILES} DESTINATION include/openfx)
