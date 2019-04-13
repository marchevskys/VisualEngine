project (imgui_lib)

set(SOURCES imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_opengl3.cpp imgui_impl_glfw.cpp imgui_demo.cpp)
set(HEADERS imconfig.h imgui.h imgui_impl_glfw.h imgui_impl_opengl3.h imgui_internal.h imstb_rectpack.h imstb_textedit.h imstb_truetype.h)

add_library(imgui_lib ${SOURCES} ${HEADERS})
target_include_directories(imgui_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})