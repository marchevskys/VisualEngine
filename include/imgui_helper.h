#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>

struct GLFWwindow;

// Covers ImGui loading / unloading.
class ImGuiHelper {
  private:
    ImGuiHelper(GLFWwindow *window);
    ~ImGuiHelper();

  public:
    static void InitImGui(GLFWwindow *window) {
        static ImGuiHelper helper(window);
    }

    static void drawWindow();
    static void setText(const std::string &text) { m_text = text; }
    static void setText(std::string &&text) { m_text = std::move(text); }

  private:
    static std::string m_text;
};
