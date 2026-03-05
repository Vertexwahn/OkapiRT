/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/integrator/normal_integrator.hpp"

#include "core/object_factory.hpp"

#include "flatland/rendering/scene/scene.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void register_NormalIntegrator3f() {
    ObjectFactory<PropertySet>::instance().register_class<NormalIntegrator3f>("normal");
}

DE_VERTEXWAHN_END_NAMESPACE
