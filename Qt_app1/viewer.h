
#ifndef VIEWER_H
#define VIEWER_H


#include <QGLWidget>
#include <QGLShaderProgram>
#include "viewerMesh.h"

//! [0]
class GlWidget : public QGLWidget
{
    //! [0]
    Q_OBJECT

public:
    GlWidget(QWidget *parent = 0);
    ~GlWidget();
    QSize sizeHint() const;

    std::string meshfile = "";
    std::string textfile = "";

    ViewerMesh * &v_mesh() { return vMesh; }
    

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    //! [1]
private:
    //! [1]
    QMatrix4x4 pMatrix;
    QGLShaderProgram shaderProgram;
    QVector<QVector3D> vertices;
    //! [2]
    QVector<QVector2D> textureCoordinates;
    GLuint texture;
    //! [2]
    double alpha;
    double beta;
    double distance;
    QPoint lastMousePosition;
    //! [3]
    ViewerMesh * vMesh;
};
//! [3]

#endif // VIEWER_H
