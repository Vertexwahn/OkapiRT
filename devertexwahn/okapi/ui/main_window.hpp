/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_MainWindow_41629fd2_fe98_4bb5_a740_462c8b717333_hpp
#define De_Vertexwahn_Okapi_MainWindow_41629fd2_fe98_4bb5_a740_462c8b717333_hpp

#include "okapi/ui/ui_main_window.h" // include Qt generated file

#include "okapi/command_line_arguments.hpp"
#include "okapi/ui/about_dialog.hpp"
#include "okapi/ui/viewport.hpp"
#include "okapi/ui/settings_dialog.hpp"

#include "QtWidgets/QMainWindow"

using namespace de_vertexwahn;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(CommandLineArguments* cla, QWidget *parent = nullptr);
    ~MainWindow() = default;

    bool eventFilter(QObject *target, QEvent *event) override;

    void write_position_settings();
    void read_position_settings();

    void moveEvent( QMoveEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void closeEvent( QCloseEvent* ) override;

public Q_SLOTS:
    void on_actionExit_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionFit_render_output_to_window_triggered();
    void on_actionPortable_Pixel_Map_triggered();
    void on_actionExportPFM_triggered();
    void on_actionExportPNG_triggered();
    void on_actionExportWebP_triggered();
    void on_actionOpenEXR_exr_triggered();
    void on_actionJPEG_triggered();
    void on_actionCopy_triggered();
    void on_checkBoxDenoise_toggled(bool checked);
    void on_horizontalSliderExposureValue_valueChanged(int position);

    void update(const QString& message);

private:
    Viewport* rw_ = nullptr;
    AboutDialog* about_dialog_ = nullptr;
    SettingsDialog* settings_dialog_ = nullptr;
    Ui::MainWindow* ui_ = nullptr;
    CommandLineArguments* cla_ = nullptr;
};

#endif // end define De_Vertexwahn_Okapi_MainWindow_41629fd2_fe98_4bb5_a740_462c8b717333_hpp
