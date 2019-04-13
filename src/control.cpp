#include "control.h"
#include "logger.h"
#include "string"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>

bool Control::keys[1024]{0};
double Control::scroll = 0.0;
Coordinates Control::m_currentMousePos{0.0, 0.0};
static Coordinates m_prevMousePos{0.0, 0.0};
static Coordinates diff{0, 0};
std::map<Control::Button, int> Control::keymapper;

Control::Control() {
    if (keymapper.empty()) {
        keymapper[Control::Button::Up] = GLFW_KEY_UP;
        keymapper[Control::Button::Down] = GLFW_KEY_DOWN;
        keymapper[Control::Button::Left] = GLFW_KEY_LEFT;
        keymapper[Control::Button::Right] = GLFW_KEY_RIGHT;
        keymapper[Control::Button::Space] = GLFW_KEY_SPACE;
        keymapper[Control::Button::Enter] = GLFW_KEY_ENTER;
        keymapper[Control::Button::W] = GLFW_KEY_W;
        keymapper[Control::Button::A] = GLFW_KEY_A;
        keymapper[Control::Button::Z] = GLFW_KEY_Z;
        keymapper[Control::Button::S] = GLFW_KEY_S;
        keymapper[Control::Button::D] = GLFW_KEY_D;
        keymapper[Control::Button::E] = GLFW_KEY_E;
        keymapper[Control::Button::Q] = GLFW_KEY_Q;
        keymapper[Control::Button::F1] = GLFW_KEY_F1;
        keymapper[Control::Button::F2] = GLFW_KEY_F2;
    }
}

void Control::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key > 10 && key < 300) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void Control::mouse_callback(GLFWwindow *window, double x, double y) {
    diff = m_prevMousePos - Coordinates(x, y);
    m_prevMousePos = {x, y};
}

void Control::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    scroll += yoffset;
}

double Control::scrollOffset() {
    double value = scroll;
    scroll = 0;
    return value;
}

Coordinates Control::mousePos() {
    return diff;
}

bool Control::pressed(Control::Button button) {
    return keys[keymapper[button]];
}

void Control::resetMouse() {
    m_currentMousePos = {0.0, 0.0};
    scroll = 0.0;
    diff = {0, 0};
}

void Control::resetKeyboard() {
    for (auto &k : keys)
        k = false;
}

void Control::reset() {
    Control::resetKeyboard();
    Control::resetMouse();
}
