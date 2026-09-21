cmake_minimum_required(VERSION 4.3)

project(dehancer_external VERSION 1.7.0 LANGUAGES CXX)

include(GNUInstallDirs)

option(PRINT_DEBUG "Enable OpenFX debug logging" OFF)
option(DEHANCER_EXTERNAL_INSTALL "Install library and CMake package" ${PROJECT_IS_TOP_LEVEL})

# OpenFX support is linked into plugins, which supply getPluginIDs().
add_library(dehancer_external STATIC
    OpenFXS/Library/ofxsCore.cpp
    OpenFXS/Library/ofxsHWNDInteract.cpp
    OpenFXS/Library/ofxsImageEffect.cpp
    OpenFXS/Library/ofxsInteract.cpp
    OpenFXS/Library/ofxsLog.cpp
    OpenFXS/Library/ofxsMultiThread.cpp
    OpenFXS/Library/ofxsParams.cpp
    OpenFXS/Library/ofxsProperty.cpp
    OpenFXS/Library/ofxsPropertyValidation.cpp
)
add_library(dehancer_external::dehancer_external ALIAS dehancer_external)

target_compile_features(dehancer_external PUBLIC cxx_std_17)

set_target_properties(dehancer_external PROPERTIES
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET default
)

if(PRINT_DEBUG)
    target_compile_definitions(dehancer_external PRIVATE PRINT_DEBUG=1 DEBUG_BUILD=1)
endif()

# Keep qualified and unqualified includes identical in build and install trees.
file(GLOB dehancer_external_headers CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenFX/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenFXS/include/*.h"
)
set(dehancer_external_public_headers)
foreach(header IN LISTS dehancer_external_headers)
    cmake_path(GET header FILENAME filename)
    set(output "${CMAKE_CURRENT_BINARY_DIR}/include/openfx/${filename}")
    configure_file("${header}" "${output}" COPYONLY)
    list(APPEND dehancer_external_public_headers "${output}")
endforeach()

target_sources(dehancer_external PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
    FILES ${dehancer_external_public_headers}
)

target_include_directories(dehancer_external PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/openfx>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/openfx>"
)

if(NOT DEHANCER_EXTERNAL_INSTALL)
    return()
endif()

include(CMakePackageConfigHelpers)

install(TARGETS dehancer_external
    EXPORT dehancer_externalTargets
    FILE_SET HEADERS
)

set(dehancer_external_cmake_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/dehancer_external")
configure_package_config_file(
    cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_externalConfig.cmake"
    INSTALL_DESTINATION "${dehancer_external_cmake_install_dir}"
    NO_SET_AND_CHECK_MACRO
)
install(EXPORT dehancer_externalTargets
    NAMESPACE dehancer_external::
    DESTINATION "${dehancer_external_cmake_install_dir}"
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_externalConfig.cmake"
    DESTINATION "${dehancer_external_cmake_install_dir}"
)
