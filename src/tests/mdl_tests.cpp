#include "defs.hpp"
#include <cstring>
#include <tests.hpp>
#include <fstream>


std::vector<Error> Tests::TestMDL(std::string path){
  std::vector<Error> rtn;
  std::ifstream test(path); 
    if (!test)
    {
      rtn.push_back({ErrorType::Blocking,std::string("File does not exist")});
    }
    else{

      char magic[4];
      test.read(magic, 4);
      if(strcmp("IDST",magic) >= 0){
        rtn.push_back({ErrorType::Success,std::string("Valid MDL file")});

        char ver[1];
        test.read(ver, 1);
        if((int)ver[0] == 49){
          rtn.push_back({ErrorType::Success,std::string("Version 49")});
        }
        else if((int)ver[0] == 47){
          rtn.push_back({ErrorType::Warning,std::string("Version 47")});
        }
        else{
          rtn.push_back({ErrorType::Blocking,std::string("Version ") + std::to_string((int)ver[0])});
        }
      }
      else{
        rtn.push_back({ErrorType::Blocking,std::string("Invalid MDL file")});
      }
      test.close();
    }
  return rtn;
}
