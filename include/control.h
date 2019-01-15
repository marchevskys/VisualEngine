#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <functional>
#include <map>

using KeyCode = int;
class GLFWwindow;

struct Coordinates {
    double x = 0, y = 0;
    Coordinates(double _x, double _y) : x(_x), y(_y) {}
    Coordinates operator+=(const Coordinates right) { return {x + right.x, y + right.y}; }
    Coordinates &operator=(const Coordinates right) {
        x = right.x;
        y = right.y;
        return *this;
    }
    friend Coordinates operator+(const Coordinates left, const Coordinates right) { return {left.x + right.x, left.y + right.y}; }
    friend Coordinates operator-(const Coordinates left, const Coordinates right) { return {left.x - right.x, left.y - right.y}; }
};

class Control {
    static bool keys[1024];
    static double scroll;
    static Coordinates m_currentMousePos;

  public:
    enum class Button { // clang-format off
        Up, Down, Left, Right, Space, Enter,
        W, A, S, D, E, Q
    }; // clang-format on
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    static KeyCode getKey(const char *input);
    static void attachKey(KeyCode k, std::function<void()> f);

    static double scrollOffset();
    static Coordinates mousePos();
    static bool pressed(Button button);

    static void reset();
    static void resetMouse();
    static void resetKeyboard();

    Control(){};
};

#endif // CONTROLLER_H
