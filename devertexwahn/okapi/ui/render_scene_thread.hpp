/*
 *  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_RenderSceneThread_3817cc7d_3000_4371_9768_dc5e67577169_hpp
#define De_Vertexwahn_Okapi_RenderSceneThread_3817cc7d_3000_4371_9768_dc5e67577169_hpp

#include "flatland/rendering/scene/scene.hpp"
#include "okapi/command_line_arguments.hpp"
#include "okapi/rendering/rendering.hpp"
#include "okapi/ui/render_state.hpp"

#include "QtCore/QString"
#include "QtCore/QThread"

#include <filesystem>

class RenderSceneThread : public QThread
{
    Q_OBJECT

public:
    RenderSceneThread(const de_vertexwahn::CommandLineArguments& cla);

    void run() override;

signals:
    void update(const QString &message);
    void resultReady(const QString &s);

public:
    de_vertexwahn::InteractiveRenderDescription render_desc_;
    de_vertexwahn::ReferenceCounted<de_vertexwahn::Scene3f> scene = nullptr;
    RenderState render_state_ = RenderState::Start;
    de_vertexwahn::CommandLineArguments cla_;
};

#endif // end define De_Vertexwahn_Okapi_RenderSceneThread_3817cc7d_3000_4371_9768_dc5e67577169_hpp
