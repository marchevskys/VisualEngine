#include "window.h"
#include "control.h"
#include "logger.h"

//#include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>

Window *Window::currentWindow = nullptr;
int Window::primaryScreenWidth, Window::primaryScreenHeight;

class WindowManager {
    WindowManager() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        Window::primaryScreenWidth = mode->width;
        Window::primaryScreenHeight = mode->height;
        DLOG("GLFW initialized");
    }

    ~WindowManager() {
        glfwTerminate();
        DLOG("GLFW terminated");
    }

  public:
    static WindowManager *getInstance() {
        static WindowManager winManager;
        return &winManager;
    }
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool sIsUp = true;
    if (key == GLFW_KEY_F && action == GLFW_PRESS && sIsUp) {
        Window::currentWindow->toggleFullscreen();
        //glViewport(0, 0, Window::primaryScreenWidth, Window::primaryScreenHeight);
        sIsUp = false;
    }

    else if (action == GLFW_RELEASE)
        sIsUp = true;

    Control::key_callback(window, key, scancode, action, mode);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    Window::currentWindow->resize(width, height);
    glViewport(0, 0, width, height);
}

void Window::resize(int width, int height) {

    if (m_fullScreen) {
        primaryScreenWidth = width;
        primaryScreenHeight = height;
    } else {
        m_width = width;
        m_height = height;
    }
}

// clang-format off
//#define CAPTURED_CURSOR
#ifdef CAPTURED_CURSOR
#define CURSOR_WINDOW_MODE      GLFW_CURSOR_DISABLED
#define CURSOR_FULL_SCREEN_MODE GLFW_CURSOR_DISABLED
#else
#define CURSOR_WINDOW_MODE      GLFW_CURSOR_NORMAL
#define CURSOR_FULL_SCREEN_MODE GLFW_CURSOR_HIDDEN
#endif
// clang-format on

void Window::toggleFullscreen() {
    if (m_fullScreen) {
        glfwSetWindowMonitor(m_window, nullptr, m_xPos, m_yPos, m_width, m_height, 1);
        glfwSetInputMode(m_window, GLFW_CURSOR, CURSOR_WINDOW_MODE);
        m_fullScreen = false;
        DLOGN(m_width, m_height);
    } else {
        glfwGetWindowPos(m_window, &m_xPos, &m_yPos);
        glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, primaryScreenWidth, primaryScreenHeight, 1);
        glfwSetInputMode(m_window, GLFW_CURSOR, CURSOR_FULL_SCREEN_MODE);
        m_fullScreen = true;
        DLOGN(primaryScreenWidth, primaryScreenHeight);
    }
    DLOGN(m_fullScreen);
}

Window::Window(int width, int height, const char *_name, bool _fullScreen) : m_fullScreen(_fullScreen) {
    WindowManager::getInstance(); // initialize GLFW
    glfwWindowHint(GLFW_SAMPLES, 1);
    m_windowWidth = m_width = width;
    m_windowHeight = m_height = height;

    m_xPos = (primaryScreenWidth - width) / 2;
    m_yPos = (primaryScreenHeight - height) / 2;

    if (_fullScreen) {
        m_window = glfwCreateWindow(primaryScreenWidth, primaryScreenHeight, _name, glfwGetPrimaryMonitor(), nullptr);
        glfwSetInputMode(m_window, GLFW_CURSOR, CURSOR_FULL_SCREEN_MODE);
    } else {
        m_window = glfwCreateWindow(m_width, m_height, _name, nullptr, nullptr);
        glfwSetWindowPos(m_window, m_xPos, m_yPos);
        glfwSetInputMode(m_window, GLFW_CURSOR, CURSOR_WINDOW_MODE);
    }
    if (m_window == nullptr) {
        glfwTerminate();
        THROW("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        THROW("Failed to initialize GLEW");
    }
    glfwSwapInterval(1);
    glfwSetKeyCallback(m_window, key_callback);
    DLOG("Window created");
}

void Window::refresh() {
    currentWindow = this;
    Control::resetMouse();
    glfwMakeContextCurrent(m_window);
    glfwSwapBuffers(m_window);
    glfwPollEvents();

    glfwSetCursorPosCallback(m_window, Control::mouse_callback); // mouse func
    glfwSetScrollCallback(m_window, Control::scroll_callback);   // scroll func
}

float Window::bind() const {
    if (m_fullScreen) {
        return IFrameBuffer::bind(0, 0, primaryScreenWidth, primaryScreenHeight);
        glfwSwapInterval(1);
    }

    return IFrameBuffer::bind(0, 0, m_width, m_height);
    glfwSwapInterval(1);
}

void Window::setTitle(const char *title) {
    glfwSetWindowTitle(m_window, title);
}

float Window::getAspectRatio() {
    if (m_fullScreen)
        return static_cast<float>(primaryScreenWidth) / static_cast<float>(primaryScreenHeight);
    else
        return static_cast<float>(m_width) / static_cast<float>(m_height);
}

double Window::getTime() {
    return glfwGetTime();
}

bool Window::active() {
    return !glfwWindowShouldClose(m_window);
}

void Window::clear() const {
    //bind();
    //glClearColor(0.10f, 0.1f, 0.13f, 1.0f);
    glClearColor(0.8f, 0.8f, 0.99f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Window::~Window() {
    Control::reset();
    glfwDestroyWindow(m_window);
    DLOG("Window destroyed");
}
