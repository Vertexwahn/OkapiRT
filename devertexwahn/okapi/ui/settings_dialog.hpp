/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/ui_settings_dialog.h" // include Qt generated file

#include "okapi/command_line_arguments.hpp"

#include "QtCore/QThread"
#include "QtCore/QFile"

class SettingsDialog : public QDialog {
    Q_OBJECT;

public:
    SettingsDialog(de_vertexwahn::CommandLineArguments* cla, QWidget *parent = nullptr);

public Q_SLOTS:
    void on_pushButtonOk_clicked();

    void on_radioButtonRenderingModeProgressive_toggled(bool checked);

    void on_radioButtonRenderingModeTileBased_toggled(bool checked);

private:
    Ui::SettingsDialog* ui_;
    de_vertexwahn::CommandLineArguments* cla_;
};
