#include <GL/glew.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <rendering/popup.hpp>
#include <functional>

Popups::FileBrowser::FileBrowser(GLFWwindow* parent){
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(800, 670, "Browse...", NULL, parent);
    if(window == NULL){
        fprintf(stderr, "Failed to open window\n");
        glfwTerminate();
    }
}

Popups::FileBrowser::~FileBrowser(){
    glfwDestroyWindow(window);
}

void Popups::FileBrowser::RegisterCallback(std::function<int(std::string)> func){
    callback = func;
}


void Popups::FileBrowser::Render(){
    glfwMakeContextCurrent(window);
    glfwPollEvents();
    glClearColor(0.0f, 0.1f, 0.3f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader);
    int width,height;
    glfwGetWindowSize(window, &width, &height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({(float)width,50});
    ImGui::Begin("##Browse", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::Button("Hello!");



    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}