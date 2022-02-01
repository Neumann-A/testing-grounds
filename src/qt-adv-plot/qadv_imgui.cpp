
#include "qadv_imgui.hpp"

#include <QtImGui.h>
#include <imgui.h>
#include <qtmetamacros.h>
#include <QDebug>
#include <QHash>
#include <QDateTime>
#include <QWheelEvent>
#include <QOpenGLExtraFunctions>
#include <QGuiApplication>
#include <QWidget>
#include <QClipboard>

namespace {
    // Keyboard mapping. Dear ImGui use those indices to peek into the io.KeysDown[] array.
    const QHash<int, ImGuiKey> keyMap = {
        { Qt::Key_Tab, ImGuiKey_Tab },
        { Qt::Key_Left, ImGuiKey_LeftArrow },
        { Qt::Key_Right, ImGuiKey_RightArrow },
        { Qt::Key_Up, ImGuiKey_UpArrow },
        { Qt::Key_Down, ImGuiKey_DownArrow },
        { Qt::Key_PageUp, ImGuiKey_PageUp },
        { Qt::Key_PageDown, ImGuiKey_PageDown },
        { Qt::Key_Home, ImGuiKey_Home },
        { Qt::Key_End, ImGuiKey_End },
        { Qt::Key_Insert, ImGuiKey_Insert },
        { Qt::Key_Delete, ImGuiKey_Delete },
        { Qt::Key_Backspace, ImGuiKey_Backspace },
        { Qt::Key_Space, ImGuiKey_Space },
        { Qt::Key_Enter, ImGuiKey_Enter },
        { Qt::Key_Return, ImGuiKey_Enter },
        { Qt::Key_Escape, ImGuiKey_Escape },
        { Qt::Key_A, ImGuiKey_A },
        { Qt::Key_C, ImGuiKey_C },
        { Qt::Key_V, ImGuiKey_V },
        { Qt::Key_X, ImGuiKey_X },
        { Qt::Key_Y, ImGuiKey_Y },
        { Qt::Key_Z, ImGuiKey_Z },
        { Qt::MiddleButton, ImGuiMouseButton_Middle }
    };

    const QHash<ImGuiMouseCursor, Qt::CursorShape> cursorMap = {
        { ImGuiMouseCursor_Arrow,      Qt::CursorShape::ArrowCursor },
        { ImGuiMouseCursor_TextInput,  Qt::CursorShape::IBeamCursor },
        { ImGuiMouseCursor_ResizeAll,  Qt::CursorShape::SizeAllCursor },
        { ImGuiMouseCursor_ResizeNS,   Qt::CursorShape::SizeVerCursor },
        { ImGuiMouseCursor_ResizeEW,   Qt::CursorShape::SizeHorCursor },
        { ImGuiMouseCursor_ResizeNESW, Qt::CursorShape::SizeBDiagCursor },
        { ImGuiMouseCursor_ResizeNWSE, Qt::CursorShape::SizeFDiagCursor },
        { ImGuiMouseCursor_Hand,       Qt::CursorShape::PointingHandCursor },
        { ImGuiMouseCursor_NotAllowed, Qt::CursorShape::ForbiddenCursor },
    };
    QByteArray g_currentClipboardText;
}

QAdvImguiPlot::QAdvImguiPlot(QWidget *parent) : QOpenGLWidget(parent), refresh_timer(this) {
        refresh_timer.callOnTimeout([this](){this->update();});
        refresh_timer.start((int)(1000.0 / 200.0));
        frame_time.start();
    };
QAdvImguiPlot::~QAdvImguiPlot() noexcept {
    ImGui_ImplOpenGL3_Shutdown();
    if(im_ctx) {
        ImGui::DestroyContext();
    }
}

void QAdvImguiPlot::onMousePressedChange(QMouseEvent *event)
{
    qmousepressed.push(event->buttons());
}

