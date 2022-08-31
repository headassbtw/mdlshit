#pragma once
#include <defs.hpp>
#include <vector>
namespace Tests{
  std::vector<Error> TestMDL(std::string path);
  std::vector<Error> TestVVD(std::string path);
  std::vector<Error> TestVTX(std::string path);
  std::vector<Error> TestPHY(std::string path);
}