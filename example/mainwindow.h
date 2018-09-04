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

#pragma once

#include <QString>
#include <QMainWindow>
#include "ui_window.h"

class QEditor;
class QEditConfig;
class QEditSession;
class QFormatConfig;
class QFormatScheme;
class QLanguageFactory;

class QSnippetBinding;
class QSnippetManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *p = nullptr);
    virtual ~MainWindow();

public slots:
    void load(const QString& file);

protected:
    virtual void closeEvent(QCloseEvent *e);

    QEditor *createEditor();
    QEditor *activeEditor();

private slots:
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_SaveAs_triggered();
    void on_action_Print_triggered();

    void on_action_About_Qt_triggered();

    void on_menu_Recent_triggered(QAction *a);

private:
    bool maybeSave();

    void updateRecentFiles(const QString& filename = QString());

    QEditSession *m_session;

    QEditConfig *m_config;
    QFormatConfig *m_formatConfig;

    QFormatScheme *m_formats;
    QLanguageFactory *m_languages;

    QToolBar *m_edit, *m_find;

    Ui::MainWindow *ui;
};
