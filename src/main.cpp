#include <logger.hpp>
#include <cstdio>
#include "conv.hpp"
#include "structs.hpp"
#include <cstdlib>
#include <cstring>
#include <argtools.hpp>
#include <string>
#include <rendering/render.hpp>
using namespace std;

#define INTRO "Source MDL v49 --> v53 converter\n"\
              "Copyright 2022 MasterLiberty and headassbtw\n"\
              "Do not distribute\n"

bool DisableAnim = true; //sets the seq and anim count to 0, from before anims worked
bool IgnoreOtherFiles = false; //ignores if the other 3 files exist, mcor_pilot_male_br.mdl my beloved



string ChangeFileExtension(const char* file, const char* ext){
  string workingString = string(file);
  size_t dot = workingString.find_last_of('.');
  workingString.resize(dot);
  workingString.append(".");
  workingString.append(ext);
  return workingString;
}


int main(int argc, char *argv[]) {
  if(cmdOptionExists(argv,argv+argc,"--forcelog")){
    LogForceEnabled = true;
    LogEnabled = true;
    Logger::Init();
  }


  
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
    Logger::Info("Usage: %s [--noui] <mdl file> <vtx file> <vvd file> [phy file] [ruimesh file] [OPTIONS]\n", argv[0]);
    Logger::Info("args:\n");
    Logger::Info("\"--help\": Shows this\n");
    Logger::Info("\"--noui\": Disables the UI\n");
    return 0;
    Logger::Info("\"\": \n");
  }
  
  
  if(cmdOptionExists(argv,argv+argc,"--noui")){
    FileInfo inf;

    for(int i = 1; i < argc; i++) {
      std::string arg_str = std::string(argv[i]);

      size_t dot_pos = arg_str.find_last_of(".");
      if (dot_pos != std::string::npos) { // is a file
        if (arg_str.substr(dot_pos+1).compare("mdl") == 0) {
          if (inf.mdl->empty()) {
            inf.mdl = arg_str;
          } else {
            inf.out = arg_str;
          }
        } else if (arg_str.substr(dot_pos+1).compare("vtx") == 0) {
          inf.vtx = arg_str;
        } else if (arg_str.substr(dot_pos+1).compare("vvd") == 0) {
          inf.vvd = arg_str;
        } else if (arg_str.substr(dot_pos+1).compare("vvc") == 0) {
          inf.vvc = arg_str;
        } else if (arg_str.substr(dot_pos+1).compare("phy") == 0) {
          inf.phy = arg_str;
        } else if (arg_str.substr(dot_pos+1).compare("ruimesh") == 0) {
          inf.rui = arg_str;
        }
      }
    }
    if (inf.vtx->empty()) {
      Logger::Critical(".vtx file missing!\n");
      return 1;
    }

    if (inf.vvd->empty()) {
      Logger::Critical(".vvd file missing!\n");
      return 1;
    }

    if (inf.out->empty()) {
      string out_name = string(inf.mdl->c_str());
      auto dot = out_name.find_last_of('.');
      out_name.resize(dot);
      out_name.append("_conv.mdl");
      inf.out = out_name;
    }

    Logger::Info("mdl: %s\n", inf.mdl->c_str());
    Logger::Info("vtx: %s\n", inf.vtx->c_str());
    Logger::Info("vvd: %s\n", inf.vvd->c_str());
    Logger::Info("phy: %s\n", inf.phy->c_str());
    Logger::Info("rui: %s\n", inf.rui->c_str());
    Logger::Info("out: %s\n", inf.out->c_str());

    return Conversion::ReadHeader(inf);
  }
  else{
    return UI::Run();
  }
}