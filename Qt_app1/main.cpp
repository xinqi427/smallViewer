
#include <QApplication>
#include "viewer.h"
#include "viewerMesh.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlWidget w;
    w.meshfile = argv[1];
    w.textfile = argv[2];
    w.v_mesh()->input_obj(w.meshfile);
    w.show();
    return a.exec();
}
