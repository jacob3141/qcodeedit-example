
#include "window.h"

#include <cstdio>

#include <QApplication>


int main (int argc, char **argv)
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("QCodeEdit::Demo");
    QApplication::setOrganizationName("Luc BRUANT aka fullmetalcoder");
    QApplication::setOrganizationDomain("qcodeedit.edyuk.org");

    Window window;

    if ( argc > 1 )
        window.load(argv[argc - 1]);

    window.show();

    int ret = app.exec();

    return ret;
}
