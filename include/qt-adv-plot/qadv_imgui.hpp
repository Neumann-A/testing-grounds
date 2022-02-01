#pragma once

#include <stack>

#include <QOpenGLWidget>
#include <QTimer>
#include <QElapsedTimer>

#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include <implot.h>

class QAdvImguiPlot : public QOpenGLWidget {
    Q_OBJECT

    ImGuiContext * im_ctx = nullptr;
    float        g_MouseWheel;
    float        g_MouseWheelH;
    QTimer       refresh_timer;
    QElapsedTimer  frame_time;

    Qt::MouseButtons current_mouse_event{Qt::NoButton};
    std::stack<Qt::MouseButtons> qmousepressed;
public: 
    QAdvImguiPlot(QWidget *parent = nullptr);
    ~QAdvImguiPlot() noexcept override;

    void onMousePressedChange(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void onWheel(QWheelEvent *event);
    void onKeyPressRelease(QKeyEvent *event);

    
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void setCursorPos(const ImGuiIO& io);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
