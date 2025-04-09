/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/ui_about_dialog.h" // include Qt generated file

#include "QtCore/QThread"
#include "QtCore/QFile"

class AboutDialog : public QDialog {
    Q_OBJECT;

public:
    AboutDialog(QWidget *parent = nullptr);

public Q_SLOTS:
    void on_pushButtonOk_clicked();

private:
    Ui::AboutDialog* ui_;
};
