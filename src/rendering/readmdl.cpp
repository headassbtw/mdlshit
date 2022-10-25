#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>

Widgets::File* mdl;

void UI::SetupReadMdlWindow(){
    mdl = new Widgets::File("MDL", false, "*.mdl");
}

void UI::RenderReadMdlWindow(int x, int y){
    mdl->UI(x-8);
    if(ImGui::Button("Read")){
        //READ SHIT!
    }
}