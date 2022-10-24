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

#include "../utils/utils.h"

using std::cout;
using std::endl;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Simple EDA"));

    CreateActions();
    CreateMenus();
    CreateLayout();
    CreateToolBars();

    resize(1200, 900);
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

    action_analyzer = new QAction(tr("Analyzer"), this);
    action_analyzer->setStatusTip(tr("SPICE Analyzer"));
    connect(action_analyzer, SIGNAL(triggered()), this, SLOT(SlotAnalyzer()));
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

    analysis_tool = addToolBar(tr("SPICE Analysis"));

    analysis_tool->addAction(action_parser);
    analysis_tool->addAction(action_analyzer);
}

void MainWindow::CreateLayout() {
    main_widget = new QWidget(this);
    text = new QTextEdit(main_widget);
    output = new QTextEdit(main_widget);
    output->setReadOnly(true);
    output->append(tr("╔═══════════════╗"));
    output->append(tr("║  Output Window  ║"));
    output->append(tr("╚═══════════════╝"));
    output->append(tr("  "));

    main_layout = new QHBoxLayout();
    main_layout->addWidget(text);
    main_layout->addWidget(output);
    main_layout->setStretchFactor(text, 2);
    main_layout->setStretchFactor(output, 3);

    main_widget->setLayout(main_layout);

    setCentralWidget(main_widget);
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
    cout << "==============================" << endl;
    output->append(tr("=============================="));
    cout << "Entering parser" << endl;
    output->append(tr("Entering parser"));
    cout << "file_name: " << file_name << endl;
    output->append(tr("file_name: ") + file_name);

    parser = Parser(output);

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
            cout << "Parsed Title: " << title << endl;
            output->append(tr("Parsed Title: ") + title);
        } else if (line.size() > 0) {
            if (line.startsWith("*")) {
                cout << "Parsed Annotation: " << line << endl;
                output->append(tr("Parsed Annotation: ") + line);
            } else {
                line = line.toLower();  // SPICE is case-insensistive
                if (line.startsWith("."))
                    parser.CommandParser(line, lineCount);
                else
                    parser.DeviceParser(line, lineCount);
            }
        }
    }
    cout << "------ Summary ------" << endl;
    cout << "Device: "
         << parser.GetResistor().size() + parser.GetInductor().size() +
                parser.GetCapacitor().size()
         << endl;
    cout << "R: " << parser.GetResistor().size() << "  "
         << "L: " << parser.GetInductor().size() << "  "
         << "C: " << parser.GetCapacitor().size() << endl;
    cout << "Vsrc: " << parser.GetVsrc().size() << endl;
    cout << "Node: " << parser.GetNode().size() << endl;

    if (!parser.ParserFinalCheck()) {
        cout << "Parser check failed" << endl;
        QMessageBox::warning(this, tr("Error"), tr("Parser check failed."),
                             QMessageBox::Ok);
        return;
    }
}

// @brief SPICE Analyzer
void MainWindow::SlotAnalyzer() {
    analyzer = Analyzer(parser);
    auto result_mat = analyzer.GetAnalysisResults();
    for (auto mat : result_mat) {
        analyzer.PrintMatrix(mat.node_metrix, mat.node_vec);
        analyzer.PrintRHS(mat.rhs, mat.node_vec);
    }
}
