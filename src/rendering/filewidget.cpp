#include "defs.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <rendering/filewidget.hpp>
#include <tinyfiledialogs.h>
#include <unistd.h>


void Widgets::File::CheckErrors(){
    
    
    
}

void Widgets::File::TestFileDrop(double xpos, double ypos, const char* path){
    


}




void Widgets::File::UI(float win_width){
    auto bg = ImGui::GetBackgroundDrawList();
    auto style = ImGui::GetStyle();

    ImGui::BeginChild(boxname.c_str(), {win_width-16,(isEnabled)?64.0f:38.0f}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4.0f,4.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{4.0f,4.0f});


    auto p = ImGui::GetWindowPos();
    bounds.x = p.x - ImGui::GetMainViewport()->Pos.x;
    bounds.y = p.y - ImGui::GetMainViewport()->Pos.y;
    auto s = ImGui::GetWindowSize();
    bounds.z = p.x + s.x - ImGui::GetMainViewport()->Pos.x;
    bounds.w = p.y + s.y - ImGui::GetMainViewport()->Pos.y;

    const ImU32 col = ImGui::GetColorU32(ImGuiCol_WindowBg);
    float x = p.x + 4.0f;
    float y = p.y + 4.0f;
    float sz = 16;
    const float spacing = 10.0f;
    const float rounding = sz / 5.0f;

    float th = 1;
    bg->AddRectFilled(ImVec2(bounds.x+th, bounds.y+th), ImVec2(bounds.z-th, bounds.w-th), col);

    
    ImGui::BeginGroup();
    ImGui::BeginDisabled(!canBeDisabled);
    ImGui::SetNextItemWidth(win_width-(16));
    ImGui::Checkbox(name.c_str(), &isEnabled);
    //ImGui::Text("%s", name.c_str());
    ImGui::EndDisabled();
    

    if(isEnabled){
        ImGui::SetNextItemWidth(win_width-86);
        ImGui::InputTextWithHint(boxname.c_str(), "Type path or drag file",BoxBuffer, 256);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    if(ImGui::Button("Clear",{50,22})){
        memset(BoxBuffer, '\0', 256);
    }
    if(ImGui::Button("Browse",{50,22})){

        memset(BoxBuffer, '\0', 256);
        //char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        char const * selection = tinyfd_openFileDialog( // there is also a wchar_t version
        "Select file", // title
        NULL, // optional initial directory
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
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
    if(effect > 0){
        effect -= 0.05;

    }
    else{
        effect = 0;
    }
}
Widgets::File::File(std::string Name, bool canDisable, const char* ext){
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