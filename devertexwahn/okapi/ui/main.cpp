/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "core/logging.hpp"
#include "okapi/command_line_arguments.hpp"
#include "okapi/print_version.hpp"
#include "okapi/ui/main_window.hpp"

#include "QtCore/QtCore"
#include "QtWidgets/QApplication"

#include "cpuinfo.h"

using namespace de_vertexwahn;

void print_qt_version() {
    LOG_INFO_WITH_LOCATION("Using Qt {}", QT_VERSION_STR);
}

int main(int argc, char ** argv) {
    CommandLineArguments cla;
    cla.parse_command_line_options(argc, argv);

    if(cla.handle_standard_commands()) {
        return 0;
    }

    print_boost_version();
    print_open_exr_version();
    print_qt_version();
    print_one_tbb_version();
    print_embree_version();

    bool result = cpuinfo_initialize();
    if(!result) {
        return 2;
    }

    LOG_INFO_WITH_LOCATION("Running on {} CPU", cpuinfo_get_package(0)->name);

    QApplication application(argc, argv);
    QApplication::setWindowIcon(QIcon(":/okapi.ui.ico"));
    QApplication::setOrganizationDomain("vertexwahn.de");
    application.setApplicationName("Okapi Renderer Alpha 1.0.9");
    application.setOrganizationName("vertexwahn.de");

    QFile style_sheet(":/okapi.ui.qss");
   
    if (!style_sheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open stylesheet");
    } else {
        application.setStyleSheet(style_sheet.readAll());
    }

    MainWindow main_window{&cla};
    main_window.read_position_settings();
    main_window.show();

    return application.exec();
}
