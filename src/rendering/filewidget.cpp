#include "defs.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <rendering/filewidget.hpp>
#include <tinyfiledialogs.h>
#include <filesystem>
#include <unistd.h>
using std::filesystem::current_path;


void Widgets::File::CheckErrors(){
    
    
    
}

void Widgets::File::TestFileDrop(double xpos, double ypos, const char* path){
    


}




void Widgets::File::UI(float win_width){

    auto style = ImGui::GetStyle();
    
    
    

    ImGui::BeginChild(boxname.c_str(), {win_width-8,(isEnabled)?56.0f:30.0f}, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding);
    


    auto p = ImGui::GetWindowPos();
    bounds.x = p.x - ImGui::GetMainViewport()->Pos.x;
    bounds.y = p.y - ImGui::GetMainViewport()->Pos.y;
    auto s = ImGui::GetWindowSize();
    bounds.z = p.x + s.x - ImGui::GetMainViewport()->Pos.x;
    bounds.w = p.y + s.y - ImGui::GetMainViewport()->Pos.y;


    
    ImGui::BeginGroup();
    ImGui::BeginDisabled(!canBeDisabled);
    ImGui::SetNextItemWidth(win_width-(8));
    ImGui::Checkbox(name.c_str(), &isEnabled);
    //ImGui::Text("%s", name.c_str());
    ImGui::EndDisabled();
    

    if(isEnabled){
        ImGui::SetNextItemWidth(win_width-70);
        ImGui::InputTextWithHint(boxname.c_str(), "Path",BoxBuffer, 256);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    if(ImGui::Button("Clear",{50,22})){
        memset(BoxBuffer, '\0', 256);
    }
    if(ImGui::Button("Browse",{50,22})){
        char tmp[256];
        getcwd(tmp,256);

        memset(BoxBuffer, '\0', 256);
        //char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        char const * selection = tinyfd_openFileDialog( // there is also a wchar_t version
        "Select file", // title
        tmp, // optional initial directory
        1, // number of filter patterns
        exts, // 
        NULL, // optional filter description
        0 // forbid multiple selections
        );
        if(selection != NULL){
            strcpy(BoxBuffer, selection);
        }
    }
    }
    ImGui::EndGroup();

    ImGui::EndChild();
    if(effect > 0){
        effect -= 0.05;

    }
    else{
        effect = 0;
    }
}
Widgets::File::File(string Name, bool canDisable, const char* ext){
    name = Name;
    isEnabled = true;
    canBeDisabled = canDisable;
    std::string a = "##" + Name + "_Panel";
    std::string b = "##" + Name + "_Box";
    std::string c = "##" + Name + "_CheckBox";
    panelname = a;
    boxname = b;
    checkboxname = c;
    
    ext_count = 1;
    exts[0] = ext;
    memset(BoxBuffer,'\0',256);
    return;
}