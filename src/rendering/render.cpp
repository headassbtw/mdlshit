#include <GL/glew.h>
#include <gl.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"
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

int window_min_x = 500;
int window_min_y = 500;
int log_height = 200;

float viewport_width = 500;
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

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  printf("%f %f\n", xpos, ypos);

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



int attachments, animations, flags, sequences;
bool override_attachments, override_animations, override_flags, override_sequences;


float conv_demo_prog;
std::mutex conv_demo_prog_mutex;
bool bingChilling = false;

bool showAbout;


void Convert(int a){
    Conversion::ReadHeader(fileinfo);
    conv_demo_prog = 100;
}


GLuint GRUNT_POG;
ImVec2 pog_size;


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


void RenderGUI(){
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4.0f,4.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{8.0f,8.0f});

    
    ImGuiIO& io = ImGui::GetIO();
    if(demoWindow) ImGui::ShowDemoWindow();

    ImGui::SetNextWindowPos(
        {ImGui::GetMainViewport()->Pos.x,
        ImGui::GetMainViewport()->Pos.y+23}
    );


    ImGui::SetNextWindowSize({viewport_width,viewport_height-23});
    ImGui::SetNextWindowBgAlpha(0.2);
    ImGui::Begin("Box", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.0f);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Quit", "ALT+F4")) {
            glfwSetWindowShouldClose(Window, true);
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
          ImGui::MenuItem("Show Demo Window", "", &demoWindow);
          /*
          if(ImGui::MenuItem("Show Log Output", "", &console)){
            if(console){
              glfwSetWindowSizeLimits(Window, 500, 700, GLFW_DONT_CARE, GLFW_DONT_CARE);
            if(viewport_height<700)
              glfwSetWindowSize(Window,viewport_width,700);
            }
            else{
              glfwSetWindowSizeLimits(Window, 500, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
              glfwSetWindowSize(Window,viewport_width,viewport_height);
            }
          }
           */
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) {
                showAbout = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::PopStyleVar();

    for(int i = 0; i < 3;i++){
        files[i]->UI(viewport_width);
    }
    files[3]->UI((viewport_width/2)+6);
    ImGui::SameLine();
    files[4]->UI((viewport_width/2)+6);
    if(ImGui::IsItemHovered()){
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextWrapped("This is a section present after the PHY, only touch this if you know what you're doing.");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }


    

    ImGui::BeginChild("OptionsBox",{(viewport_width/2.0f) -10, (float)((console)?-log_height:0)},false,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysUseWindowPadding);
    depth_border();
    ImGui::Text("Options");
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.0f,0.0f});
    ImGui::BeginChild("##ConvertSmol", {0,0},false);
    ImGui::Checkbox("Override Attachments",                   &override_attachments );
    if(override_attachments) ImGui::InputInt("##Attachments", &attachments          );
    ImGui::Checkbox("Override Sequences",                     &override_sequences   );
    if(override_sequences)   ImGui::InputInt("##Sequences",   &sequences            );
    ImGui::Checkbox("Override Animations",                    &override_animations  );
    if(override_animations)  ImGui::InputInt("##Animations",  &animations           );
    ImGui::Checkbox("Override Flags",                         &override_flags       );
    if(override_flags)       ImGui::InputInt("##Flags",       &flags                );
    ImGui::Text("Disable Conversions");
    ImGui::Separator();
    ImGui::Checkbox("Bones",            &fileinfo.disable_bones         );
    ImGui::Checkbox("Attachments",      &fileinfo.disable_attachments   );
    ImGui::Checkbox("Hitbox Sets",      &fileinfo.disable_hitboxsets    );
    ImGui::Checkbox("Animations",       &fileinfo.disable_animations    );
    ImGui::Checkbox("Sequences",        &fileinfo.disable_sequences     );
    ImGui::Checkbox("Body Parts",       &fileinfo.disable_bodyparts     );
    ImGui::Checkbox("Include Models",   &fileinfo.disable_includemodels );
    ImGui::Checkbox("Textures",         &fileinfo.disable_textures      );

    ImGui::EndChild();
    ImGui::PopStyleVar(1);
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("ConvertBox",{(viewport_width/2.0f) - 10,-(56 + (float)((console)?log_height:0))},false,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysUseWindowPadding);
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
    if(ImGui::Button("Check",{(viewport_width/2.0f) - 10,24})){
      int total = 0;
      int bad = 0;
      int good = 0;
      for(auto f : files){
        f->errors.clear();
      }
      files[0]->errors = Tests::TestMDL(files[0]->BoxBuffer);
      files[1]->errors = Tests::TestVTX(files[1]->BoxBuffer);
      files[2]->errors = Tests::TestVVD(files[2]->BoxBuffer);
      files[3]->errors = Tests::TestPHY(files[3]->BoxBuffer);

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
    if(ImGui::Button("Convert",{(viewport_width/2.0f) - 10,24})){
        Logger::Debug("Starting conversion\n");
                                fileinfo.mdl = files[0]->BoxBuffer;
        if(files[1]->isEnabled) fileinfo.vtx = files[1]->BoxBuffer;
        if(files[2]->isEnabled) fileinfo.vvd = files[2]->BoxBuffer;
        if(files[3]->isEnabled) fileinfo.phy = files[3]->BoxBuffer;
        if(files[4]->isEnabled) fileinfo.pfb = files[4]->BoxBuffer;
        fileinfo.out = rn(files[0]->BoxBuffer,"conv");

        if(override_attachments) fileinfo.attachment_override =  attachments;
        if(override_sequences) fileinfo.sequence_override =  sequences;
        if(override_animations) fileinfo.animation_override =  animations;
        if(override_flags) fileinfo.flags_override =  flags;

        ImGui::OpenPopup("Status##ConvertModal");
        Logger::Debug("Spinning up conversion thread\n");
        std::thread fp(Convert,0);
        fp.detach();
        Logger::Debug("Conversion thread detached\n");
    }
    if(!blocked) ImGui::PopStyleColor(3);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.0f);

    if(ImGui::BeginPopupModal("Status##ConvertModal",NULL,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)){
        auto sz = ImGui::GetWindowSize();
        ImGui::SetWindowSize({300,100});
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
                        memset(f->BoxBuffer, '\0', 256);
                    }
                    blocked = true;
                    ImGui::CloseCurrentPopup();
                }
            }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::EndDisabled();
    ImGui::EndGroup();

    //uncomment this if tests break again
    /*
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextWrapped("Checks to verify that the input files are correct do not work currently,\nthis may cause unusual errors.\nYOU HAVE BEEN WARNED.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    */


    if(console){
        ImGui::BeginChild("Logger",{0,0},false,ImGuiWindowFlags_AlwaysUseWindowPadding);
        depth_border();
        ImFont* monospace = ImGui::GetIO().Fonts->Fonts[2];

        ImGui::PushFont(monospace);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {12.0,4.0});

        if (ImGui::BeginTable("table1", 2,ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableSetupColumn("small",ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("half");
            for (int row = LoggerMessages.size(); row >0; row--)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushItemWidth(150.0f);
                switch(LoggerMessages[row-1]->type){
                    case 4: ImGui::TextColored({0.1,1.0,0.1,1.0},"Debug");break;
                    case 1: ImGui::TextColored({1.0,1.0,0.2,1.0},"Notice");break;
                    case 2: ImGui::TextColored({1.0,0.1,0.1,1.0},"Error");break;
                    case 3: ImGui::TextColored({1.0,0.2,1.0,1.0},"Critical");break;
                    case 0: ImGui::TextColored({1.0,1.0,1.0,1.0},"Info");break;
                }
                ImGui::PopItemWidth();
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s",LoggerMessages[row-1]->msg.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::SetScrollHereY(0.001f);
        ImGui::PopStyleVar();
        ImGui::PopFont();
        ImGui::EndChild();
    }

    
    ImGui::End();
    if(showAbout){
      if(ImGui::Begin("About", &showAbout,ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)){
        UI::RenderAboutWindow(GRUNT_POG,pog_size.x,pog_size.y);
        auto sz = ImGui::GetWindowSize();

        float xp = (viewport_width - sz.x)/2;
        float yp = (viewport_height - sz.y)/2;
        ImGui::SetWindowPos({xp,yp});
        ImGui::End();
      }
      ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("About"));
    }
    ImGui::PopStyleVar(2);
}

