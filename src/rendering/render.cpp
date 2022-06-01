#include <GL/glew.h>
#include <GL/gl.h>
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
#include <GL/gl.h>
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
#include <conv.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std;


FileInfo fileinfo;

string rn(const char* file, const char* ext){
  string yo = string(file);
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

vector<Widgets::File*> files;
vector<Error> Errors;

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

void RenderGUI(){
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{4.0f,4.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{4.0f,4.0f});

    
    ImGuiIO& io = ImGui::GetIO();
        

    ImGui::SetNextWindowPos(
        {ImGui::GetMainViewport()->Pos.x,
        ImGui::GetMainViewport()->Pos.y+20}
    );


    ImGui::SetNextWindowSize({viewport_width,viewport_height-20});
    ImGui::SetNextWindowBgAlpha(0.2);
    ImGui::Begin("Box", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit", "ALT+F4")) {
                glfwSetWindowShouldClose(Window, true);
            }
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


    for(int i = 0; i < files.size();i++){
        files[i]->UI(viewport_width);
    }

    

    ImGui::BeginChild("OptionsBox",{(viewport_width/2.0f) - 6,0},true,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("Options");
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0.0f,0.0f});
    ImGui::BeginChild("##ConvertSmol", {0,0},false);
    ImGui::Checkbox("Override Attachments", &override_attachments);
    if(override_attachments) ImGui::InputInt("##Attachments", &attachments);
    ImGui::Checkbox("Override Sequences", &override_sequences);
    if(override_sequences) ImGui::InputInt("##Sequences", &sequences);
    ImGui::Checkbox("Override Animations", &override_animations);
    if(override_animations) ImGui::InputInt("##Animations", &animations);
    ImGui::Checkbox("Override Flags", &override_flags);
    if(override_flags) ImGui::InputInt("##Flags", &flags);

    ImGui::EndChild();
    ImGui::PopStyleVar(1);
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("ConvertBox",{(viewport_width/2.0f) - 6,-56},true,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
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
    if(ImGui::Button("Check",{(viewport_width/2.0f) - 6,24})){
        files[0]->errors = Tests::TestMDL(files[0]->BoxBuffer);
        files[2]->errors = Tests::TestVVD(files[2]->BoxBuffer);

        bool _block;
        for(auto f : files){
        if(f->isEnabled)
            for(auto e: f->errors){
                if(e.type == Blocking) _block = true;
            }
        }
        if(_block) blocked = true;
        else blocked = false;
    }

    ImGui::BeginDisabled(blocked);
    static bool popup = false;
    if(ImGui::Button("Convert",{(viewport_width/2.0f) - 6,24})){
        
        fileinfo.mdl = files[0]->BoxBuffer;
        if(files[1]->isEnabled) fileinfo.vtx = files[1]->BoxBuffer;
        if(files[2]->isEnabled) fileinfo.vvd = files[2]->BoxBuffer;
        if(files[3]->isEnabled) fileinfo.phy = files[3]->BoxBuffer;
        fileinfo.out = rn(files[0]->BoxBuffer,"conv");

        if(override_attachments) fileinfo.attachment_override =  attachments;
        if(override_sequences) fileinfo.sequence_override =  sequences;
        if(override_animations) fileinfo.animation_override =  animations;
        if(override_flags) fileinfo.flags_override =  flags;

        

        bingChilling = true;
        std::thread fp(Convert,0);
        fp.detach();
    }
    ImGui::EndDisabled();
    ImGui::EndGroup();
    ImGui::End();
    if(bingChilling){
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{8.0f,8.0f});
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        if(ImGui::Begin("ConvDemoProg",NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar)){
            ImGui::SetWindowSize({viewport_width,viewport_height});


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
                if(ImGui::Button("Continue", {viewport_width-16.0f,50})){
                    for(auto f : files){
                        f->errors.clear();
                        memset(f->BoxBuffer, '\0', 256);
                    }
                    bingChilling = false;
                    blocked = true;
                }
            }

            ImGui::End();
        }
        ImGui::PopStyleVar(1);
    }
    if(showAbout){
        if(ImGui::Begin("AboutWindow", &showAbout,ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Image((void*)(intptr_t)GRUNT_POG, {pog_size.x/3.5f,pog_size.y/3.5f});
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("MDLSHIT");
            ImGui::Text("- Source engine model converter, v49/47 to v53");
            ImGui::Text("By headassbtw and MasterLiberty");
            ImGui::Text("Compiled on %s",__DATE__);
            ImGui::Text("Compiled using %s, v%s",
                        #ifdef __MINGW32__
                        "MinGW",
                        #elif __GNUC__
                        "GCC",
                        #endif
                        __VERSION__);
            ImGui::Text("OpenGL Version: %s",glGetString(GL_VERSION));
            ImGui::Text("GPU: %s",glGetString(GL_RENDERER));
            
            ImGui::Text("FPS: %f",ImGui::GetIO().Framerate);
            ImGui::EndGroup();
            auto sz = ImGui::GetWindowSize();

            float xp = (viewport_width - sz.x)/2;
            float yp = (viewport_height - sz.y)/2;
            ImGui::SetWindowPos({xp,yp});
            ImGui::End();
            
        }
        ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("AboutWindow"));
    }


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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,GL_TRUE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    int ix,iy, channels;    

    GLFWimage images[1];
    auto grunt_pog = stbi_load_from_memory(Resources::Grunt, 380291, &images[0].width, &images[0].height, 0, 4);

    images[0].pixels = grunt_pog;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "mdlshit", NULL, NULL);
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
    ImGui::GetIO().Fonts->Build();
    auto style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg].w = 0.2;
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glfwSetWindowSizeCallback(Window, callback_name);
    glfwSetDropCallback(Window, drop_callback);

    glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();
    glViewport(0,0,viewport_width,viewport_height);
    glClearColor(0, 0, 0, 255);
    
const char* mdls = "*.mdl";
const char* vtxs = "*.vtx";
const char* vvds = "*.vvd";
const char* phys = "*.phy";

files.push_back(new Widgets::File("MDL",false,mdls));
files.push_back(new Widgets::File("VTX",true,vtxs));
files.push_back(new Widgets::File("VVD",true,vvds));
files.push_back(new Widgets::File("PHY",true,phys));

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
        //Render();
        Errors.clear();
        for(auto f : files){
            for(auto e : f->errors){
                Errors.push_back(e);
            }
        }
        


        //TestDockingUI();
        RenderGUI();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        glfwSwapBuffers(Window);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    while(glfwWindowShouldClose(Window) == 0);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();


    return 0;
}
