#pragma once
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "glm/glm.hpp"

struct GLFWwindow;

// Covers ImGui loading / unloading. 
class ImGuiHelper {
private:
   ImGuiHelper(GLFWwindow* window);
   ~ImGuiHelper();
public:
   static void InitImGui(GLFWwindow* window) {
      static ImGuiHelper helper(window);
   }

   static void drawWindow();
   static void setShipCoordinates(const glm::vec3& coordinates) { m_shipCoordinates = coordinates; }
   
private:
   static glm::vec3 m_shipCoordinates;
};
