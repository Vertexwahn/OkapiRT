/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/reconstruction_filter/reconstruction_filter.hpp"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_BoxFilter() {
    ObjectFactory<PropertySet>::instance().register_class<BoxFilter>("box");
}

void register_GaussianFilter() {
    ObjectFactory<PropertySet>::instance().register_class<GaussianFilter>("gaussian");
}

void register_TentFilter() {
    ObjectFactory<PropertySet>::instance().register_class<TentFilter>("tent");
}

DE_VERTEXWAHN_END_NAMESPACE
