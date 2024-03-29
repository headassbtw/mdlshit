#include <GL/glew.h>
#include <gl.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "rendering/shaders.hpp"
#include "resource.hpp"
#include <cstring>
#include <glm/common.hpp>
#include <cstdio>
#include <vector>
#include <rendering/render.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <defs.hpp>
#include <rendering/filewidget.hpp>
#include <rendering/popup.hpp>
#include <tests.hpp>
#include <mutex>
#include <chrono>
#include <thread>
#include <rendering/progress.hpp>
#include <structs.hpp>
#include <logger.hpp>
#include <conv.hpp>
#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif
//#define _GLFW_USE_MENUBAR
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool demoWindow, console;

FileInfo fileinfo;

std::string rn(const char* file, const char* ext){
  std::string yo = std::string(file);
  auto dot = yo.find_last_of('.');
  yo.resize(dot);
  yo.append("_");
  yo.append(ext);
  yo.append(".mdl");
  return yo;
}


GLFWwindow* Window;
Popups::FileBrowser* browser;
bool hasBrowser;

std::vector<Widgets::File*> files;
std::vector<Error> Errors;

int viewport_min_width = 856;
int viewport_min_height = 500;

float viewport_width = 856; //this is minecraft's default window size, but taller by 20px lmfao
float viewport_height = 500;
bool debugStats;

bool blocked = true;


char fileBrowseBuffer[50];
void FileMenu(std::string title){
    std::string a = "##" + title + "_Panel";
    ImGui::BeginChild(a.c_str(), {viewport_width-16,54}, true);

    ImGui::Text("%s", title.c_str());
    ImGui::InputTextEx("##", "File Path", fileBrowseBuffer, 50, {viewport_width-90,20},ImGuiInputTextFlags_None);
    ImGui::SameLine();
    ImGui::Button("W",{50,20});


    ImGui::EndChild();

}

void AboutWindow(){

}
bool showAbout, readMdl, conv;

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if(readMdl){
    UI::SetReadMdlFile(paths[0]);
    return;
  }
  for (int i = 0;  i < count;  i++){
    for(int j = 0; j < files.size();j++){
      if(
        files[j]->bounds.x <= xpos &&
        files[j]->bounds.z >= xpos &&
        files[j]->bounds.y <= ypos &&
        files[j]->bounds.w >= ypos
        ){
          strcpy(files[j]->BoxBuffer, paths[i]);
          blocked = true;
          for(auto f : files)
            f->errors.clear();
        }
    }
  }
}
float conv_demo_prog;
std::mutex conv_demo_prog_mutex;
bool bingChilling = false;




void Convert(int a){
    Conversion::ReadHeader(fileinfo);
    conv_demo_prog = 100;
}


GLuint GRUNT_POG;
ImVec2 pog_size;

bool _isBlockingTabSwitch = false; //this is so the invidual tabs can block switching, to prevent the user from reading the same file more than once

void depth_border(){
    auto bg = ImGui::GetBackgroundDrawList();

    ImVec4 bounds;
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
    //bg->AddRect(ImVec2(bounds.x+th, bounds.y+th), ImVec2(bounds.z-th, bounds.w-th), col, 0.0f, ImDrawFlags_None, th*2);      x += sz + spacing;  // Square with all rounded corners
}

