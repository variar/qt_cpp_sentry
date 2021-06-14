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

#include "crashhandler.h"

#include "sentry.h"

#include "client/crash_report_database.h"

#include "openfilehelper.h"
#include "version.h"

#include <QByteArray>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>

#include <iostream>
#include <memory>
#include <vector>

namespace {

constexpr const char* DSN
    = "https://82ce42a57dda493eb32f705ca3983898@o453796.ingest.sentry.io/5448069";

QString sentryDatabasePath()
{
    auto basePath = QCoreApplication::applicationDirPath();
    return basePath.append( "/dump_data" );
}

QJsonDocument extractJson( const QByteArray& data, int& lastOffset )
{
    QJsonParseError parseError;
    auto envelopeJson = QJsonDocument::fromJson( data, &parseError );
    if ( parseError.error != QJsonParseError::NoError ) {
        envelopeJson = QJsonDocument::fromJson( data.mid( 0, parseError.offset ) );
    }

    lastOffset = parseError.offset;
    return envelopeJson;
}

std::vector<QJsonDocument> extractMessage( const QByteArray& envelopeString )
{
    std::vector<QJsonDocument> messages;
    int position = 0;
    auto offset = 0;
    do {
        messages.push_back( extractJson( envelopeString.mid( position ), offset ) );
        position += offset;
    } while ( offset > 0 && position < envelopeString.size() );

    return messages;
}

QDialog::DialogCode askUserConfirmation( const QString& formattedReport )
{
    auto message = std::make_unique<QLabel>();
    message->setText( "During last run application has encountered an unexpected error." );

    auto crashReportHeader = std::make_unique<QLabel>();
    crashReportHeader->setText( "We collected the following crash report:" );

    auto report = std::make_unique<QTextEdit>();
    report->setReadOnly( true );
    report->setPlainText( formattedReport );
    report->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    auto sendReportLabel = std::make_unique<QLabel>();
    sendReportLabel->setText( "Application can send this report to sentry.io for developers to "
                              "analyze and fix the issue" );

    auto privacyPolicy = std::make_unique<QLabel>();
    privacyPolicy->setText(
        "<a href=\"https://klogg.filimonov.dev/docs/privacy_policy\">Privacy policy</a>" );

    privacyPolicy->setTextFormat( Qt::RichText );
    privacyPolicy->setTextInteractionFlags( Qt::TextBrowserInteraction );
    privacyPolicy->setOpenExternalLinks( true );

    auto exploreButton = std::make_unique<QPushButton>();
    exploreButton->setText( "Open report directory" );
    exploreButton->setFlat( true );
    QObject::connect( exploreButton.get(), &QPushButton::clicked, [] {
        showPathInFileExplorer( sentryDatabasePath().append( "/last_crash" ) );
    } );

    auto privacyLayout = std::make_unique<QHBoxLayout>();
    privacyLayout->addWidget( privacyPolicy.release() );
    privacyLayout->addStretch();
    privacyLayout->addWidget( exploreButton.release() );

    auto buttonBox = std::make_unique<QDialogButtonBox>();
    buttonBox->addButton( "Send report", QDialogButtonBox::AcceptRole );
    buttonBox->addButton( "Discard report", QDialogButtonBox::RejectRole );

    auto confirmationDialog = std::make_unique<QDialog>();
    confirmationDialog->resize( 800, 600 );

    QObject::connect( buttonBox.get(), &QDialogButtonBox::accepted, confirmationDialog.get(),
                      &QDialog::accept );
    QObject::connect( buttonBox.get(), &QDialogButtonBox::rejected, confirmationDialog.get(),
                      &QDialog::reject );

    auto layout = std::make_unique<QVBoxLayout>();
    layout->addWidget( message.release() );
    layout->addWidget( crashReportHeader.release() );
    layout->addWidget( report.release() );
    layout->addWidget( sendReportLabel.release() );
    layout->addLayout( privacyLayout.release() );
    layout->addWidget( buttonBox.release() );

    confirmationDialog->setLayout( layout.release() );

    return static_cast<QDialog::DialogCode>( confirmationDialog->exec() );
}

void checkCrashpadReports( const QString& databasePath )
{
    using namespace crashpad;

    auto database = CrashReportDatabase::InitializeWithoutCreating(
        base::FilePath{ databasePath.toStdString() } );

    std::vector<CrashReportDatabase::Report> pendingReports;
    database->GetCompletedReports( &pendingReports );
    std::cout << "Pending reports " << pendingReports.size() << std::endl;

    for ( const auto& report : pendingReports ) {
        if ( !report.uploaded ) {
            const auto reportFile = QString::fromStdString( report.file_path.value() );
#ifdef Q_OS_WIN
            const auto stackwalker = QCoreApplication::applicationDirPath() + "/minidump_dump.exe";
#else
            const auto stackwalker = QCoreApplication::applicationDirPath() + "/minidump_dump";
#endif
            QProcess stackProcess;
            stackProcess.start( stackwalker, QStringList() << reportFile );
            stackProcess.waitForFinished();

            QString formattedReport = reportFile;
            formattedReport.append( QChar::LineFeed )
                .append( QString::fromUtf8( stackProcess.readAllStandardOutput() ) );

            if ( QDialog::Accepted == askUserConfirmation( formattedReport ) ) {
                database->RequestUpload( report.uuid );
            }
            else {
                database->DeleteReport( report.uuid );
            }
        }
    }
}

} // namespace

CrashHandler::CrashHandler()
{
    const auto dumpPath = sentryDatabasePath();
    checkCrashpadReports( dumpPath );

    sentry_options_t* sentryOptions = sentry_options_new();
    sentry_options_set_debug( sentryOptions, 1 );

#ifdef Q_OS_WIN
    sentry_options_set_database_pathw( sentryOptions, dumpPath.toStdWString().c_str() );
#else
    sentry_options_set_database_path( sentryOptions, dumpPath.toStdString().c_str() );
#endif

    sentry_options_set_dsn( sentryOptions, DSN );

    sentry_options_set_require_user_consent( sentryOptions, true );
    sentry_options_set_auto_session_tracking( sentryOptions, false );

    sentry_options_set_symbolize_stacktraces( sentryOptions, true );

    sentry_options_set_environment( sentryOptions, "development" );
    sentry_options_set_release( sentryOptions, QT_CPP_SENTRY_VERSION );

    sentry_init( sentryOptions );

    sentry_set_tag( "commit", QT_CPP_SENTRY_COMMIT );
    sentry_set_tag( "qt", qVersion() );
}

CrashHandler::~CrashHandler()
{
    sentry_shutdown();
}
