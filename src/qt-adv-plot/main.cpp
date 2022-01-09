#include <qnamespace.h>
#include <qobject.h>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <qsize.h>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>

#include <DockManager.h>

#include <qcustomplot.h>

#include "qt-adv-plot.hpp"
#include "qadv_plot.hpp"

#include <QOpenGLWidget>

#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include <implot.h>
#include <QtImGui.h>
#include <qtmetamacros.h>
#include <QDebug>
#include <QHash>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include <stack>
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
class QAdvImguiPlot : public QOpenGLWidget
{
    Q_OBJECT

    ImGuiContext * im_ctx = nullptr;
    float        g_MouseWheel;
    float        g_MouseWheelH;
    QTimer       refresh_timer;
    QElapsedTimer  frame_time;

    Qt::MouseButtons current_mouse_event{Qt::NoButton};
    std::stack<Qt::MouseButtons> qmousepressed;
public:
    QAdvImguiPlot(QWidget *parent = nullptr) : QOpenGLWidget(parent), refresh_timer(this) {
        refresh_timer.callOnTimeout([this](){this->update();});
        refresh_timer.start((int)(1000.0 / 200.0));
        frame_time.start();
    };
    ~QAdvImguiPlot() noexcept override {
        ImGui_ImplOpenGL3_Shutdown();
        if(im_ctx) {
            ImGui::DestroyContext();
        }
    }

    void onMousePressedChange(QMouseEvent *event)
    {
        qmousepressed.push(event->buttons());
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override {
        QOpenGLWidget::mouseDoubleClickEvent(event);
        onMousePressedChange(event);
    }
    void mousePressEvent(QMouseEvent *event) override {
        QOpenGLWidget::mousePressEvent(event);
        onMousePressedChange(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        QOpenGLWidget::mouseReleaseEvent(event);
        onMousePressedChange(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        QOpenGLWidget::mouseMoveEvent(event);
    }
    void onWheel(QWheelEvent *event)
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

    void onKeyPressRelease(QKeyEvent *event)
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

    void wheelEvent(QWheelEvent *event) override {
        QOpenGLWidget::wheelEvent(event);
        this->onWheel(event);
    }

    void keyPressEvent(QKeyEvent *event) override {
        QOpenGLWidget::keyPressEvent(event);
        this->onKeyPressRelease(event);
    }
    void keyReleaseEvent(QKeyEvent *event) override {
        QOpenGLWidget::keyReleaseEvent(event);
        this->onKeyPressRelease(event);
    }

    void setCursorPos(const ImGuiIO& io)
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

protected:

    void initializeGL() override
    {
        
        QOpenGLWidget::initializeGL();
        //initializeOpenGLFunctions();
        const char* glsl_version = "#version 130";
        IMGUI_CHECKVERSION();
        //ImGui::SetCurrentContext(this->context());
        if(!im_ctx) {
            im_ctx = ImGui::CreateContext();
            ImGui_ImplOpenGL3_Init(glsl_version);
            QObject::connect(this->context(),&QOpenGLContext::aboutToBeDestroyed,[this](){ if(im_ctx) ImGui::DestroyContext(); im_ctx = nullptr;});
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
    void resizeGL(int w, int h) override {
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
    void paintGL() override
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



        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
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
};


struct QDockInit {
    QFlags<ads::CDockWidget::DockWidgetFeature> features {ads::CDockWidget::DefaultDockWidgetFeatures};
    ads::DockWidgetArea             area{ads::CenterDockWidgetArea};
    ads::DockWidgetAreas            allowed;
    bool floating {false};
    //bool visible {true};
    QString title {QString::fromUtf8("Not set")};
};

static  ads::CDockWidget * setUpNewDockWidget(QMainWindow& main, ads::CDockManager& manager, QMenu& menu, QDockInit init) {
    static std::size_t counter = 0;
    ads::CDockWidget * pdock = new  ads::CDockWidget(init.title,&main);
    ads::CDockWidget & dock = *pdock; 
    dock.setFeatures(init.features);
    if((init.features & ads::CDockWidget::DockWidgetFloatable) ? init.floating : false) {
        dock.setFloating( );
    }
    dock.setVisible(true);
#ifdef _DEBUG
    dock.setWindowTitle(init.title + QString("_0x") +  QString::number((std::size_t)(pdock),16));
#endif
    dock.setObjectName(QString{"dock_"}.append(QString::number(counter++)).append("_").append(init.title.toLower()));
    if(init.features & ads::CDockWidget::DockWidgetClosable) {
        menu.addAction(dock.toggleViewAction());
    }

    manager.addDockWidgetTab(init.area,&dock);
    return pdock;
}

class QtAdvPlot_App : public QApplication {
private:
    QMainWindow mainWindow;
    ads::CDockManager* m_DockManager;
protected:
    //bool event(QEvent *e) override;
public:
    QtAdvPlot_App(int argc, char *argv[]) : QApplication(argc,argv), mainWindow() {
        mainWindow.setObjectName(QString::fromUtf8("main_window"));
        mainWindow.resize(QSize(600,400));

        auto menubar = mainWindow.menuBar(); //new QMenuBar(&mainWindow);
        menubar->setObjectName(QString::fromUtf8("main_menu_bar"));
        auto viewsmenu = menubar->addMenu(tr("&Views"));

        ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
        ads::CDockManager::setConfigFlag(ads::CDockManager::RetainTabSizeWhenCloseButtonHidden, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);
        m_DockManager = new ads::CDockManager(&mainWindow);

        {   
            auto qwtPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qwt-plot"});
            qwtPlotDock->setWidget(new QAdvPlot({}, &mainWindow));
        }
        {
            auto qcustomplotPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qcustomplot-plot"});
            qcustomplotPlotDock->setWidget(new QCustomPlot(&mainWindow));
            auto plotwidget = static_cast<QCustomPlot*>(qcustomplotPlotDock->widget());
            plotwidget->xAxis2->setVisible(true);
            plotwidget->yAxis2->setVisible(true);
            plotwidget->xAxis2->setTickLabels(false);
            plotwidget->yAxis2->setTickLabels(false);
            auto pen = plotwidget->xAxis->basePen();
            pen.setWidth(2);
            plotwidget->xAxis->setBasePen(pen);
            plotwidget->xAxis->setTickPen(pen);
            plotwidget->xAxis->setSubTickPen(pen);
        }
        {
            auto imguiPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="imgui-plot"});
            imguiPlotDock->setWidget(new QAdvImguiPlot(&mainWindow));
        }
        mainWindow.show();
    }
    ~QtAdvPlot_App() noexcept override {};
};

int main(int argc, char *argv[])
{
    QtAdvPlot_App app(argc,argv);

    return app.exec();
}

#include "main.moc"
