/**
 * @file MainWindow.cpp
 * @author Yaotian Liu
 * @brief MainWindow implementation
 * @date 2022-10-10
 */

#include "MainWindow.h"

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

    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), "Text File (*.txt)");
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