void RenderConversionPanel(){
  ImGui::BeginGroup();

    int w = ImGui::GetContentRegionAvail().x*0.75;
    float sidebarWidth = viewport_width - (w + 4);

    files[0]->UI(w);
    files[1]->UI(w);
    files[2]->UI(w);
    files[3]->UI(w);
    
    if(ImGui::IsItemHovered()){
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextWrapped("Some random bullshit that rika thought we should add");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
    files[4]->UI(w);
    static int message_count = 0;
    ImGui::BeginChild("Logger",{(float)w-16,0},false,ImGuiWindowFlags_AlwaysUseWindowPadding);
    depth_border();
    //ImFont* monospace = ImGui::GetIO().Fonts->Fonts[2];

    //ImGui::PushFont(monospace);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {12.0,4.0});
    if (ImGui::BeginTable("table1", 2,ImGuiTableFlags_BordersInnerV))
    {
      ImGui::TableSetupColumn("small",ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("half");
      for (int row = 0; row < LoggerMessages.size(); row++)
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::PushItemWidth(150.0f);
        switch(LoggerMessages[row]->type){
            case 4: ImGui::TextColored({0.1,1.0,0.1,1.0},"Debug");break;
            case 1: ImGui::TextColored({1.0,1.0,0.2,1.0},"Notice");break;
            case 2: ImGui::TextColored({1.0,0.1,0.1,1.0},"Error");break;
            case 3: ImGui::TextColored({1.0,0.2,1.0,1.0},"Critical");break;
            case 0: ImGui::TextColored({1.0,1.0,1.0,1.0},"Info");break;
        }
        ImGui::PopItemWidth();
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s",LoggerMessages[row]->msg.c_str());
      }
      ImGui::EndTable();
    }
    if (message_count < LoggerMessages.size()) {
      ImGui::SetScrollHereY(1.0f);
      message_count = LoggerMessages.size();
    }
    ImGui::PopStyleVar();
    //ImGui::PopFont();
    ImGui::EndChild();
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    

  const ImU32   u32_min = 0,u32_max = UINT_MAX/2;
    
  const ImU32 w_bg_col = ImGui::GetColorU32(ImGuiCol_WindowBg);
    
    
    if(ImGui::Button("Clear File Paths##SidebarResetButton",{sidebarWidth,22})){
      for(auto file:files){
        memset(file->BoxBuffer, '\0', 256);
      }
    }

    ImGui::BeginChild("OptionsBox",{sidebarWidth, ((viewport_height-(99+26)) / 2.0f)},false,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysUseWindowPadding);
    depth_border();
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.0f,0.0f,0.0f,0.0f));
    //taking this out for now, because there's nothing there anymore besides the files
    //ImGui::Text("Advanced Options");
    //ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.0f,0.0f});
    ImGui::BeginChild("##ConvertSmol", {0,0},false);
    ImGui::Text("Extra Files");
    if(ImGui::IsItemHovered()){
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextWrapped("Some other files that enable extra features or supplemental data,\n not required but will assist in more complex models");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
    ImGui::Separator();
    files[5]->UI(sidebarWidth);
    if(ImGui::IsItemHovered()){
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextWrapped("This is a section present after the PHY, only touch this if you know what you're doing.");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
    files[6]->UI(sidebarWidth);
    files[7]->UI(sidebarWidth);
    files[8]->UI(sidebarWidth);

    ImGui::EndChild(); //ConvertSmol
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor();
    ImGui::EndChild();//OptionsBox

    ImGui::BeginGroup();
    ImGui::BeginChild("ConvertBox",{(0),((viewport_height-(99+26)) / 2.0f)},false,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysUseWindowPadding);
    depth_border();
    ImGui::Text("Problems");
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.0f,0.0f});
    ImGui::BeginChild("##ConvertSmol", {0,0},false);


    for(auto f : files){
        ImGui::SetNextItemOpen(true);
        if(f->isEnabled)
        if(ImGui::TreeNode(f->name.c_str())){
            for(auto e: f->errors){
                ImVec4 col;
                switch (e.type) {
                case Blocking:
                    col = {1.0,0.0,0.0,1.0}; break;
                case Critical:
                    col = {1.0,0.0,0.7,1.0}; break;
                case Warning:
                    col = {1.0,1.0,0.0,1.0}; break;
                case Neutral:
                    col = {1.0,1.0,1.0,1.0}; break;
                case Success:
                    col = {0.0,1.0,0.0,1.0}; break;
                }
                ImGui::TextColored(col, "%s", e.message.c_str());
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(1);


    
    
    ImGui::EndChild();
    if(ImGui::Button("Check",{sidebarWidth,24})){
      int total = 0;
      int bad = 0;
      int good = 0;
      for(auto f : files){
        f->errors.clear();
      }
      files[0]->errors = Tests::TestMDL(files[0]->BoxBuffer);
      files[1]->errors = Tests::TestVTX(files[1]->BoxBuffer);
      files[2]->errors = Tests::TestVVD(files[2]->BoxBuffer);
      files[3]->errors = Tests::TestVVC(files[3]->BoxBuffer);
      files[4]->errors = Tests::TestPHY(files[4]->BoxBuffer);

      bool _block;
      for(auto f : files){
      if(f->isEnabled)
        for(Error e: f->errors){
          total++;
          if(e.type == Blocking){
            bad++;
            _block = true;
          }
          else{
            good++;
          }
        }
      }
      blocked = (good < total);

      if(blocked){
        Logger::Error("Conversion blocked, not all criteria met!\n");
      }
      Logger::Notice("%i good, %i bad, %i total\n",good,bad,total);
    }

    ImGui::BeginDisabled(blocked);
    static bool popup = false;
    if(!blocked){
      ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.6f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.7f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.8f));
    }
    if(ImGui::Button("Convert",{sidebarWidth,24})){
        Logger::Debug("Starting conversion\n");
                                fileinfo.mdl  = files[0]->BoxBuffer;
        if(files[1]->isEnabled) fileinfo.vtx  = files[1]->BoxBuffer;
        if(files[2]->isEnabled) fileinfo.vvd  = files[2]->BoxBuffer;
        if(files[3]->isEnabled) fileinfo.vvc  = files[3]->BoxBuffer;
        if(files[4]->isEnabled) fileinfo.phy  = files[4]->BoxBuffer;
        if(files[5]->isEnabled) fileinfo.pfb  = files[5]->BoxBuffer;
        if(files[6]->isEnabled) fileinfo.str  = files[6]->BoxBuffer;
        if(files[7]->isEnabled) fileinfo.aabb = files[7]->BoxBuffer;
        if(files[8]->isEnabled) fileinfo.rui  = files[8]->BoxBuffer;
        fileinfo.out = rn(files[0]->BoxBuffer,"conv");

        ImGui::OpenPopup("Status##ConvertModal");
        Logger::Debug("Spinning up conversion thread\n");
        std::thread fp(Convert,0);
        fp.detach();
        Logger::Debug("Conversion thread detached\n");
    }
    if(!blocked) ImGui::PopStyleColor(3);
    ImGui::EndGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.0f);

    if(ImGui::BeginPopupModal("Status##ConvertModal",NULL,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)){
      _isBlockingTabSwitch = true;
      auto sz = ImGui::GetWindowSize();
      ImGui::SetWindowSize({300,200});
      ImGui::SetWindowPos({(viewport_width/2) - (150),(viewport_height/2) - (50)});

      if(UI::Progress.MainTask.Show()){
        ImGui::Text("%s",UI::Progress.MainTask.Name().c_str());
        ImGui::ProgressBar(UI::Progress.MainTask.Progress());
      }
      if(UI::Progress.SubTask.Show()){
        ImGui::Text("%s",UI::Progress.SubTask.Name().c_str());
        ImGui::ProgressBar(UI::Progress.SubTask.Progress());
      }


      if(!UI::Progress.MainTask.Show()){
        ImGui::Text("Finished!");
        if(ImGui::Button("Continue",{-20,30})){
          for(auto f : files){
            f->errors.clear();
          }
          blocked = true;
          ImGui::CloseCurrentPopup();
          _isBlockingTabSwitch = false;
        }
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::EndDisabled();
    ImGui::EndGroup();
}



void RenderGUI(){
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4.0f,4.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{8.0f,8.0f});


    ImGuiIO& io = ImGui::GetIO();
    if(demoWindow) ImGui::ShowDemoWindow();
    auto MainViewport = ImGui::GetMainViewport(); //caching this because it's needed for multi-viewport imgui stuff and i don't want to rework it later

    ImGui::SetNextWindowSize({MainViewport->Size.x/2,21});
    ImGui::SetNextWindowPos({MainViewport->Pos.x,MainViewport->Pos.y});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.0f);
    static auto u32_transparent_menubar_bg = ImGui::GetColorU32(ImGuiCol_WindowBg,0.0f);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg,u32_transparent_menubar_bg);
    if(ImGui::Begin("MenuBarContainer", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav)){
      if(ImGui::BeginMenuBar()){
          if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Quit", "ALT+F4")) {
            glfwSetWindowShouldClose(Window, true);
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
          if(!LogForceEnabled){
            if(!LogEnabled) if (ImGui::MenuItem("Enable Log File", "")) {Logger::Init(); LogEnabled = true;}
            if(LogEnabled) if (ImGui::MenuItem("Disable Log File", "")) {Logger::End(); LogEnabled = false;}
          }
           
          ImGui::MenuItem("Show Demo Window", "", &demoWindow);

          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) {
                showAbout = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      ImGui::End();
    }
    ImGui::PopStyleColor();
    ImGui::SetNextWindowSize({MainViewport->Size.x/2,23-16});
    ImGui::SetNextWindowPos({MainViewport->Pos.x+((MainViewport->Size.x*0.75f)-16),MainViewport->Pos.y});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.0f,0.0f});
    ImGui::BeginDisabled(_isBlockingTabSwitch);
    if(ImGui::Begin("MainWindowSwapButtons", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav)){
      if(ImGui::BeginTabBar("MainWindowSwapBar", ImGuiTabBarFlags_FittingPolicyMask_)){
        
        if(ImGui::BeginTabItem("Convert")){
          conv = true;
          readMdl = false;
          ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Tools")){
          readMdl = true;
          conv = false;
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
      std::string version = "v2.3.2";
      float text_width = ImGui::CalcTextSize(version.c_str()).x;

      ImGui::SameLine(((MainViewport->Size.x*0.25f)+16)-(text_width+10));
      ImGui::Text("%s",version.c_str());
      ImGui::End();
    }
    ImGui::EndDisabled();
    
    ImGui::PopStyleVar(2);
    ImGui::GetBackgroundDrawList()->AddRectFilled({MainViewport->Pos.x,MainViewport->Pos.y}, {MainViewport->Pos.x+MainViewport->Size.x,MainViewport->Pos.y+23}, ImGui::GetColorU32(ImGuiCol_MenuBarBg));
    ImGui::GetBackgroundDrawList()->AddLine({0,21}, {MainViewport->Size.x,MainViewport->Pos.y+21}, ImGui::GetColorU32(ImGuiCol_TabActive), 1.0f);
    

    ImGui::SetNextWindowPos(
        {ImGui::GetMainViewport()->Pos.x,
        ImGui::GetMainViewport()->Pos.y+23}
    );


    ImGui::SetNextWindowSize({viewport_width,viewport_height-23});
    ImGui::SetNextWindowBgAlpha(0.2);
    ImGui::Begin("Box", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);
    
    if(conv) RenderConversionPanel();

    if(readMdl) _isBlockingTabSwitch =  UI::RenderReadMdlWindow(viewport_width-4,viewport_height-27);
    

    ImGui::End();
    if(showAbout){
      if(ImGui::Begin("About", &showAbout,ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse)){
        UI::RenderAboutWindow(GRUNT_POG,pog_size.x,pog_size.y);
        auto sz = ImGui::GetWindowSize();

        float xp = (viewport_width - sz.x)/2;
        float yp = (viewport_height - sz.y)/2;
        ImGui::SetWindowPos({xp,yp});
        ImGui::End();
      }
      ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("About"));
    }
    //ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("MenuBarContainer"));
    //ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("MainWindowSwapButtons"));
    ImGui::PopStyleVar(2);
}

