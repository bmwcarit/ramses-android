#  -------------------------------------------------------------------------
#  Copyright (C) 2021 BMW AG
#  -------------------------------------------------------------------------
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.
#  -------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)


set(AAR_VERSION_MAJOR 1)
set(AAR_VERSION_MINOR 0)
set(AAR_VERSION_PATCH 2)

set(AAR_VERSION ${AAR_VERSION_MAJOR}.${AAR_VERSION_MINOR}.${AAR_VERSION_PATCH})

project(RamsesAAR
    VERSION ${AAR_VERSION}
    DESCRIPTION "Android Archive (AAR) which packages ramses and ramses-logic for use in Android"
    LANGUAGES CXX
    HOMEPAGE_URL  "https://github.com/bmwcarit/ramses-android"
)

# let cmake know c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(RamsesAAR)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    SET(ENV{PKG_CONFIG_PATH} "")
    SET(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
    SET(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
ENDIF()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ramses/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(AndroidSDK)


# skip policies not known to earlier cmake versions
if(POLICY CMP0054)
   cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)

function(ensure_submodule_exists SUBMOD_NAME)
    if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${SUBMOD_NAME}")
        message(FATAL_ERROR "Missing submodule /${SUBMOD_NAME}. Please fetch Git submodules! (see build documentation)")
    endif()
endfunction()


################################################
################     Ramses   ##################
################################################
set(ramses-sdk_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_IVI_TEST_APPS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_DEMO_ECM OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_SMOKE_TESTS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ramses-sdk_ENABLE_DLT OFF CACHE BOOL "" FORCE)
set(ramses-sdk_FORCE_OFF_SOMEIP_HU ON CACHE BOOL "" FORCE)
set(ramses-sdk_FORCE_OFF_SOMEIP_IC ON CACHE BOOL "" FORCE)

# Disable some things which don't have a CMake option
set(ramses-sdk_ramses-daemon OFF CACHE BOOL "" FORCE)

set(RAMSES_TARGET "ramses-shared-lib-android-egl-es-3-0")
set(ramses-sdk_BUILD_FULL_SHARED_LIB ON CACHE BOOL "" FORCE)
set(ramses-sdk_BUILD_CLIENT_ONLY_SHARED_LIB OFF CACHE BOOL "" FORCE)

ensure_submodule_exists(ramses)

add_subdirectory(ramses)


################################################
#############    Ramses Logic    ###############
################################################
set(ramses-logic_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ramses-logic_SHARED_LIB_TARGET_NAME "ramseslogic" CACHE STRING "" FORCE)
set(ramses-logic_RAMSES_TARGET ${RAMSES_TARGET} CACHE STRING "" FORCE)
set(ramses-logic_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ramses-logic_ENABLE_FLATBUFFERS_GENERATION OFF CACHE BOOL "" FORCE)
set(ramses-logic_ENABLE_CODE_STYLE OFF CACHE BOOL "" FORCE)
set(ramses-logic_ALLOW_RAMSES_BUILD OFF CACHE BOOL "" FORCE)
set(ramses-logic_FORCE_OFF_TESTS ON CACHE BOOL "" FORCE)
set(ramses-logic_BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(ramses-logic_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

ensure_submodule_exists(ramses-logic)

add_subdirectory(ramses-logic)


################################################
###############     Android   ##################
################################################
add_subdirectory(android)

