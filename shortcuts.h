#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include <QObject>
#include <QShortcut>
#include <QMap>

class MainWindow;

class Shortcuts : public QObject
{
    Q_OBJECT

public:
    explicit Shortcuts(MainWindow *window);

private:
    void loadShortcuts();
    void createDefaultFile();

    MainWindow *m_window;
    QMap<QString, QShortcut*> m_shortcuts;
};

#endif