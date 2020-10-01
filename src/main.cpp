/*
 * Copyright (C) 2020 Anton Filimonov and other contributors
 *
 * This file is part of qt_cpp_sentry.
 *
 * qt_cpp_sentry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qt_cpp_sentry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qt_cpp_sentry.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QScreen>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include "crashhandler.h"

int main( int argc, char** argv )
{
    QApplication app{ argc, argv };

    CrashHandler crashHandler;

    auto crashButton = new QPushButton( "Crash me" );
    QObject::connect( crashButton, &QPushButton::clicked, [] {
        int* a = nullptr;
        *a = 1;
    } );

    auto window = new QMainWindow();

    auto widget = new QWidget( window );
    widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    auto layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget( crashButton );
    layout->addStretch();

    widget->setLayout( layout );

    window->setCentralWidget( widget );

    window->setGeometry(
        QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, window->size(),
                             QGuiApplication::primaryScreen()->availableGeometry() ) );
    window->show();

    return app.exec();
}