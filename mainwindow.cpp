#include "mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(1920, 1080);

    m_pTextEdit = new QPlainTextEdit(this);
    this->setCentralWidget(m_pTextEdit);

    this->CreateMenus();
}

MainWindow::~MainWindow()
{

}

void MainWindow::CreateMenus()
{
    //菜单栏
    QMenu *pFileMenu = menuBar()->addMenu("文件");
    //工具栏
    QToolBar *pFileToolBar = addToolBar("文件");
    QAction *pNewAct = new QAction(QIcon(":/images/new.png"), "新建", this);
    //添加快捷键
    pNewAct->setShortcut(QKeySequence::New);
    //状态栏提示
    pNewAct->setStatusTip("创建新文件");
    connect(pNewAct,&QAction::triggered, this, &MainWindow::createNewFile);

    pFileMenu->addAction(pNewAct);
    pFileToolBar->addAction(pNewAct);

    QAction *pOpenAct = new QAction(QIcon(":/images/open.png"), "打开", this);
    pOpenAct->setShortcut(QKeySequence::Open);
    pOpenAct->setStatusTip("打开现有文件");
    connect(pOpenAct, &QAction::triggered, this, &MainWindow::openFile);

    pFileMenu->addAction(pOpenAct);
    pFileToolBar->addAction(pOpenAct);

    QAction *pSaveAct = new QAction(QIcon(":/images/save.png"), "保存", this);
    pSaveAct->setShortcut(QKeySequence::Save);
    pSaveAct->setStatusTip("将文档保存到磁盘");
    connect(pSaveAct, &QAction::triggered, this, &MainWindow::saveFile);

    pFileMenu->addAction(pSaveAct);
    pFileToolBar->addAction(pSaveAct);

    QAction *pSaveAsAct = pFileMenu->addAction("另存为...", this, &MainWindow::saveAs);
    pSaveAsAct->setStatusTip("以新名称保存文档");

    //添加分割线
    pFileMenu->addSeparator();

   QAction *pExitAct = pFileMenu->addAction("退出", this, &QWidget::close);
   pExitAct->setShortcut(QKeySequence::Quit);
   pExitAct->setStatusTip("退出应用程序");

   QMenu *pEditMenu = menuBar()->addMenu("编辑");
   QToolBar *pEditToolBar = addToolBar("编辑");

   QAction *pCutAct = new QAction(QIcon(":images/cut.png"), "剪切", this);
   pCutAct->setShortcut(QKeySequence::Cut);
   pCutAct->setStatusTip("将当前选择的内容剪切到剪贴板");
   connect(pCutAct, &QAction::triggered, m_pTextEdit, &QPlainTextEdit::cut);

   pEditMenu->addAction(pCutAct);
   pEditToolBar->addAction(pCutAct);

   QAction *pCopyAct = new QAction(QIcon(":/images/copy.png"), "拷贝", this);
   pCopyAct->setShortcut(QKeySequence::Copy);
   pCopyAct->setStatusTip("将当前选择的内容复制到剪贴板");
   connect(pCopyAct, &QAction::triggered, m_pTextEdit, &QPlainTextEdit::copy);

   pEditMenu->addAction(pCopyAct);
   pEditToolBar->addAction(pCopyAct);

   QAction *pPasteAct = new QAction(QIcon(":/images/paste.png"), "粘贴", this);
   pPasteAct->setShortcut(QKeySequence::Paste);
   pPasteAct->setStatusTip("将剪贴板的内容粘贴到当前选择内");
   connect(pPasteAct, &QAction::triggered, m_pTextEdit, &QPlainTextEdit::paste);

   pEditMenu->addAction(pPasteAct);
   pEditToolBar->addAction(pPasteAct);

   menuBar()->addSeparator();

   QMenu *helpMenu = menuBar()->addMenu("帮助");
   QAction *aboutAct = helpMenu->addAction("帮助", this, &MainWindow::about);
   aboutAct->setStatusTip("显示应用程序的关于框");

   QAction *pAboutQtAct = helpMenu->addAction("关于Qt", qApp, &QApplication::aboutQt);
   pAboutQtAct->setStatusTip("显示Qt库的About框");

   pCutAct->setEnabled(false);
   pCopyAct->setEnabled(false);
   connect(m_pTextEdit, &QPlainTextEdit::copyAvailable, pCutAct, &QAction::setEnabled);
   connect(m_pTextEdit, &QPlainTextEdit::copyAvailable, pCopyAct, &QAction::setEnabled);

   //状态栏
   statusBar()->showMessage("已准备");
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void MainWindow::createNewFile()
{
    if (mayBeSave()) {
        m_pTextEdit->clear();
        setCurrentFile(QString());
    }
}

bool MainWindow::mayBeSave()
{
    //检测编辑框中的内容是否修改，若修改则保存
    if (!m_pTextEdit->document()->isModified()) {
        return true;
    }

    auto ret = QMessageBox::warning(this, "错误",
                                    "文档已被修改。要保存更改吗？",
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        return saveFile();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }

    return true;
}



bool MainWindow::saveFile()
{
    if (m_currentFile.isEmpty()) {
        return saveAs();
    } else {
        return save(m_currentFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    return save(dialog.selectedFiles().first());
}

bool MainWindow::save(const QString &fileName)
{
    QString errorMessage;
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << m_pTextEdit->toPlainText();
        if (!file.commit()) {
            errorMessage = QString("不能写入文件 %1:%2.")
                    .arg(QDir::toNativeSeparators(fileName), file.errorString());

        }
    } else {
        errorMessage = QString("不能打开文件 %1 for writing:%2")
                .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }

    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, "错误", errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage("文件已保存", 2000);

    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    m_pTextEdit->document()->setModified(false);
    this->setWindowModified(false);

    QString showName = m_currentFile;
    if (m_currentFile.isEmpty()) {
        showName = "untitled.txt";
    }
    this->setWindowFilePath(showName);
}

void MainWindow::openFile()
{
    if (mayBeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "error",
                             QString("无法读取文件 %1:%2")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
    }

    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    m_pTextEdit->setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    this->setCurrentFile(fileName);
    statusBar()->showMessage("文件已打开", 2000);
}

