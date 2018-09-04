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

#include "mainwindow.h"

#include <QApplication>

int main (int argc, char **argv) {
    QApplication app(argc, argv);

    QApplication::setApplicationName("QCodeEdit::Demo");
    QApplication::setOrganizationName("QCodeEdit Developers");

    MainWindow mainWindow;

    if(argc > 1) {
        mainWindow.load(argv[argc - 1]);
    }

    mainWindow.show();
    return app.exec();
}
