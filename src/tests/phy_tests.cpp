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
      test.close();
    }
  return rtn;
}
