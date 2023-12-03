#include "defs.hpp"
#include <cstring>
#include <tests.hpp>
#include <logger.hpp>
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
      if (std::string(magic) == std::string("IDST") || std::strcmp("IDST", magic))
      {
          rtn.push_back({ ErrorType::Success,std::string("Valid MDL file") });

          char ver[1];
          test.read(ver, 1);

          switch ((int)ver[0])
          {
          case 49: rtn.push_back({ ErrorType::Success,std::string("Version 49") });
              break;
          case 52: rtn.push_back({ ErrorType::Success,std::string("Version 52") });
              break;
          case 48: rtn.push_back({ ErrorType::Warning,std::string("Version 48") });
              break;

          default:
            {
                std::string err = std::string("Version ") + std::to_string((int)ver[0]);
                rtn.push_back({ ErrorType::Blocking,err });
                Logger::Critical("%s\n", err.c_str());
                break;
            }
          }
      }
      else {
          std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
          rtn.push_back({ ErrorType::Blocking,err });
          Logger::Critical("%s\n", err.c_str());
      }
      test.close();
    }
  return rtn;
}