void callback_name(GLFWwindow* window, int xpos, int ypos){
    viewport_height = ypos;  
    viewport_width = xpos;
}


int UI::Run(){
    viewport_height = WINDOW_HEIGHT; viewport_width = WINDOW_WIDTH;
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

    auto grunt_pog = stbi_load_from_memory(Resources::Grunt, 380291, &images[0].width, &images[0].height, 0, 4);


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
    glfwShowWindow(Window);
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

    glfwSetWindowSizeLimits(Window, 500, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowIcon(Window, 1, images);

    stbi_image_free(grunt_pog);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    
    

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().IniFilename = NULL;
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::SegoeUI_compressed, Resources::SegoeUI_compressed_size, 16);
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::Impact_compressed, Resources::Impact_compressed_size, 16);
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::Menlo_compressed, Resources::Menlo_compressed_size, 16);
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Resources::ComicSans_compressed, Resources::ComicSans_compressed_size, 16);
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
const char* phys = "*.phy";
const char* pfbs = "*.bin";

files.push_back(new Widgets::File("MDL",false,mdls));
files.push_back(new Widgets::File("VTX",true,vtxs));
files.push_back(new Widgets::File("VVD",true,vvds));
files.push_back(new Widgets::File("PHY",true,phys));
auto phyblock_uiblock = new Widgets::File("PHY Block",true,pfbs);
phyblock_uiblock->isEnabled = false;
files.push_back(phyblock_uiblock);

for(int i = 0; i < files.size();i++){
    files[i]->CheckErrors();
}
hasBrowser = true;  
    
    do{
        glViewport(0,0,viewport_width,viewport_height);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        Errors.clear();
        for(auto f : files){
          for(auto e : f->errors){
            Errors.push_back(e);
          }
        }
        RenderGUI();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        glfwSwapBuffers(Window);
    }
    while(glfwWindowShouldClose(Window) == 0);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();


    return 0;
}
