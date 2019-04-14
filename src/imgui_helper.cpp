
#include "imgui_helper.h"
#include "logger.h"

glm::vec3 ImGuiHelper::m_shipCoordinates = glm::vec3(0., 0., 0.);

ImGuiHelper::ImGuiHelper(GLFWwindow* window) {
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsClassic();

   // Setup Platform/Renderer bindings
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplGlfw_Shutdown();
   ImGui_ImplOpenGL3_Init(nullptr);	// using default OpenGL version, 130
   DLOG("ImGui initialized");
}

ImGuiHelper::~ImGuiHelper() {
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

void ImGuiHelper::drawWindow() {
   ImGui::Begin("config");
   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   ImGui::Text("Ship position (%.1f;%.1f;%.1f)", m_shipCoordinates.x, m_shipCoordinates.y, m_shipCoordinates.z);
   ImGui::End();
}