void QAdvImguiPlot::mouseDoubleClickEvent(QMouseEvent *event) {
    QOpenGLWidget::mouseDoubleClickEvent(event);
    onMousePressedChange(event);
}
void QAdvImguiPlot::mousePressEvent(QMouseEvent *event)  {
    QOpenGLWidget::mousePressEvent(event);
    onMousePressedChange(event);
}
void QAdvImguiPlot::mouseReleaseEvent(QMouseEvent *event)  {
    QOpenGLWidget::mouseReleaseEvent(event);
    onMousePressedChange(event);
}
void QAdvImguiPlot::mouseMoveEvent(QMouseEvent *event)  {
    QOpenGLWidget::mouseMoveEvent(event);
}
void QAdvImguiPlot::onWheel(QWheelEvent *event)
{
    //qDebug() << Q_FUNC_INFO;
    // Select current context
    ImGui::SetCurrentContext(im_ctx);

    // Handle horizontal component
    if(event->pixelDelta().x() != 0)
    {
        g_MouseWheelH += (float)event->pixelDelta().x() / (ImGui::GetTextLineHeight());
    } else {
        // Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
        g_MouseWheelH += (float)event->angleDelta().x() / 120;
    }

    // Handle vertical component
    if(event->pixelDelta().y() != 0)
    {
        // 5 lines per unit
        g_MouseWheel += (float)event->pixelDelta().y() / (5.0 * ImGui::GetTextLineHeight());
    } else {
        // Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
        g_MouseWheel += (float)event->angleDelta().y() / 120;
    }
}

void QAdvImguiPlot::onKeyPressRelease(QKeyEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    // Select current context
    ImGui::SetCurrentContext(im_ctx);

    ImGuiIO& io = ImGui::GetIO();
    
    const bool key_pressed = (event->type() == QEvent::KeyPress);
    
    // Translate `Qt::Key` into `ImGuiKey`, and apply 'pressed' state for that key
    const auto key_it = keyMap.constFind( event->key() );
    if (key_it != keyMap.constEnd()) { // Qt's key found in keyMap
        const int imgui_key = *(key_it);
        io.KeysDown[imgui_key] = key_pressed;
    }

    if (key_pressed) {
        const QString text = event->text();
        if (text.size() == 1) {
            io.AddInputCharacter( text.at(0).unicode() );
        }
    }

#ifdef Q_OS_MAC
    io.KeyCtrl  = event->modifiers() & Qt::MetaModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt   = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::ControlModifier; // Comamnd key
#else
    io.KeyCtrl  = event->modifiers() & Qt::ControlModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt   = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::MetaModifier;
#endif
}

void QAdvImguiPlot::wheelEvent(QWheelEvent *event) {
    QOpenGLWidget::wheelEvent(event);
    this->onWheel(event);
}

void QAdvImguiPlot::keyPressEvent(QKeyEvent *event) {
    QOpenGLWidget::keyPressEvent(event);
    this->onKeyPressRelease(event);
}
void QAdvImguiPlot::keyReleaseEvent(QKeyEvent *event) {
    QOpenGLWidget::keyReleaseEvent(event);
    this->onKeyPressRelease(event);
}

void QAdvImguiPlot::setCursorPos(const ImGuiIO& io)
{
    // NOTE: This code will be executed, only if the following flags have been set:
    // - backend flag: `ImGuiBackendFlags_HasSetMousePos`      - enabled
    // - config  flag: `ImGuiConfigFlags_NavEnableSetMousePos` - enabled
    
#ifndef QT_NO_CURSOR
    if(io.WantSetMousePos) {
        //QWidget::mapToGlobal()
        QCursor::setPos({(int)io.MousePos.x, (int)io.MousePos.y});
    }
#else
    Q_UNUSED(io);
#endif
}

