/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/print_version.hpp"

#include "core/logging.hpp"

#include "ImfHeader.h"
#include "boost/version.hpp"
#include "embree4/rtcore.h"
#include "tbb/version.h"

DE_VERTEXWAHN_BEGIN_NAMESPACE

// see src/bin/exrcheck/main.cpp
void print_open_exr_version() {
    LOG_INFO_WITH_LOCATION("Using {}", OPENEXR_PACKAGE_STRING);
    LOG_INFO_WITH_LOCATION("Using {}", IMATH_PACKAGE_STRING);
}

void print_boost_version() {
    LOG_INFO_WITH_LOCATION("Using Boost {}.{}.{}",
                           BOOST_VERSION / 100000,      // major version
                           BOOST_VERSION / 100 % 1000,  // minor version
                           BOOST_VERSION % 100  // patch level
    );
}

void print_one_tbb_version() {
    LOG_INFO_WITH_LOCATION("oneTBB {}.{}.{}", TBB_VERSION_MAJOR, TBB_VERSION_MINOR, TBB_VERSION_PATCH);
}

void print_embree_version() {
    LOG_INFO_WITH_LOCATION("Embree {}", RTC_VERSION_STRING);
}

DE_VERTEXWAHN_END_NAMESPACE
