/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/shape/sphere.h"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_Sphere3f() {
    ObjectFactory<PropertySet>::instance().register_class<Sphere3f>("sphere");
}

DE_VERTEXWAHN_END_NAMESPACE
