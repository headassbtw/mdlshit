#include <defs.hpp>
#include <cstring>
#include <tests.hpp>
#include <fstream>

vector<Error> Tests::TestVVD(string path){
  vector<Error> rtn;
  std::ifstream test(path); 
  if (!test)
  {
    rtn.push_back({ErrorType::Blocking,string("File does not exist")});
  }
  else{
    char magic[4];
    test.read(magic, 4);

    if(strcmp(magic,"IDSV") == 0){
      rtn.push_back({ErrorType::Success,string("Valid VVD File")});
    }
    else{
      rtn.push_back({ErrorType::Warning,string("Unknown file type")});
    }
    test.close();
  }
  return rtn;
}