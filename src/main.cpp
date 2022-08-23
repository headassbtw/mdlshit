#include <logger.hpp>
#include <cstdio>
#include "conv.hpp"
#include "structs.hpp"
#include <cstdlib>
#include <cstring>
#include <argtools.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <rendering/render.hpp>
using namespace std;

#define INTRO "Source MDL v49 --> v53 converter\n"\
              "Copyright 2022 MasterLiberty and headassbtw\n"\
              "Do not distribute\n"

bool DisableAnim = true; //sets the seq and anim count to 0, from before anims worked
bool IgnoreOtherFiles = false; //ignores if the other 3 files exist, mcor_pilot_male_br.mdl my beloved



string hh(const char* file, const char* ext){
  string yo = string(file);
  auto dot = yo.find_last_of('.');
  yo.resize(dot);
  yo.append(".");
  yo.append(ext);
  return yo;
}


int main(int argc, char *argv[]) {
  Logger::Init();
  //UI::main(argc,argv);
  //Logger::End();


  
  printf(INTRO);

  
  printf("Compiled on %s, using %s v%s\n", __DATE__,
  #ifdef __MINGW32__
  "MinGW",
  #elif __GNUC__
  "GCC",
  #else
  "Other",
  #endif
  #if _MSC_VER
  "MSVC",
  _MSC_VER
  #else
  __VERSION__
  #endif
  );

  if(cmdOptionExists(argv,argv+argc,"--help")){
    Logger::Info("mdlshit:\n");
    Logger::Info("Usage: %s [--noui] <mdl file> [OPTIONS]\n", argv[0]);
    Logger::Info("---------------------------------------------------\n");
    Logger::Notice("3 other files with names EXACTLY THE SAME NAME except for extension, must exist in the same folder as the .mdl file,\n");
    Logger::Notice("with extensions of vtx, vvd, and phy\n");
    Logger::Info("---------------------------------------------------\n");
    Logger::Info("args:\n");
    Logger::Info("\"--anims\": [EXPERIMENTAL] Enables animations\n");
    Logger::Info("\"--help\": Shows this\n");
    Logger::Info("\"--noui\": Disables the UI\n");
    return 0;
    Logger::Info("\"\": \n");
  }
  bool anim;
  if(cmdOptionExists(argv,argv+argc,"--anims")){
    anim = true;
    Logger::Info("Animations enabled\n");
  }
  
  if(cmdOptionExists(argv,argv+argc,"--noui")){

    if (argc < 2) {
      Logger::Critical("Invalid arguments\n");
      Logger::Info("Usage: %s <mdl file> [OPTIONS]\n", argv[0]);
      return 1;
    }
    FileInfo inf;
    inf.mdl = argv[2];
    inf.vtx = hh(argv[2],"vtx");
    inf.vvd = hh(argv[2],"vvd");
    inf.phy = hh(argv[2],"phy");
    if(!anim) inf.animation_override = 0;
    if(!anim) inf.sequence_override = 0;
    string yo = string(argv[2]);
    auto dot = yo.find_last_of('.');
    yo.resize(dot);
    yo.append("_conv.mdl");
    inf.out = yo;


    return Conversion::ReadHeader(inf);
  }
  else{
    return UI::Run();
  }
}