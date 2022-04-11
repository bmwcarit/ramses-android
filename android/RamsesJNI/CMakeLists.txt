#  -------------------------------------------------------------------------
#  Copyright (C) 2021 BMW AG
#  -------------------------------------------------------------------------
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at https://mozilla.org/MPL/2.0/.
#  -------------------------------------------------------------------------

add_library(RamsesJNI SHARED)

file(GLOB src
    src/*.cpp
    include/*.h
)

target_sources(RamsesJNI
    PRIVATE
        ${src}
)

target_link_libraries(RamsesJNI
    PRIVATE
        ${AndroidSDK_LIBRARIES}
        ${ramses-logic_SHARED_LIB_TARGET_NAME}
        ${RAMSES_TARGET}
)

target_include_directories(RamsesJNI
    PRIVATE
        include
)