void callback_name(GLFWwindow* window, int xpos, int ypos){
    viewport_height = ypos;  
    viewport_width = xpos;
}

int UI::Run(){
    glewExperimental = true;
    if(!glfwInit()){
        fprintf(stderr, "Could not initialize GLFW\n");
    }


    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,GL_TRUE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    int ix,iy, channels;    

    GLFWimage images[1];

    auto grunt_pog = stbi_load_from_memory(Resources::Grunt, 81765, &images[0].width, &images[0].height, 0, 4);


    images[0].pixels = grunt_pog;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    Window = glfwCreateWindow(viewport_width, viewport_height, "mdlshit", NULL, NULL);
    if(Window == NULL){
        fprintf(stderr, "Failed to open window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);
    //glfwSwapInterval(1);
    
    glewExperimental = true;
    if(glewInit() != GLEW_OK){
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }


    
   glGenTextures(1, &GRUNT_POG);
    glBindTexture(GL_TEXTURE_2D, GRUNT_POG);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    #if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    #endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[0].width, images[0].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, grunt_pog);
    pog_size.x = images[0].width;
    pog_size.y = images[0].height;

    UI::SetupAboutWindow();
    UI::SetupReadMdlWindow();

    glfwSetWindowSizeLimits(Window, viewport_min_width, viewport_min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowIcon(Window, 1, images);
    glfwSetWindowSize(Window, viewport_width, viewport_height);
    glfwShowWindow(Window);

    stbi_image_free(grunt_pog);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    
    

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().IniFilename = NULL;
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::OpenSans_compressed, Resources::OpenSans_compressed_size, 16);
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::NvidiaSans_compressed, Resources::NvidiaSans_compressed_size, 20);
    ImGui::GetIO().Fonts->Build();

    ImGui::GetStyle().ScrollbarRounding = 0.0f;
    ImGui::GetStyle().TabRounding = 0.0f;
    ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
    ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.7f, 0.7f, 0.7f, 0.4f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.08f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.3f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.54f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 0.08f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.24f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.47f);
    ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.35f);
    ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(0.63f, 0.63f, 0.63f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImVec4(0.71f, 0.71f, 0.71f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);

    ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.67f);
    ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetWindowSizeCallback(Window, callback_name);
    glfwSetDropCallback(Window, drop_callback);

    glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();
    glViewport(0,0,viewport_width,viewport_height);
    glClearColor(0.1, 0.1, 0.1, 255);
    
