/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/main_window.hpp"

#include "QtCore/QEvent"
#include "QtCore/QSettings"
#include "QtCore/QThread"
#include "QtCore/QTimer"
#include "QtGui/QClipboard"
#include "QtGui/QKeyEvent"
#include "QtWidgets/QApplication"
#include "QtWidgets/QFileDialog"

MainWindow::MainWindow(CommandLineArguments* cla, QWidget *parent /*= nullptr*/)
: ui_(new Ui::MainWindow), cla_(cla) {
    ui_->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout();
    ui_->scrollAreaWidgetContents->setLayout(layout);

    rw_ = new Viewport(*cla);
    layout->addWidget(rw_);

    connect(&rw_->render_scene_thread_, SIGNAL(update(const QString&)), this, SLOT(update(const QString&)));

    rw_->render_scene_thread_.start();
    rw_->render_preview_thread_.start();

    installEventFilter(this);

    if (cla_->show_maximized()) {
        this->showMaximized();
        LOG_INFO_WITH_LOCATION("Show maximized");
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);

        if (key_event->key() == Qt::Key_Escape)
        {
            this->close();
            return QMainWindow::eventFilter(target,event);
        }
    }
    return QMainWindow::eventFilter(target,event);
}

void MainWindow::write_position_settings() {
    QSettings settings( "devertexwahn", "okapi" );

    settings.beginGroup( "mainwindow" );

    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "savestate", saveState() );
    settings.setValue( "maximized", isMaximized() );
    if ( !isMaximized() ) {
        settings.setValue( "pos", pos() );
        settings.setValue( "size", size() );
    }

    settings.endGroup();
}

void MainWindow::read_position_settings() {
    QSettings settings( "devertexwahn", "okapi" );

    settings.beginGroup( "mainwindow" );

    restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
    restoreState(settings.value( "savestate", saveState() ).toByteArray());
    move(settings.value( "pos", pos() ).toPoint());
    resize(settings.value( "size", size() ).toSize());
    if ( settings.value( "maximized", isMaximized() ).toBool() )
        showMaximized();

    settings.endGroup();
}

void MainWindow::moveEvent( QMoveEvent* ) {
    write_position_settings();
}

void MainWindow::resizeEvent( QResizeEvent* ) {
    write_position_settings();
}

void MainWindow::closeEvent( QCloseEvent* ) {
    LOG_INFO_WITH_LOCATION("Shutting down preview thread");
    rw_->render_preview_thread_.stop_update_ = true;
    rw_->render_preview_thread_.wait();

    rw_->render_scene_thread_.render_desc_.abort = true;
    LOG_INFO_WITH_LOCATION("Waiting for render thread");
    rw_->render_scene_thread_.wait();
    LOG_INFO_WITH_LOCATION("Render thread finished");

    write_position_settings();
}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
};

void MainWindow::on_actionAbout_triggered() {
    if(!about_dialog_)
        about_dialog_ = new AboutDialog(this);

    about_dialog_->show();
}

void MainWindow::on_actionSettings_triggered() {
    if(!settings_dialog_)
        settings_dialog_ = new SettingsDialog(cla_, this);

    settings_dialog_->show();
}

void MainWindow::on_actionFit_render_output_to_window_triggered() {
    ui_->scrollArea->setFixedSize(rw_->render_preview_thread_.image_->size().x()+18, rw_->render_preview_thread_.image_->size().y()+18);
    this->adjustSize();
}

void MainWindow::on_actionPortable_Pixel_Map_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("Portable Pixel Map (*.ppm)"));

    if (!filename.isNull()) {
        rw_->save_as_ppm(filename);
    }
}

void MainWindow::on_actionExportPFM_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("Portable FloatMap Image Format (*.pfm)"));

    if (!filename.isNull()) {
        rw_->save_as_pfm(filename);
    }
}

void MainWindow::on_actionExportPNG_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("Portable Network Graphics (*.png)"));

    if (!filename.isNull()) {
        rw_->save_as_png(filename);
	}
}

void MainWindow::on_actionExportWebP_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("WebP (*.webp)"));

    if (!filename.isNull()) {
        rw_->save_as_webp(filename);
    }
}

void MainWindow::on_actionOpenEXR_exr_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("OpenEXR (*.exr)"));

    if (!filename.isNull()) {
        rw_->save_as_open_exr(filename);
	}
}

void MainWindow::on_actionJPEG_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::currentPath(), tr("JPEG (*.jpg)"));

    if (!filename.isNull()) {
        rw_->save_as_jpeg(filename);
    }
}

void MainWindow::on_actionCopy_triggered() {
    QGuiApplication::clipboard()->setImage(*rw_->render_preview_thread_.qimage_);
}

void MainWindow::on_checkBoxDenoise_toggled(bool checked) {
   rw_->render_preview_thread_.denoise_image_ = checked;
}

void MainWindow::on_horizontalSliderExposureValue_valueChanged(int position) {
    float value = position / 100.f;
    float scale = std::pow(2.f, (value - 0.5f) * 20);

    rw_->render_preview_thread_.scale_ = scale;
}

void MainWindow::update(const QString& message) {
    ui_->statusbar->showMessage(message);
}
