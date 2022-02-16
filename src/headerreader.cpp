#include <binarystream.hpp>
#include <structs.hpp>
using namespace std;

v49_Header ReaddHeader(const char* file){
  BinaryReader InStream = BinaryReader(file);
  v49_Header* Header = new v49_Header();
  InStream.Read(&Header->id);
  

}