#include "defs.hpp"
#include <cstring>
#include <tests.hpp>
#include <logger.hpp>
#include <fstream>


std::vector<Error> Tests::TestPHY(std::string path){
  std::vector<Error> rtn;
  std::ifstream test(path); 
    if (!test)
    {
      rtn.push_back({ErrorType::Blocking,std::string("File does not exist")});
    }
    else{
      test.seekg(20);
      char magic[4];
      test.read(magic, 4);
      if(
        strcmp(magic,"VPHY") >= 0
      ){
          rtn.push_back({ErrorType::Success,std::string("Valid PHY file")});
      }
      else{
        std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
        rtn.push_back({ErrorType::Blocking,err});
      }
      test.seekg(0);
      char version[1];
      test.read(version, 1);
      if(version[0] != 16){
        std::string err = std::string("Version ") + std::to_string((int)version[0]);
        rtn.push_back({ErrorType::Blocking,err});
      }
      else{
        rtn.push_back({ErrorType::Success,std::string("Version 16")});
      }

      test.close();
    }
  return rtn;
}