const char* mdls = "*.mdl";
const char* vtxs = "*.vtx";
const char* vvds = "*.vvd";
const char* vvcs = "*.vvc";
const char* phys = "*.phy";
const char* pfbs = "*.bin";
const char* ruis = "*.rui";
const char* json = "*.json";

files.push_back(new Widgets::File("MDL",false,mdls));
files.push_back(new Widgets::File("VTX",true,vtxs));
files.push_back(new Widgets::File("VVD",true,vvds));
auto vvc_uiblock = new Widgets::File("VVC",true,pfbs);
vvc_uiblock->isEnabled = false;
files.push_back(vvc_uiblock);
files.push_back(new Widgets::File("PHY",true,phys));
auto phyblock_uiblock = new Widgets::File("PHY Block",true,pfbs);
phyblock_uiblock->isEnabled = false;
files.push_back(phyblock_uiblock);
auto extrastructs_uiblock = new Widgets::File("Extra Structs",true,json);
extrastructs_uiblock->isEnabled = false;
files.push_back(extrastructs_uiblock);
auto aabbtree_uiblock = new Widgets::File("AABB Tree",true,pfbs);
aabbtree_uiblock->isEnabled = false;
files.push_back(aabbtree_uiblock);
auto rui_uiblock = new Widgets::File("RUI",true,ruis);
rui_uiblock->isEnabled = false;
files.push_back(rui_uiblock);

for(int i = 0; i < files.size();i++){
    files[i]->CheckErrors();
}
hasBrowser = true;  
    double lastTime;
    double currentTime;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    do{
        //https://discourse.glfw.org/t/frame-limiting/70
        //:clueless:
        currentTime = glfwGetTime();
        if(currentTime - lastTime >= 1.0 / (glfwGetWindowAttrib(Window, GLFW_FOCUSED)?60:20)) {
          lastTime = currentTime;

          glViewport(0, 0, viewport_width, viewport_height);
          glClearColor(0.1, 0.1, 0.1, 255);
          ImGui_ImplOpenGL3_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          glfwPollEvents();
          Errors.clear();
          for (auto f: files) {
            for (auto e: f->errors) {
              Errors.push_back(e);
            }
          }
          RenderGUI();
          ImGui::Render();
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
          if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
          }
          glfwSwapBuffers(Window);
        }
    }
    while(glfwWindowShouldClose(Window) == 0);
    UI::DestroyReadMdlWindow(); //there's some shaders and textures in here
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    if(LogEnabled) Logger::End();
    return 0;
}