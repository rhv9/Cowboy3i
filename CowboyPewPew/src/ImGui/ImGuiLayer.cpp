#include "pch.h"
#include "ImGuiLayer.h"
#include <imgui.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

#include "Game.h"

ImGuiLayer::ImGuiLayer()
    : Layer() {}

void ImGuiLayer::OnBegin()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Game::Instance().GetWindow()->GetNativeWindow(), true);
#ifdef __EMSCRIPTEN__
    GK_ASSERT(false, "This might not be ready, double check");
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init("#version 430");
}

void ImGuiLayer::OnUpdate(Timestep delta)
{
}

void ImGuiLayer::OnRemove()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::BeginRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndRender()
{

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
