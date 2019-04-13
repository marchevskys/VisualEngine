#ifndef WINDOW_H
#define WINDOW_H
#include "framebuffer.h"

#include <vector>
struct GLFWwindow;
class Window : public Visual::IFrameBuffer {
    friend void processInput(GLFWwindow *window);
    friend void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    friend class WindowManager;

  public:
    Window(int w, int h, const char *name, bool _fullScreen = false);
    void toggleFullscreen();
    void refresh();
    float bind() const override final;

    void setTitle(const char *title);
    float getAspectRatio();
    static double getTime();
    bool active();
    void clear() const final;
    ~Window();
    static Window *currentWindow;
    static int primaryScreenWidth, primaryScreenHeight;

  private:
    GLFWwindow *m_window = nullptr;
    GLFWwindow *getGLFWwindow() { return m_window; }

    int m_xPos = 0, m_yPos = 0;
    bool m_fullScreen = false;
    int m_windowWidth = 20, m_windowHeight = 20;
    void resize(int width, int height);
};

#endif // WINDOW_H
