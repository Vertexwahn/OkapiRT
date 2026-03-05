/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/bsdf/diffuse.hpp"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_Diffuse() {
    ObjectFactory<PropertySet>::instance().register_class<Diffuse>("diffuse");
}

DE_VERTEXWAHN_END_NAMESPACE
