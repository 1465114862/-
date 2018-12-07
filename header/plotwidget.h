#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include "calculator.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class plotWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    plotWidget(Calculator *icalculate,std::vector<std::string> itoPlot,std::string ip1,std::string ip2,referenceOrigin iorigin,referenceRotate irotate,QWidget *parent = 0);
    ~plotWidget();
    static bool isTransparent() { return m_transparent; }
    static void setTransparent(bool t) { m_transparent = t; }
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
public slots:
    void cleanup();
    void reference(std::string ip1,std::string ip2,referenceOrigin iorigin,referenceRotate irotate);
    void rePlot();
    void rePlotResult();
signals:
    void rePlotResultReady(bool);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    int p1,p2;
    referenceRotate rotate;
    referenceOrigin origin;
    void resetScaleAndShift();
    std::vector<double> scaleAndShift;
    std::vector<std::string> toPlot;
    Calculator *calculate;
    void resetBuffer();
    void updateVBO(GLfloat* ptr);
    void setupVertexAttribs();
    void disableVertexAttribs();
    QQuaternion rotation;
    bool m_core;
    QPoint m_lastPos;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    static bool m_transparent;
    GLfloat *color;
};

#endif // PLOTWIDGET_H
