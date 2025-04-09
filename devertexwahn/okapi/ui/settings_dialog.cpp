/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/settings_dialog.hpp"

#include "QtWidgets/QMessageBox"

SettingsDialog::SettingsDialog(de_vertexwahn::CommandLineArguments* cla, QWidget *parent/*= nullptr*/)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint), 
      ui_(new Ui::SettingsDialog),
      cla_(cla) {
    ui_->setupUi(this);

    switch (cla_->render_mode()) {
    case de_vertexwahn::RenderMode::ParallelProgressive:
        ui_->radioButtonRenderingModeProgressive->setChecked(true);
        break;
    case de_vertexwahn::RenderMode::ParallelTileBased:
        ui_->radioButtonRenderingModeTileBased->setChecked(true);
        break;
    default:
        break;
    }
}

void SettingsDialog::on_radioButtonRenderingModeProgressive_toggled(bool checked) {
    if (checked == true) {
        if (cla_->render_mode() == de_vertexwahn::RenderMode::ParallelProgressive) {
            return;
        }
        else {
            QMessageBox::information(this, "Information", "Cannot switch render mode in UI.\nStart application with command line parameter --render_mode=progressive.");
            ui_->radioButtonRenderingModeTileBased->setChecked(true);
        }
    }
}

void SettingsDialog::on_radioButtonRenderingModeTileBased_toggled(bool checked) {
    if (checked == true) {
        if (cla_->render_mode() == de_vertexwahn::RenderMode::ParallelTileBased) {
            return;
        }
        else {
            QMessageBox::information(this, "Information", "Cannot switch render mode im UI.\nStart application with command line parameter --render_mode=tile_based.");
            ui_->radioButtonRenderingModeProgressive->setChecked(true);
        }
    }
}

void SettingsDialog::on_pushButtonOk_clicked() {
    hide();
};
