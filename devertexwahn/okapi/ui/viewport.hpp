/*
 *  SPDX-FileCopyrightText: Copyright 2024-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_Viewport_aacaddc2_e91f_46c1_bbe2_6b4ee48e03e6_hpp
#define De_Vertexwahn_Okapi_Viewport_aacaddc2_e91f_46c1_bbe2_6b4ee48e03e6_hpp

#include "core/reference_counted.hpp"
#include "okapi/command_line_arguments.hpp"
#include "okapi/ui/camera.hpp"
#include "okapi/ui/render_preview_thread.hpp"
#include "okapi/ui/render_scene_thread.hpp"

#include "QKeyEvent"
#include "QMouseEvent"
#include "QtWidgets/QMessageBox"
#include "QtWidgets/QStylePainter"

#include "OpenImageDenoise/oidn.hpp"

class Viewport : public QWidget
{
    Q_OBJECT

public:
    Viewport(const de_vertexwahn::CommandLineArguments& cla, QWidget* parent = nullptr);
    virtual ~Viewport();

    void paintEvent(QPaintEvent * e) override;

    void save_as_ppm(const QString& filename);
    void save_as_png(const QString& filename);
    void save_as_webp(const QString& filename);
    void save_as_pfm(const QString& filename);
    void save_as_open_exr(const QString& filename);
    void save_as_jpeg(const QString& filename);

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;

public Q_SLOTS:
    void draw_next_frame();

private Q_SLOTS:
    void tick();

private:
    void update_camera();

public:
    RenderSceneThread render_scene_thread_;
    RenderPreviewThread render_preview_thread_;

private:
    QTimer* timer_;
    std::chrono::nanoseconds last_tick_;

    de_vertexwahn::ReferenceCounted<de_vertexwahn::Camera> camera_;
    de_vertexwahn::ReferenceCounted<de_vertexwahn::CameraController> camera_controller_;
    de_vertexwahn::Vector2f last_mouse_position_;
};

#endif // end define De_Vertexwahn_Okapi_Viewport_aacaddc2_e91f_46c1_bbe2_6b4ee48e03e6_hpp
