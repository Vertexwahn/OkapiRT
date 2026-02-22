/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/shape/serialized_mesh.h"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_SerializedMesh3f() {
    ObjectFactory<PropertySet>::instance().register_class<SerializedMesh3f>("serialized");
}

DE_VERTEXWAHN_END_NAMESPACE
