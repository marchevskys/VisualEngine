#ifndef WINDOW_H
#define WINDOW_H
#include <vector>

class GLFWwindow;
class Window {
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
    ~Window();
    static Window *currentWindow;

  private:
    GLFWwindow *m_window = nullptr;
    GLFWwindow *getGLFWwindow() { return m_window; }
    int m_WindowedWidth = 100, m_WindowedHeight = 100;
    int m_xPos = 0, m_yPos = 0;
    bool m_fullScreen = false;
    static int primaryScreenWidth, primaryScreenHeight;
    void resize(int width, int height);
};

#endif // WINDOW_H
