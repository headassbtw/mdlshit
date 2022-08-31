#include "defs.hpp"
#include <cstring>
#include <tests.hpp>
#include <logger.hpp>
#include <fstream>


std::vector<Error> Tests::TestVTX(std::string path){
  std::vector<Error> rtn;
  std::ifstream test(path); 
    if (!test)
    {
      rtn.push_back({ErrorType::Blocking,std::string("File does not exist")});
    }
    else{
      int version;
      test.read((char*)&version,4);
      if((int)version != 7){
        std::string error = "Version mismatch, expected \"7\", got \"" + std::to_string(version) + "\"";
        rtn.push_back({ErrorType::Blocking,error});
      }
      else{
        rtn.push_back({ErrorType::Success,std::string("Version " + std::to_string(version))});
      }
      test.close();
    }
  return rtn;
}
