/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/about_dialog.hpp"

#include "core/logging.hpp"

#include <iostream>
#include <fstream>

AboutDialog::AboutDialog(QWidget *parent/*= nullptr*/) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint), ui_(new Ui::AboutDialog) {
    ui_->setupUi(this);

    std::ifstream license_notes;
    std::string filename = "okapi/ui/license_notes.txt";
    license_notes.open(filename);
    if (!license_notes) {
        LOG_ERROR_WITH_LOCATION("Unable to open file {}", filename);
    }
    else
    {
        std::string str((std::istreambuf_iterator<char>(license_notes)),
                        std::istreambuf_iterator<char>());
        ui_->textEditLicenseNotes->setText(QString(str.data()));
    }
}

void AboutDialog::on_pushButtonOk_clicked() {
    hide();
};
