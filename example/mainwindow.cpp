/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
** Copyright (C) 2018 Jacob Dawid <jada@posteo.de>
**
** This file is part of the Edyuk project <http://edyuk.org>
**
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

// Own includes
#include "mainwindow.h"

// qcodeedit includes
#include "qsnippet.h"
#include "qsnippetmanager.h"
#include "qsnippetbinding.h"

#include "qpanel.h"
#include "qeditor.h"
#include "qcodeedit.h"
#include "qeditconfig.h"
#include "qeditsession.h"
#include "qformatconfig.h"

#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qformatscheme.h"
#include "qlanguagefactory.h"

#include "qlinemarksinfocenter.h"

#include "qlinemarkpanel.h"
#include "qlinenumberpanel.h"
#include "qfoldpanel.h"
#include "qlinechangepanel.h"
#include "qstatuspanel.h"
#include "qgotolinepanel.h"
#include "qsearchreplacepanel.h"

// Qt includes
#include <QDir>
#include <QTime>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *p)
 : QMainWindow(p),
   ui(new Ui::MainWindow) {
    Q_INIT_RESOURCE(qcodeedit);

    ui->setupUi(this);

    // QCE setup
    m_formats = new QFormatScheme(":/qxs/formats.qxf", this);
    QDocument::setDefaultFormatScheme(m_formats);

    QLineMarksInfoCenter::instance()->loadMarkTypes(":/qxs/marks.qxm");

    m_session = new QEditSession("session", this);
    connect(m_session	, SIGNAL( restored(QEditor*) ),
            this		, SLOT  ( restored(QEditor*) ) );

    m_languages = new QLanguageFactory(m_formats, this);
    m_languages->addDefinitionPath("qxs");

//    // create toolbars
//    m_edit = new QToolBar(tr("Edit"), this);
//    m_edit->setIconSize(QSize(24, 24));
//    m_edit->addAction(_codeEdit->editor()->action("undo"));
//    m_edit->addAction(_codeEdit->editor()->action("redo"));
//    m_edit->addSeparator();
//    m_edit->addAction(_codeEdit->editor()->action("cut"));
//    m_edit->addAction(_codeEdit->editor()->action("copy"));
//    m_edit->addAction(_codeEdit->editor()->action("paste"));
//    m_edit->addSeparator();
//    m_edit->addAction(_codeEdit->editor()->action("indent"));
//    m_edit->addAction(_codeEdit->editor()->action("unindent"));
//    addToolBar(m_edit);
//    m_edit->hide();

//    m_find = new QToolBar(tr("Find"), this);
//    m_find->setIconSize(QSize(24, 24));
//    m_find->addAction(_codeEdit->editor()->action("find"));
//    m_find->addAction(_codeEdit->editor()->action("replace"));
//    m_find->addAction(_codeEdit->editor()->action("goto"));
//    addToolBar(m_find);
//    m_find->hide();


    updateRecentFiles();

    m_session->restore();

    on_action_New_triggered();
}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent *e) {
    if(maybeSave()) {
        e->ignore();
        return;
    }

    e->accept();

    // update session data and save it
    m_session->updateData();
}

