#include "viewer.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>
//! [0]
#ifdef WIN32
#include <GL/glext.h>
PFNGLACTIVETEXTUREPROC pGlActiveTexture = NULL;
#define glActiveTexture pGlActiveTexture
#endif //WIN32
//! [0]

GlWidget::GlWidget(QWidget *parent)
    : QGLWidget(QGLFormat(/* Additional format options */), parent)
{
    alpha = 0;
    beta = 0;
    distance = 2.5;
    vMesh = new ViewerMesh();
}

GlWidget::~GlWidget()
{
}

QSize GlWidget::sizeHint() const
{
    return QSize(640, 480);
}

//! [1]
void GlWidget::initializeGL()
{
    //! [1]
    //! [2]
#ifdef WIN32
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress((LPCSTR) "glActiveTexture");
#endif
    //! [2]

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    qglClearColor(QColor(Qt::white));

    shaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/vertexShader.vsh");
    shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/fragmentShader.fsh");
    shaderProgram.link();

    // input vertices positions and uv coords.
    for (CFace * pf : vMesh->m_mesh()->faces())
    {
        for (CHalfEdge * phe : pf->halfedges())
        {
            CVertex * pv = phe->vertex();
            QVector3D pos;
            QVector2D qt_uv;
            for (int i = 0; i < 3; i++)
            {
                pos[i] = pv->point()[i];
            }
            for (int j = 0; j < 2; j++)
            {
                qt_uv[j] = phe->uv()[j];
            }
            vertices.push_back(pos);
            textureCoordinates.push_back(qt_uv);
        }
    }
    const QString txfile = QString::fromStdString(textfile);
    
    texture = bindTexture(QPixmap(txfile));

}


void GlWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float)width / (float)height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

//! [5]
void GlWidget::paintGL()
{
    //! [5]
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mMatrix;
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(alpha, 0, 1, 0);
    cameraTransformation.rotate(beta, 1, 0, 0);

     QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
     QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
    // QVector3D cameraPosition = QVector3D(0, 0, 2.5);
    // QVector3D cameraUpDirection = QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    //! [6]
    shaderProgram.bind();

    shaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);

    shaderProgram.setUniformValue("texture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(0);

    shaderProgram.setAttributeArray("vertex", vertices.constData());
    shaderProgram.enableAttributeArray("vertex");

    shaderProgram.setAttributeArray("textureCoordinate", textureCoordinates.constData());
    shaderProgram.enableAttributeArray("textureCoordinate");

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    shaderProgram.disableAttributeArray("vertex");

    shaderProgram.disableAttributeArray("textureCoordinate");

    shaderProgram.release();
}
//! [6]

void GlWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePosition = event->pos();

    event->accept();
}

void GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = event->x() - lastMousePosition.x();
    int deltaY = event->y() - lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        alpha -= deltaX;
        while (alpha < 0) {
            alpha += 360;
        }
        while (alpha >= 360) {
            alpha -= 360;
        }

        beta -= deltaY;
        /*if (beta < -90) {
        beta = -90;
        }
        if (beta > 90) {
        beta = 90;
        }*/

        updateGL();
    }

    lastMousePosition = event->pos();

    event->accept();
}

void GlWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            distance *= 1.1;
        }
        else if (delta > 0) {
            distance *= 0.9;
        }

        updateGL();
    }

    event->accept();
}
