#pragma once
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <imgui.h>
using namespace std;
namespace Popups{
    struct FileBrowser{
        GLFWwindow* window;
        GLuint shader;
        FileBrowser(GLFWwindow* parent);
        void Render();
        function<int(string)> callback;

        void RegisterCallback(function<int(string)> func);
        ~FileBrowser();
    };
}