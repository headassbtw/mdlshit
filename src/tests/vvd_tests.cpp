#include <defs.hpp>
#include <cstring>
#include <tests.hpp>
#include <fstream>

std::vector<Error> Tests::TestVVD(std::string path){
  std::vector<Error> rtn;
  std::ifstream test(path); 
  if (!test)
  {
    rtn.push_back({ErrorType::Blocking,std::string("File does not exist")});
  }
  else{
    char magic[4];
    test.read(magic, 4);

    if(strcmp(magic,"IDSV") == 0){
      rtn.push_back({ErrorType::Success,std::string("Valid VVD File")});
    }
    else{
      rtn.push_back({ErrorType::Warning,std::string("Unknown file type")});
    }
    test.close();
  }
  return rtn;
}