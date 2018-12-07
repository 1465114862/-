#include "plotwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <iostream>
#include <QThread>
#include <QMessageBox>


bool plotWidget::m_transparent = false;

plotWidget::plotWidget(Calculator *icalculate,std::vector<std::string> itoPlot,std::string ip1,std::string ip2,referenceOrigin iorigin,referenceRotate irotate,QWidget *parent)
    : QOpenGLWidget(parent),
      toPlot(itoPlot),
      calculate(icalculate),
      m_program(0),
      origin(iorigin),
      rotate(irotate)
{
    p1=calculate->planetToIndex(ip1);
    p2=calculate->planetToIndex(ip2);
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

plotWidget::~plotWidget()
{
    cleanup();
}

QSize plotWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize plotWidget::sizeHint() const
{
    return QSize(600, 600);
}

void plotWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

void plotWidget::resetScaleAndShift()
{
    for(std::string i : toPlot)
        calculate->getProbe(i)->probeplot->shift(scaleAndShift);
}

void plotWidget::reference(std::string ip1,std::string ip2,referenceOrigin iorigin,referenceRotate irotate){
    /*calculate->probeRelativeOrbit(200,1,6,24,calculate->getProbe(toPlot[0])->EndT,*(calculate->getProbe(toPlot[0])),0.9986295347545738,P1,P1toP2);*/
    p1=calculate->planetToIndex(ip1);
    p2=calculate->planetToIndex(ip2);
    origin=iorigin;
    rotate=irotate;
    calculate->probeRelativeOrbit(200,1,p1,p2,calculate->getProbe(toPlot[0])->EndT,*(calculate->getProbe(toPlot[0])),0.9986295347545738,origin,rotate);
    scaleAndShift=calculate->getScaleAndOrigin(toPlot.at(0));
    resetScaleAndShift();
    m_program->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vbo.bind();
    m_vbo.allocate(calculate->getVertices(toPlot.at(0)), 6 * calculate->getStep(toPlot.at(0)) * sizeof(GLfloat));
    setupVertexAttribs();
    resetBuffer();
    m_vao.release();
    m_program->release();
    update();
}

void plotWidget::rePlot() {
    QThread *calculateThread=QThread::create([&]()->void{
        calculate->Gamma3SolveProbe(*(calculate->getProbe(toPlot[0])),15000,0.0645462/1.5,864*40500);
        calculate->probeRelativeOrbit(200,1,p1,p2,calculate->getProbe(toPlot[0])->EndT,*(calculate->getProbe(toPlot[0])),0.9986295347545738,origin,rotate);});
    connect(calculateThread, &QThread::finished, this, &plotWidget::rePlotResult);
    connect(calculateThread, &QThread::finished, calculateThread, &QObject::deleteLater);
    calculateThread->start();
}

void plotWidget::rePlotResult() {
    scaleAndShift=calculate->getScaleAndOrigin(toPlot.at(0));
    resetScaleAndShift();
    m_program->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vbo.bind();
    m_vbo.allocate(calculate->getVertices(toPlot.at(0)), 6 * calculate->getStep(toPlot.at(0)) * sizeof(GLfloat));
    setupVertexAttribs();
    resetBuffer();
    m_vao.release();
    m_program->release();
    update();
    emit rePlotResultReady(true);
}

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "uniform mat4 projMatrix;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 mvMatrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position =projMatrix * mvMatrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";


void plotWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &plotWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("posAttr", 0);
    m_program->bindAttributeLocation("colAttr", 1);
    m_program->link();
    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    calculate->Gamma3SolveProbe(*(calculate->getProbe(toPlot[0])),15000,0.0645462/1.5,864*40500);
    calculate->probeRelativeOrbit(200,1,p1,p2,calculate->getProbe(toPlot[0])->EndT,*(calculate->getProbe(toPlot[0])),0.9986295347545738,origin,rotate);
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vbo.create();
    m_vbo.bind();
    scaleAndShift=calculate->getScaleAndOrigin(toPlot.at(0));
    resetScaleAndShift();
    m_vbo.allocate(calculate->getVertices(toPlot.at(0)), 6 * calculate->getStep(toPlot.at(0)) * sizeof(GLfloat));
    setupVertexAttribs();
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);
    m_program->release();
}

void plotWidget::setupVertexAttribs()
{
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void plotWidget::resetBuffer()
{
    m_vbo.bind();
    GLfloat* ptr{(GLfloat*)m_vbo.map(QOpenGLBuffer::WriteOnly)};
    if (ptr) {
        updateVBO(ptr);
        m_vbo.unmap();
    }
    m_vbo.release();
}

void plotWidget::updateVBO(GLfloat* ptr)
{
    int m=6 * calculate->getStep(toPlot.at(0));
    GLfloat* pt{calculate->getVertices(toPlot.at(0))};
    for(int i=0;i<m;i++)
        ptr[i]=pt[i];
}

void plotWidget::disableVertexAttribs() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDisableVertexAttribArray(0);
    f->glDisableVertexAttribArray(1);
}

void plotWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    m_world.setToIdentity();
    m_world.rotate(rotation);
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    glDrawArrays(GL_LINE_STRIP, 0, calculate->getStep(toPlot.at(0)));
    m_program->release();
}

void plotWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void plotWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void plotWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        rotation=QQuaternion::fromAxisAndAngle(QVector3D(dy,dx,0.0),sqrt(dx*dx+dy*dy))*rotation;
        update();
    } else if (event->buttons() & Qt::RightButton) {
        QQuaternion reverse=rotation.conjugated();
        QVector3D xa=reverse.rotatedVector(QVector3D(-1.0f,0.0f,0.0f));
        QVector3D ya=reverse.rotatedVector(QVector3D(0.0f,1.0f,0.0f));
        for(int i=0;i<3;i++) {
            scaleAndShift[i+1]=scaleAndShift[i+1]+dx*xa[i]/scaleAndShift[0]/500+dy*ya[i]/scaleAndShift[0]/500;
        }
        m_program->bind();
        resetScaleAndShift();
        resetBuffer();
        m_program->release();
        update();
    }
    m_lastPos = event->pos();
}

void plotWidget::wheelEvent(QWheelEvent *event) {
    GLfloat sensitivity = 0.0005f;
    scaleAndShift[0] *= (1.0f + event->delta() * sensitivity);
    m_program->bind();
    resetScaleAndShift();
    resetBuffer();
    m_program->release();
    update();
}