void QAdvImguiPlot::initializeGL()
{
    
    QOpenGLWidget::initializeGL();
    //initializeOpenGLFunctions();
    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    //ImGui::SetCurrentContext(this->context());
    if(!im_ctx) {
        im_ctx = ImGui::CreateContext(); 
        ImGui_ImplOpenGL3_Init(glsl_version);
        QObject::connect(this->context(),&QOpenGLContext::aboutToBeDestroyed,[this](){ 
                auto ctx = ImGui::GetCurrentContext();
                if(ctx) 
                    ImGui::DestroyContext(); 
                im_ctx = nullptr;
            });
        QObject::connect(this,&QOpenGLWidget::frameSwapped,[this](){ this->paintGL();});
        this->installEventFilter(this);
    } else {
        ImGui::SetCurrentContext(im_ctx);
    }
    auto qt_ctx = this->context();
    qDebug() << "init:" << this->size();
    //qDebug() << qt_ctx->surface()->size();
    //qDebug() << qt_ctx->surface()->format();
    //qDebug() << qt_ctx;
    //qDebug() << static_cast<QOpenGLWidget*>(this);
    ImGuiIO& io = ImGui::GetIO(); // (void)io;
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    //io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "qt-widget-opengl-imgui";
    for (ImGuiKey key : keyMap.values()) {
        io.KeyMap[key] = key;
    }
    io.SetClipboardTextFn = [](void *user_data, const char *text) {
        Q_UNUSED(user_data);
        QGuiApplication::clipboard()->setText(text);
    };
    io.GetClipboardTextFn = [](void *user_data) {
        Q_UNUSED(user_data);
        g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
        return (const char *)g_currentClipboardText.data();
    };
    io.DisplaySize.x = (float)this->size().width();
    io.DisplaySize.y = (float)this->size().height();
    io.MouseDoubleClickTime = 0.5;
    this->update();
    //QtImGui::initialize(this);
}
void QAdvImguiPlot::resizeGL(int w, int h) {
    if(!im_ctx) {
        initializeGL();
    }
    QOpenGLWidget::resizeGL(w,h);
    ImGui::SetCurrentContext(im_ctx);
    ImGuiIO& io = ImGui::GetIO();
    qDebug() << "resize:" << this->size();
    io.DisplaySize.x = (float)this->size().width();
    io.DisplaySize.y = (float)this->size().height();

    //io.DisplaySize.x = 50;
    //io.DisplaySize.y = 50;
    this->update();
}
void QAdvImguiPlot::paintGL() 
{
    QOpenGLExtraFunctions qopengl(this->context());
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //auto qt_ctx = this->context();
    //io.DisplaySize.x = (float)qt_ctx->surface()->size().rwidth();
    //io.DisplaySize.y = (float)qt_ctx->surface()->size().rheight();
    // you can do custom GL rendering as well in paintGL
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2((float)this->size().width(), (float)this->size().height());
    io.DisplayFramebufferScale = ImVec2((float)this->devicePixelRatio(), (float)this->devicePixelRatio());

    if(!qmousepressed.empty()) {
        current_mouse_event = qmousepressed.top();
        qmousepressed.pop();
    }

    io.MouseDown[0] = current_mouse_event & Qt::LeftButton;
    io.MouseDown[1] = current_mouse_event & Qt::RightButton;
    io.MouseDown[2] = current_mouse_event & Qt::MiddleButton;
    io.MouseDown[3] = current_mouse_event & Qt::BackButton;
    io.MouseDown[4] = current_mouse_event & Qt::ForwardButton;

    const auto widget_geometry = this->geometry();
    const auto mousePos = this->mapFromGlobal(QCursor::pos());
    if (this->isActiveWindow() && widget_geometry.contains(mousePos)) {
        io.MousePos = ImVec2((float)mousePos.x(), (float)mousePos.y());
    } else {
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }
    io.MouseWheelH = g_MouseWheelH;
    io.MouseWheel = g_MouseWheel;
    g_MouseWheelH = 0;
    g_MouseWheel = 0;
    // Setup time step
    const auto frametime = frame_time.isValid() ? (float)frame_time.restart() / 1000 + FLT_MIN : (float)(1.0f/60.0f);
    io.DeltaTime = frametime;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    //setCursorPos(io);
    //for (int i = 0; i < 5; i++)
    //{
    //    io.MouseDown[i] = g_MousePressed[i];
    //    if(g_MousePressed[i])
    //    {
    //         qDebug() << "Mouse pressed";
    //    }
    //}
    //const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2((float)this->size().width(), (float)this->size().height()), ImGuiCond_FirstUseEver);
    ImGui::Begin("Hello, world!");  
    //QtImGui::newFrame();
    ImGui::ShowDemoWindow();
    //ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2((float)this->size().width(), (float)this->size().height()), ImGuiCond_FirstUseEver);
    //ImGui::Text("Hello");
    //ImGui::ShowUserGuide();
    // more widgets...
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    qopengl.glViewport(0, 0, this->size().width(), this->size().height());
    //QtImGui::render();
    //io.MousePos = ImVec2(-1,-1);
    QOpenGLWidget::paintGL();
    //this->update();
}

#include "moc_qadv_imgui.cpp"