QEditor *MainWindow::createEditor() {
    QCodeEdit *_codeEdit = new QCodeEdit(this);
    _codeEdit
        ->addPanel(new QLineMarkPanel, QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F6"));

    _codeEdit
        ->addPanel(new QLineNumberPanel, QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F11"));

    _codeEdit
        ->addPanel(new QFoldPanel, QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F9"));

    _codeEdit
        ->addPanel(new QLineChangePanel, QCodeEdit::West, true)
        ; //->setShortcut(QKeySequence("F11"));

    _codeEdit
        ->addPanel(new QStatusPanel, QCodeEdit::South, true);

    _codeEdit
        ->addPanel(new QGotoLinePanel, QCodeEdit::South, true);

    _codeEdit
        ->addPanel(new QSearchReplacePanel, QCodeEdit::South, true);

    connect(
        _codeEdit->editor(), SIGNAL( contentModified(bool) ),
            this					, SLOT  ( setWindowModified(bool) ) );

    m_session->addEditor(_codeEdit->editor());

    return _codeEdit->editor();
}

QEditor *MainWindow::activeEditor() {
    return dynamic_cast<QEditor*>(ui->mdiArea->activeSubWindow()->widget());
}

bool MainWindow::maybeSave() {
    if(activeEditor()->isContentModified()) {
        int ret = QMessageBox::warning(
                            this,
                            tr("About to quit"),
                            tr(
                                "The open document contains unsaved modifications.\n"
                                "Save it as %1 ?"
                            ).arg(activeEditor()->fileName()),
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                            QMessageBox::Yes
                        );

        if ( ret == QMessageBox::Cancel ) {
            return true;
        } else if ( ret == QMessageBox::Yes ) {
            activeEditor()->save();
        }
    }

    return false;
}

void MainWindow::load(const QString& file) {
    QEditor *editor = createEditor();
    ui->mdiArea->addSubWindow(editor)->showMaximized();

    QString filename = file.count() ? QFileInfo(file).absoluteFilePath() : file;

    if ( filename.count() && QFile::exists(filename) ) {
        m_languages->setLanguage(editor, filename);
        editor->load(filename);

        updateRecentFiles(filename);

        setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(filename));
    } else {
        m_languages->setLanguage(editor, "test.cpp");
        editor->setFileName(""); // Loading failed\n// fallback to C++\n");
        editor->setText(""); // Loading failed\n// fallback to C++\n");

        setWindowTitle("QCodeEdit::Demo [untitled[*]]");
    }
}

void MainWindow::on_action_New_triggered() {
    load(QString());
}

void MainWindow::on_action_Open_triggered() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open file...",
        QDir::currentPath(),
        m_languages->fileFilters().join(";;"),
        nullptr,
        nullptr
    );

    if(filename.count()) {
        load(filename);
    }
}

void MainWindow::on_action_Save_triggered() {
    QEditor *editor = activeEditor();
    QString old = editor->fileName();
    editor->save();

    QString file = editor->fileName();

    if(file != old) {
        m_languages->setLanguage(editor, file);
        setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(file));
    }
}

void MainWindow::on_action_SaveAs_triggered() {
    QString fn = QFileDialog::getSaveFileName(
        this,
        tr("Save file as..."),
        QDir::currentPath(),
        m_languages->fileFilters().join(";;")
    );

    if(fn.isEmpty()) {
        return;
    }

    activeEditor()->save(fn);
    m_languages->setLanguage(activeEditor(), fn);
    setWindowTitle(QString("QCodeEdit::Demo [%1[*]]").arg(fn));
}

void MainWindow::on_action_Print_triggered() {
    QEditor *editor = dynamic_cast<QEditor*>(ui->mdiArea->activeSubWindow()->widget());
    editor->print();
}

void MainWindow::on_action_About_Qt_triggered() {
    QMessageBox::aboutQt(this, "About Qt 4");
}

void MainWindow::on_menu_Recent_triggered(QAction *a) {
    if ( !a )
        return;

    if ( a == ui->action_Clear )
    {
        QSettings settings;
        settings.beginGroup("files");
        settings.setValue("recent_list", QStringList());
        settings.endGroup();
        updateRecentFiles();
        return;
    }

    if ( maybeSave() )
        return;

    load(a->objectName());
}

void MainWindow::updateRecentFiles(const QString& filename) {
    QSettings settings;
    settings.beginGroup("files");

    int max = settings.value("max_recent", 10).toInt();
    QStringList l = settings.value("recent_list").toStringList();

    ui->menu_Recent->removeAction(ui->action_Clear);
    ui->menu_Recent->clear();
    ui->menu_Recent->addAction(ui->action_Clear);
    ui->menu_Recent->addSeparator();

    if ( filename.count() )
    {
        l.removeAll(filename);
        l.prepend(filename);
    }

    while ( l.count() > max )
    {
        QString fn = l.takeLast();
    }

    int count = 0;

    foreach ( QString fn, l )
    {
        QString label = QString("%1 [%2]").arg(QFileInfo(fn).fileName()).arg(fn);

        if ( count < 10 )
        {
            label.prepend(' ');
            label.prepend('0' + count);
            label.prepend('&');
        }

        QAction *a = new QAction(label, ui->menu_Recent);
        a->setObjectName(fn);

        ui->menu_Recent->addAction(a);
        ++count;
    }

    settings.setValue("recent_list", l);
    settings.endGroup();
}

