#ifndef WINDOW_H
#define WINDOW_H
#include "framebuffer.h"

#include <vector>
class GLFWwindow;
class Window : public Visual::IFrameBuffer {
    friend void processInput(GLFWwindow *window);
    friend void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    friend class WindowManager;

  public:
    Window(int w, int h, const char *name, bool _fullScreen = false);
    void toggleFullscreen();
    void refresh();
    void setTitle(const char *title);
    float getAspectRatio();
    bool active();
    void clear() const final;
    ~Window();
    static Window *currentWindow;

  private:
    GLFWwindow *m_window = nullptr;
    GLFWwindow *getGLFWwindow() { return m_window; }

    int m_xPos = 0, m_yPos = 0;
    bool m_fullScreen = false;
    int m_windowWidth = 20, m_windowHeight = 20;
    static int primaryScreenWidth, primaryScreenHeight;
    void resize(int width, int height);
};

#endif // WINDOW_H
