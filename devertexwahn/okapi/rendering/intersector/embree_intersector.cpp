/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/intersector/embree_intersector.hpp"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_EmbreeIntersector() {
    ObjectFactory<PropertySet>::instance().register_class<EmbreeIntersector>("embree");
}

DE_VERTEXWAHN_END_NAMESPACE
