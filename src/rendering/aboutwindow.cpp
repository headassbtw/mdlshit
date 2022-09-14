#include <rendering/render.hpp>
#include <imgui.h>
#include <resource.hpp>
#include <cstdint>
#include <GL/gl.h>
#include <string>
#include "stb_image.h"


GLuint Klules_Img;
int klules_width, klules_height;

void PatchNotesVersionHeader(std::string VersionID){
  ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
  ImGui::TextColored({1.0,0.8,0.8,1.0},VersionID.c_str());
  ImGui::PopFont();
}

void OpenLink(const char* site){
#ifdef WIN32
  ShellExecute(NULL, "open", site, NULL, NULL, SW_SHOWNORMAL);
#else
  system(("xdg-open " + std::string(site)).c_str());
#endif
}


void UI::SetupAboutWindow() {
  auto klules = stbi_load_from_memory(Resources::Klules, 4067, &klules_width, &klules_height, 0, 4);


  glGenTextures(1, &Klules_Img);
  glBindTexture(GL_TEXTURE_2D, Klules_Img);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, klules_width, klules_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, klules);
}
void UI::RenderAboutWindow(int grunt, float grunt_x, float grunt_y) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,{0,0});
  if(ImGui::BeginTabBar("AboutTabs",ImGuiTabBarFlags_None)){
    if(ImGui::BeginTabItem("About")){
      ImGui::Image((void*)(intptr_t)grunt, {178,178});
      ImGui::SameLine();
      ImGui::BeginGroup();
      ImGui::Text("MDLSHIT");
      ImGui::Text("- Source engine model converter, v49/47 to v53");
      ImGui::Text("By headassbtw and MasterLiberty");
      ImGui::Text("Eat my entire ass, wanderer");
      ImGui::Text("Compiled on %s",__DATE__);
      ImGui::Text("%s, v%s",
#ifdef __MINGW32__
        "MinGW",
#elif __GNUC__
        "GCC",
#elif __APPLE__
        "",
#endif
#if _MSC_VER //FUCK YOU MSVC
        "MSVC",
        _MSC_VER
#else
        __VERSION__
#endif
      );
      ImGui::Text("OpenGL Version: %s",glGetString(GL_VERSION));
      ImGui::Text("GPU: %s",glGetString(GL_RENDERER));
      ImGui::Text("FPS: %f",ImGui::GetIO().Framerate);
      ImGui::EndGroup();

      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Patch Notes")){

      PatchNotesVersionHeader("2.0.0");
      ImGui::BulletText("Added spot for extra physics data (not yet automated)");
      ImGui::BulletText("Added extra file validity checks");
      ImGui::BulletText("Forced UI to 60fps at all times, 20 when unfocused (previously vsync)");
      ImGui::BulletText("Stopped creating a log file, as nothing was being written anyway");
      ImGui::BulletText("Removed fonts i shouldn't use, shrunk filezise");
      ImGui::BulletText("Stopped resetting text files after conversion (shitty decision in hindsight)");

      PatchNotesVersionHeader("1.1.2");
      ImGui::BulletText("Attachment fix, probably");

      PatchNotesVersionHeader("1.1.1");
      ImGui::BulletText("Fixed a regression");

      PatchNotesVersionHeader("1.1.0");
      ImGui::BulletText("Fixes to attachments and extra components");

      PatchNotesVersionHeader("1.0.0");
      ImGui::BulletText("Initial Release!");
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Software Used")){
      ImGui::BulletText("");
      ImGui::SameLine();
      if(ImGui::SmallButton("Dear ImGui##About_ImGui_GH")){
        OpenLink("https://github.com/ocornut/imgui");
      }
      ImGui::BulletText("");
      ImGui::SameLine();
      if(ImGui::SmallButton("GLFW##About_GLFW_GH")){
        OpenLink("https://github.com/glfw/glfw");
      }
      ImGui::BulletText("");
      ImGui::SameLine();
      if(ImGui::SmallButton("GLEW##About_GLEW_GH")){
        OpenLink("https://github.com/nigels-com/glew");
      }
      ImGui::BulletText("");
      ImGui::SameLine();
      if(ImGui::SmallButton("TinyFileDialogs##About_TFD_GH")){
        OpenLink("https://github.com/native-toolkit/libtinyfiledialogs");
      }
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Contrubutors")){

      PatchNotesVersionHeader("Programming");
      ImGui::BulletText("headassbtw");
      ImGui::BulletText("MasterLiberty (Won't accept my windows key)");
      PatchNotesVersionHeader("Research");
      ImGui::BulletText("MasterLiberty (Still won't accept my windows key)");
      ImGui::BulletText("Rika");
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Special Thanks")){
      ImGui::BulletText("Mental Illness");
      ImGui::BulletText("Destiny 2 addiction");
      ImGui::BulletText("Sleep deprivation");
      ImGui::BulletText(""); ImGui::SameLine();
      if(ImGui::SmallButton("Spotify adblock##Spotify_adblock_GH")){
        OpenLink("https://github.com/abba23/spotify-adblock");
      }
      ImGui::BulletText("foobar2000 (my internet went out)");
      ImGui::BulletText("Embed perms in northstar general");
      ImGui::BulletText("I AM THE STORM THAT IS APPROOOOOOOOOOOOOOOOOOOOOOOOOOACHIIIIIIIIIIIIIIIIIIIIIING");
      ImGui::BulletText("Improvised munitions handbook");
      ImGui::BulletText(""); ImGui::SameLine(); ImGui::Image((void*)(intptr_t)Klules_Img, {13,13});
      if (ImGui::IsItemHovered())
      {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextWrapped("YOU THOUGHT I WAS LIMITED\nTO JUST TEXT, HUH?");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::PopStyleVar();


}
