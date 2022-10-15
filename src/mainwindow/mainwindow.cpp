/**
 * @file MainWindow.cpp
 * @author Yaotian Liu
 * @brief MainWindow implementation
 * @date 2022-10-10
 */

#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QLabel>
#include <QtWidgets>
#include <iostream>

#include "parser/parser.h"
#include "utils/utils.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Simple EDA"));

    text = new QTextEdit(this);
    setCentralWidget(text);

    CreateActions();
    CreateMenus();
    CreateToolBars();

    resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::CreateActions() {
    /// @brief New File
    action_new_file = new QAction(QIcon(":/images/new_file.svg"), tr("New"), this);
    action_new_file->setShortcut(Qt::CTRL + Qt::Key_N);
    action_new_file->setStatusTip(tr("New file"));
    connect(action_new_file, SIGNAL(triggered()), this, SLOT(SlotNewFile()));

    /// @brief Open File
    action_open_file = new QAction(QIcon(":/images/open_file.svg"), tr("Open"), this);
    action_open_file->setShortcut(Qt::CTRL + Qt::Key_O);
    action_open_file->setStatusTip(tr("Open file"));
    connect(action_open_file, SIGNAL(triggered()), this, SLOT(SlotOpenFile()));

    /// @brief Save File
    action_save_file = new QAction(QIcon(":/images/save_file.svg"), tr("Save"), this);
    action_save_file->setShortcut(Qt::CTRL + Qt::Key_S);
    action_save_file->setStatusTip(tr("Save file"));
    connect(action_save_file, SIGNAL(triggered()), this, SLOT(SlotSaveFile()));

    /// @brief SPICE Parser
    action_parser = new QAction(tr("Parser"), this);
    action_parser->setStatusTip(tr("SPICE Parser"));
    connect(action_parser, SIGNAL(triggered()), this, SLOT(SlotParser()));
}

void MainWindow::CreateMenus() {
    file_menu = menuBar()->addMenu(tr("File"));
    file_menu->addAction(action_new_file);
    file_menu->addSeparator();  /// Add separator between 2 actions.
    file_menu->addAction(action_open_file);
    file_menu->addAction(action_save_file);
}

void MainWindow::CreateToolBars() {
    file_tool = addToolBar(tr("File"));

    file_tool->addAction(action_new_file);
    file_tool->addAction(action_open_file);
    file_tool->addAction(action_save_file);

    parser_tool = addToolBar(tr("Parser"));

    parser_tool->addAction(action_parser);
}

/**
 * @brief New action will cover and create a new textedit.
 * @author Deng Qiyu
 * @date 2022/08/04
 */
void MainWindow::SlotNewFile() {
    text->clear();           /// Clear the text
    text->setHidden(false);  /// Display the text.
}

/**
 * @brief Open action will open the saved files .
 * @author Deng Qiyu
 * @date 2022/08/04
 */
void MainWindow::SlotOpenFile() {
    file_name =
        QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), "SPICE (*.sp)");
    /// If the dialog is directly closed, the filename will be null.
    if (file_name == "") {
        return;
    } else {
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file!"));
            return;
        } else {
            if (!file.isReadable()) {
                QMessageBox::warning(this, tr("Error"), tr("The file is unreadable"));
            } else {
                QTextStream textStream(&file);  // Use QTextStream to load text.
                while (!textStream.atEnd()) {
                    text->setPlainText(textStream.readAll());
                }
                text->show();
                file.close();
            }
        }
    }
}

/**
 * @brief Save action will save the text into a new file when it has not been
 * saved. Otherwise it will be saved in the current file.
 * @author Deng Qiyu
 * @date 2022/08/04
 */
void MainWindow::SlotSaveFile() {
    statusBar()->showMessage(tr("Saving file..."));

    if (file_name == "")  /// File has not been saved.
    {
        /// Text is empty.
        if (text->toPlainText() == "") {
            QMessageBox::warning(this, tr("Warning"), tr("Content cannot be empty!"),
                                 QMessageBox::Ok);
        } else {
            QFileDialog fileDialog;
            file_name = fileDialog.getSaveFileName(this, tr("Open File"), "./",
                                                   "Text File(*.txt)");
            if (file_name == "") {
                return;
            }
            QFile file(file_name);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to open file!"),
                                     QMessageBox::Ok);
                return;
            } else {
                /// Create a text stream and pass text in.
                QTextStream textStream(&file);
                QString str = text->toPlainText();
                textStream << str;
            }
            file.close();
        }
    } else {  /// File has been saved.
        QFile file(file_name);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Warning"), tr("Failed to open file!"));
            return;
        } else {
            QTextStream textStream(&file);
            QString str = text->toPlainText();
            textStream << str;
            file.close();
        }
    }
}

/// @brief SPICE parser implementation
void MainWindow::SlotParser() {
    std::cout << "==============================" << std::endl;
    std::cout << "Entering parser" << std::endl;
    std::cout << "file_name: " << file_name << std::endl;

    Parser parser;

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Load the content in SPICE file failed."),
                             QMessageBox::Ok);
        return;
    }
    QTextStream textStream(&file);

    QString title;

    int lineCount = 0;
    while (!textStream.atEnd()) {
        QString line = textStream.readLine();
        lineCount++;
        if (lineCount == 1) {
            title = line;
            std::cout << "Parsed Title: " << title << std::endl;
        } else if (line.size() > 0) {
            if (line.startsWith("*"))
                std::cout << "Parsed Annotation: " << line << std::endl;
            else {
                line = line.toLower();  // SPICE is case-insensistive
                if (line.startsWith("."))
                    parser.CommandParser(line, lineCount);
                else
                    parser.DeviceParser(line, lineCount);
            }
        }
    }
    std::cout << "------ Summary ------" << std::endl;
    std::cout << "Device: "
              << parser.GetResistor().size() + parser.GetInductor().size() +
                     parser.GetCapacitor().size()
              << std::endl;
    std::cout << "R: " << parser.GetResistor().size() << "  "
              << "L: " << parser.GetInductor().size() << "  "
              << "C: " << parser.GetCapacitor().size() << std::endl;
    std::cout << "Vsrc: " << parser.GetVsrc().size() << std::endl;
    std::cout << "Node: " << parser.GetNode().size() << std::endl;

    if (!parser.ParserFinalCheck()) {
        std::cout << "Parser check failed" << std::endl;
    }
}
