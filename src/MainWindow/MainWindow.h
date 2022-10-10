/**
 * @file MainWindow.h
 * @author Yaotian Liu
 * @brief Header file of MainWindow
 * @date 2022-10-10
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QAction;
class QMenu;
class QToolBar;
class QTextEdit;
class QWidget;
class QTextStream;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void createMenus();
    void createActions();
    void createToolBars();

public slots:
    void slotNewFile();
    void slotOpenFile();
    void slotSaveFile();

private:
    QMenu* fileMenu;
    QToolBar* fileTool;

    QAction* actionNewFile;
    QAction* actionOpenFile;
    QAction* actionSaveFile;

    QTextEdit* text;

    QString fileName = "./";

};

#endif