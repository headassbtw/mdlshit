#pragma once
#include <defs.hpp>
#include <vector>
using namespace std;
namespace Tests{
  vector<Error> TestMDL(string path);
  vector<Error> TestVVD(string path);
}