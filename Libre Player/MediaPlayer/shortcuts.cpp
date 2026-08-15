#include "shortcuts.h"
#include "mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QKeySequence>

Shortcuts::Shortcuts(MainWindow *window)
    : QObject(window),
    m_window(window)
{
    loadShortcuts();
}

void Shortcuts::loadShortcuts()
{
    const QString configDir =
        QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation
            );

    QDir().mkpath(configDir);

    const QString filePath =
        configDir + "/shortcuts.txt";

    if (!QFile::exists(filePath))
        createDefaultFile();

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly |
                   QIODevice::Text))
        return;

    QTextStream stream(&file);

    while (!stream.atEnd()) {

        QString line =
            stream.readLine().trimmed();

        if (line.isEmpty() ||
            line.startsWith('#'))
            continue;

        const int separator =
            line.indexOf('=');

        if (separator <= 0)
            continue;

        const QString action =
            line.left(separator).trimmed();

        const QString key =
            line.mid(separator + 1).trimmed();

        if (action.isEmpty() ||
            key.isEmpty())
            continue;

        const QKeySequence sequence(key);

        if (sequence.isEmpty())
            continue;

        QShortcut *shortcut =
            new QShortcut(sequence, m_window);

        m_shortcuts.insert(action, shortcut);

        if (action == "PlayPause") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::togglePlayback
                );
        }

        else if (action == "Stop") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::shortcutStop
                );
        }

        else if (action == "Previous") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::previousTrack
                );
        }

        else if (action == "Next") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::nextTrack
                );
        }

        else if (action == "Mute") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::toggleMute
                );
        }

        else if (action == "SeekBackward") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::seekBackward
                );
        }

        else if (action == "SeekForward") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::seekForward
                );
        }

        else if (action == "VolumeUp") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::volumeUp
                );
        }

        else if (action == "VolumeDown") {

            connect(
                shortcut,
                &QShortcut::activated,
                m_window,
                &MainWindow::volumeDown
                );
        }

    }

    file.close();
}

void Shortcuts::createDefaultFile()
{
    const QString configDir =
        QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation
            );

    QDir().mkpath(configDir);

    QFile file(
        configDir + "/shortcuts.txt"
        );

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text))
        return;

    QTextStream stream(&file);

    stream << "# MediaPlayer shortcuts\n\n";
    stream << "PlayPause=Space\n";
    stream << "Stop=S\n";
    stream << "Previous=Left\n";
    stream << "Next=Right\n";
    stream << "SeekBackward=Ctrl+Left\n";
    stream << "SeekForward=Ctrl+Right\n";
    stream << "VolumeUp=Up\n";
    stream << "VolumeDown=Down\n";
    stream << "Mute=M\n";

    file.close();
}