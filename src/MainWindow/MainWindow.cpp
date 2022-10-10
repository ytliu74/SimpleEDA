/**
 * @file MainWindow.cpp
 * @author Yaotian Liu
 * @brief MainWindow implementation
 * @date 2022-10-10
 */

#include "MainWindow.h"

#include <iostream>

#include <QDebug>
#include <QDir>
#include <QLabel>
#include <QtWidgets>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("MainWindow"));

    text = new QTextEdit(this);
    setCentralWidget(text);

    createActions();
    createMenus();
    createToolBars();

    resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::createActions() {

    /// @brief New File
    actionNewFile = new QAction(QIcon(":/images/new_file.svg"), tr("New"), this);
    actionNewFile->setShortcut(Qt::CTRL + Qt::Key_N);
    actionNewFile->setStatusTip(tr("New file"));
    connect(actionNewFile, SIGNAL(triggered()), this, SLOT(slotNewFile()));

    /// @brief Open File
    actionOpenFile = new QAction(QIcon(":/images/open_file.svg"), tr("Open"), this);
    actionOpenFile->setShortcut(Qt::CTRL + Qt::Key_O);
    actionOpenFile->setStatusTip(tr("Open file"));
    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenFile()));

    /// @brief Save File
    actionSaveFile = new QAction(QIcon(":/images/save_file.svg"), tr("Save"), this);
    actionSaveFile->setShortcut(Qt::CTRL + Qt::Key_S);
    actionSaveFile->setStatusTip(tr("Save file"));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(slotSaveFile()));

    /// @brief SPICE Parser
    actionParser = new QAction(tr("Parser"), this);
    actionParser->setStatusTip(tr("SPICE Parser"));
    connect(actionParser, SIGNAL(triggered()), this, SLOT(slotParser()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(actionNewFile);
    fileMenu->addSeparator(); /// Add separator between 2 actions.
    fileMenu->addAction(actionOpenFile);
    fileMenu->addAction(actionSaveFile);
}

void MainWindow::createToolBars() {
    fileTool = addToolBar(tr("File"));

    fileTool->addAction(actionNewFile);
    fileTool->addAction(actionOpenFile);
    fileTool->addAction(actionSaveFile);

    parserTool = addToolBar(tr("Parser"));

    parserTool->addAction(actionParser);
}

/**
 * @brief New action will cover and create a new textedit.
 * @author Deng Qiyu
 * @date 2022/08/04
 */
void MainWindow::slotNewFile()
{
    text->clear();          /// Clear the text
    text->setHidden(false); /// Display the text.
}

/**
 * @brief Open action will open the saved files .
 * @author Deng Qiyu
 * @date 2022/08/04
 */
void MainWindow::slotOpenFile()
{

    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), "SPICE (*.sp)");
    /// If the dialog is directly closed, the filename will be null.
    if (fileName == "") {
        return;
    }
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file!"));
            return;
        }
        else {
            if (!file.isReadable()) {
                QMessageBox::warning(this, tr("Error"), tr("The file is unreadable"));
            }
            else {
                QTextStream textStream(&file); // Use QTextStream to load text.
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
void MainWindow::slotSaveFile()
{

    statusBar()->showMessage(tr("Saving file..."));

    if (fileName == "") /// File has not been saved.
    {
        /// Text is empty.
        if (text->toPlainText() == "") {
            QMessageBox::warning(this, tr("Warning"), tr("Content cannot be empty!"),
                QMessageBox::Ok);
        }
        else {
            QFileDialog fileDialog;
            fileName = fileDialog.getSaveFileName(this, tr("Open File"), "./", "Text File(*.txt)");
            if (fileName == "") {
                return;
            }
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to open file!"),
                    QMessageBox::Ok);
                return;
            }
            else {
                /// Create a text stream and pass text in.
                QTextStream textStream(&file);
                QString     str = text->toPlainText();
                textStream << str;
            }
            file.close();
        }
    }
    else { /// File has been saved.
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Warning"), tr("Failed to open file!"));
            return;
        }
        else {
            QTextStream textStream(&file);
            QString     str = text->toPlainText();
            textStream << str;
            file.close();
        }
    }
}

/// @brief SPICE parser implementation
void MainWindow::slotParser() {
    std::cout << "Entering parser" << std::endl;
    std::cout << "fileName: " << fileName.toStdString().data() << std::endl;

    Vsrc_vec.clear();
    Res_vec.clear();
    Cap_vec.clear();
    Ind_vec.clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Load the content in SPICE file failed."),
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
            printQString("Parsed Title: ", title);
        }
        else
            if (line.size() > 0) {
                if (line.startsWith("*"))
                    printQString("Parsed Annotation: ", line);
                else {
                    line = line.toLower(); // SPICE is case-insensistive
                    if (line.startsWith("."))
                        commandParser(line, lineCount);
                    else
                        deviceParser(line, lineCount);
                }
            }
    }
}

/**
 * @brief Easy way to print a QString.
 *
 * @param comment The comment ahead of the QString.
 * @param qstring The target QString.
 */
void MainWindow::printQString(const char* comment, const QString qstring) {
    std::cout << comment << qstring.toStdString().data() << std::endl;
}

void MainWindow::deviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    std::string deviceName = elements[0].toStdString().data();

    // Process Voltage Source
    if (line.startsWith("V") | line.startsWith("v")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            float value = elements[3].toFloat();
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Vsrc vsrc = { deviceName, value, node_1, node_2 };
            Vsrc_vec.push_back(vsrc);

            std::cout << "Parsed Device Type: Voltage Source ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // Process Resistor
    else if (line.startsWith("R") | line.startsWith("r")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            float value = elements[3].toFloat();
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Res res = { deviceName, value, node_1, node_2 };
            Res_vec.push_back(res);

            std::cout << "Parsed Device Type: Resistor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // Process Capacitor
    else if (line.startsWith("C") | line.startsWith("c")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            float value = elements[3].toFloat();
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Cap cap = { deviceName, value, node_1, node_2 };
            Cap_vec.push_back(cap);

            std::cout << "Parsed Device Type: Capacitor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // TODO: Inducer
}

void MainWindow::commandParser(const QString line, const int lineNum) {
    std::cout << "Enter Command Parser" << std::endl;
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    if (num_elements == 1) {
        if (elements[0] == ".op")
            std::cout << "Parsed Analysis Command .OP Token" << std::endl;
        else if (elements[0] == ".end")
            std::cout << "Parsed .END Token" << std::endl;
    } else {}
}