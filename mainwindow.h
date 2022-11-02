#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QGuiApplication>
#include <QSaveFile>
#include <QTextStream>
#include <QApplication>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void createNewFile();
    bool saveFile();
    bool saveAs();
    void openFile();

private:
    void CreateMenus();
    bool mayBeSave();
    bool save(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void about();

private:
    QPlainTextEdit *m_pTextEdit;

    QString m_currentFile;
};
#endif
