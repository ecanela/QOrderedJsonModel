#include <QApplication>
#include <QTreeView>

#include "qorderedjsonmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString filePath("E:/default.txt");

    QOrderedJsonModel model;
    model.loadFile(filePath);

    QTreeView view;
    view.setGeometry(200, 200, 480, 640);
    view.setModel(&model);
    view.setWindowTitle(QObject::tr("Simple Json  Model"));
    view.show();
    return app.exec();

}
