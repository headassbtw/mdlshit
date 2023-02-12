#include <rendering/render.hpp>
#include <imgui.h>
#include <resource.hpp>
#include <cstdint>
#include <gl.hpp>
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
#if defined(WIN32)
  ShellExecute(NULL, "open", site, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  system(("open " + std::string(site)).c_str());
#else
  system(("xdg-open " + std::string(site)).c_str());
#endif
}

void SmallLinkButton(std::string id, std::string link){
  if(ImGui::SmallButton(id.c_str())){
    OpenLink(link.c_str());
  }
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
      if(ImGui::SmallButton("Project GitHub##MDLSHIT_GH")){
        OpenLink("https://github.com/headassbtw/mdlshit");
      }
      ImGui::EndGroup();

      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Patch Notes")){
      ImGui::BeginChild("PatchNotes##PATCHNOTES_SUBCHILD");

      PatchNotesVersionHeader("2.3.2");
      ImGui::BulletText("Threaded MDL reading");
      ImGui::BulletText("Fixed jiggle bones (probably)");
      ImGui::BulletText("Fixed RUI extraction/insertion on Windows (Other platforms unaffected)");
      ImGui::BulletText("Moved RUI extraction to another thread to avoid hitching the GUI");

      PatchNotesVersionHeader("2.3.0");
      ImGui::BulletText("Various QoL UI tweaks");

      PatchNotesVersionHeader("2.2.0");
      ImGui::BulletText("Complete backend rewrite");
      ImGui::BulletText("UI rework, support for more supplemental files");
      ImGui::BulletText("RUI stitching");

      PatchNotesVersionHeader("2.0.1");
      ImGui::BulletText("Fixes IKrules");
      ImGui::BulletText("Fixes jiggle bones (booba or cloth)");

      PatchNotesVersionHeader("2.0.0");
      ImGui::BulletText("Animation support");
      ImGui::BulletText("Removed override options (they did not work)");
      ImGui::BulletText("Moved around some UI elements");
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
      ImGui::EndChild();
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
      ImGui::BulletText("");
      ImGui::SameLine();
      if(ImGui::SmallButton("Half##About_HALF_GH")){
        OpenLink("https://github.com/melowntech/half");
      }
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Contrubutors")){
      ImGui::BeginChild("Contributors##CONTRIB_SUBCHILD");

      PatchNotesVersionHeader("Programming");

      ImGui::BulletText("headassbtw");
      ImGui::SameLine(); SmallLinkButton("GitHub##headassbtw_GH","https://github.com/headassbtw");


      ImGui::BulletText("MasterLiberty");
      ImGui::SameLine(); SmallLinkButton("GitHub##MasterLiberty_GH","https://github.com/MasterLiberty");


      PatchNotesVersionHeader("Research");

      ImGui::BulletText("MasterLiberty");
      ImGui::SameLine(); SmallLinkButton("GitHub##MasterLiberty_GH","https://github.com/MasterLiberty");


      ImGui::BulletText("Rika");
      ImGui::SameLine(); SmallLinkButton("GitHub##Rika_GH","https://github.com/IJARika");


      PatchNotesVersionHeader("Testing");

      ImGui::BulletText("Spoon");
      ImGui::SameLine(); SmallLinkButton("GitHub##Spoon_GH","https://github.com/ASpoonPlaysGames");

      ImGui::BulletText("Salmon");
      ImGui::SameLine(); SmallLinkButton("GitHub##Salmon_GH","https://github.com/TH3-S4LM0N");

      ImGui::BulletText("Oltrex");
      ImGui::SameLine(); SmallLinkButton("GitHub##Oltrex_GH","https://github.com/Oltrex");

      ImGui::BulletText("Titan Ok");


      PatchNotesVersionHeader("Moral Anti-Support");

      ImGui::BulletText("Titan Ok");

      ImGui::EndChild();
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
