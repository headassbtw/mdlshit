#pragma once
#include <defs.hpp>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
using namespace glm;

namespace Widgets{
    struct File{
        private:
        bool canBeDisabled;
        std::string panelname;
        std::string checkboxname;
        public:
        bool isEnabled = true;
        std::string name;
        std::string boxname;
        float effect;
        std::vector<Error> errors;
        vec4 bounds;
        char BoxBuffer[256];
        const char* exts[1];
        int ext_count;
        void BG(std::vector<vec2>* verts, std::vector<vec3>* colors);
        void CheckErrors();
        void UI(float win_width);
        void TestFileDrop(double xpos, double ypos, const char* path);
        File(std::string Name, bool canDisable, const char* ext);
    };
}