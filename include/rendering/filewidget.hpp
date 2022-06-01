#pragma once
#include <defs.hpp>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
using namespace glm;

using namespace std;
namespace Widgets{
    struct File{
        private:
        bool canBeDisabled;
        string panelname;
        string checkboxname;
        public:
        bool isEnabled = true;
        string name;
        string boxname;
        float effect;
        vector<Error> errors;
        vec4 bounds;
        char BoxBuffer[256];
        const char* exts[1];
        int ext_count;
        void BG(vector<vec2>* verts, vector<vec3>* colors);
        void CheckErrors();
        void UI(float win_width);
        void TestFileDrop(double xpos, double ypos, const char* path);
        File(string Name, bool canDisable, const char* ext);
    };
}