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

struct Vsrc {
    std::string name;
    float value;
    std::string node_1;
    std::string node_2;
};

struct Res {
    std::string name;
    float value;
    std::string node_1;
    std::string node_2;
};

struct Cap {
    std::string name;
    float value;
    std::string node_1;
    std::string node_2;
};

struct Ind {
    std::string name;
    float value;
    std::string node_1;
    std::string node_2;
};

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

    void slotParser();

private:
    /// @brief file
    QMenu* fileMenu;

    QToolBar* fileTool;

    QAction* actionNewFile;
    QAction* actionOpenFile;
    QAction* actionSaveFile;

    /// @brief parser
    QToolBar* parserTool;

    QAction* actionParser;

    QTextEdit* text;

    QString fileName = "./";

    void printQString(const char* comment, const QString qstring);
    void deviceParser(const QString line, const int lineNum);
    void commandParser(const QString line, const int lineNum);

    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;
};

#endif