#pragma once
#include "bow/input/InputData.hpp"
#include <QtWidgets>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "LimbMesh.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class LimbView: public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    LimbView();
    ~LimbView() override;

    void setData(const InputData& data);

    void viewProfile();
    void viewTop();
    void view3D();
    void viewSymmetric(bool checked);
    void viewFit();

private:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void zoom(float factor);

    void cleanup();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    int m_xRot;
    int m_yRot;
    int m_zRot;
    float m_zoom;

    QPoint m_lastPos;

    LimbMesh m_logo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_logoVbo;
    QOpenGLShaderProgram *m_program;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
};
