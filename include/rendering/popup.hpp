#pragma once
#include <gl.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <imgui.h>
namespace Popups{
    struct FileBrowser{
        GLFWwindow* window;
        GLuint shader;
        FileBrowser(GLFWwindow* parent);
        void Render();
        std::function<int(std::string)> callback;

        void RegisterCallback(std::function<int(std::string)> func);
        ~FileBrowser();
    };
}