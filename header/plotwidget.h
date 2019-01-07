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
    plotWidget(Calculator *icalculate,std::vector<std::string> itoPlot,std::string ip1,std::string ip2,referenceOrigin iorigin,referenceRotate irotate,double itmin,QWidget *parent = 0);
    ~plotWidget();
    static bool isTransparent() { return m_transparent; }
    static void setTransparent(bool t) { m_transparent = t; }
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void setSolver(int istep,double itolerance,double itmin,double itmax,int imethod);
    void setReference(std::string ip1,std::string ip2,std::string ip3,referenceOrigin iorigin,referenceRotate irotate);
public slots:
    void cleanup();
    void reference();
    void rePlot();
    void rePlotResult();
    void resetScaleAndShiftHint();
    void resetScaleAndShiftByName(std::string name);
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
    std::string p1,p2;
    referenceRotate rotate;
    referenceOrigin origin;
    void resetScaleAndShift();
    std::vector<double> scaleAndShift;
    std::vector<std::string> toDeletePlot;
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
    GLfloat *color{nullptr};
    int step,method;
    double tmax,tolerance,tmin;
    void solve();
};

#endif // PLOTWIDGET_H
