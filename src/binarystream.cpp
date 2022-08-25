#include "structs.hpp"
#include <binarystream.hpp>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <logger.hpp>
using namespace std;

BinaryReader::BinaryReader(const char* filename){
  _filename = filename;
  Stream = fstream(_filename, ios::binary | ios::in);
  struct stat results;
  if (stat(filename, &results) == 0){
    size = results.st_size;
  }
  else{
    Logger::Critical("Could not get stats of input file!\n");
  }

}
BinaryReader::~BinaryReader(){
  Stream.close();
  Logger::Info("BinaryReader for file \"%s\" closed\n", _filename);
}
void BinaryReader::Read(int* data){
  Stream.read((char*)data, sizeof(int));
  //printf("read an int\n");
}
void BinaryReader::Read(int data[], int size){
  for(int i = 0; i < size;i++){
    Stream.read((char*)data[i], sizeof(int));
  }
  
  //printf("read an int array\n");
}
void BinaryReader::Read(char** data[], int size){
  Stream.read((char*)*data, size);
  //printf("read an array of char\n");
}
void BinaryReader::read(char* data, int size){
  Stream.read(data, size);
  //printf("read a char\n");
}

void BinaryReader::Read(char** data){
  Stream.read(*data, sizeof(char*));
  //printf("read a char*\n");
}
void BinaryReader::Read(byte* data){
  
  Stream.read((char*) data, 1);
  
  
}
void BinaryReader::Read(byte* data[] , int size){
  Stream.read((char*) data, size);
  //printf("read a byte\n");
}

void BinaryReader::Read(float* data){
  Stream.read((char*)data, 4);
  //printf("read a float\n");
}
int BinaryReader::Position(){
  return Stream.tellg();
}
void BinaryReader::Read(Vector* data){
  Stream.read((char*)&data->x, sizeof(float));
  Stream.read((char*)&data->y, sizeof(float));
  Stream.read((char*)&data->z, sizeof(float));
  //printf("read a Vector\n");
}
void BinaryReader::Read(Vector3* data) {
    Stream.read((char*)&data->x, sizeof(float));
    Stream.read((char*)&data->y, sizeof(float));
    Stream.read((char*)&data->z, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryReader::Read(RadianEuler* data) {
    Stream.read((char*)&data->x, sizeof(float));
    Stream.read((char*)&data->y, sizeof(float));
    Stream.read((char*)&data->z, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryReader::Read(matrix3x4_t* data) {
    Stream.read((char*)&data->c0r0, sizeof(float));
    Stream.read((char*)&data->c1r0, sizeof(float));
    Stream.read((char*)&data->c2r0, sizeof(float));
    Stream.read((char*)&data->c3r0, sizeof(float));

    Stream.read((char*)&data->c0r1, sizeof(float));
    Stream.read((char*)&data->c1r1, sizeof(float));
    Stream.read((char*)&data->c2r1, sizeof(float));
    Stream.read((char*)&data->c3r1, sizeof(float));

    Stream.read((char*)&data->c0r2, sizeof(float));
    Stream.read((char*)&data->c1r2, sizeof(float));
    Stream.read((char*)&data->c2r2, sizeof(float));
    Stream.read((char*)&data->c3r2, sizeof(float));
}

void BinaryReader::Read(Quaternion* data) {
    Stream.read((char*)&data->one, sizeof(float));
    Stream.read((char*)&data->i, sizeof(float));
    Stream.read((char*)&data->j, sizeof(float));
    Stream.read((char*)&data->k, sizeof(float));
    //printf("read a Vector\n");
}


void BinaryReader::seek(int pos){
  Stream.seekg(pos);
  if(Stream.tellg() != pos){
    Logger::Critical("BinaryReader failed to seek the read cursor!\n");
    abort();
  }
  Stream.seekp(pos);
  if(Stream.tellp() != pos){
    Logger::Critical("BinaryReader failed to seek the write cursor!\n");
  }
}

BinaryWriter::BinaryWriter(const char* filename){
  _filename = filename;
  Stream = fstream(_filename, ios::binary | ios::out);
  
  struct stat results;
  if (stat(filename, &results) == 0){
    size = results.st_size;
  }
  else{
    fprintf(stderr,"Could not get stats of input file!\n");
  }

}
BinaryWriter::~BinaryWriter(){
  Stream.close();
  Logger::Info("BinaryWriter for file \"%s\" closed\n", _filename);
}
void BinaryWriter::write(const char* data, int size){
  Stream.write(data, size);
}
void BinaryWriter::Write(char* data){
  Stream.write((char*)&data, sizeof(char*));
}
void BinaryWriter::Write(byte data){
  Stream.write((char*)&data, sizeof(byte));
}
void BinaryWriter::Write(byte data[], int size){
  Stream.write((char*)&data, sizeof(byte)*size);
}
void BinaryWriter::Write(char* data[]){
  
  Stream.write((char*)&data, sizeof(data)/sizeof(data[0]));
}
void BinaryWriter::Write(int data){
  Stream.write((char*)&data, sizeof(int));
}
void BinaryWriter::Write(int data[]){
  Stream.write((char*)&data, sizeof(int) * sizeof(data)/sizeof(data[0]));
}
void BinaryWriter::Write(float data){
  Stream.write((char*)&data, sizeof(float));
}
int BinaryWriter::Position(){
  if(Stream.tellp() <= -1){
    Logger::Critical("BinaryWriter's seek position was invalid! \n");
    abort();
  }
  return Stream.tellp();
}
void BinaryWriter::Write(Vector data){
  Stream.write((char*)&data.x, sizeof(float));
  Stream.write((char*)&data.y, sizeof(float));
  Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(Vector3 data) {
    Stream.write((char*)&data.x, sizeof(float));
    Stream.write((char*)&data.y, sizeof(float));
    Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(RadianEuler data) {
    Stream.write((char*)&data.x, sizeof(float));
    Stream.write((char*)&data.y, sizeof(float));
    Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(matrix3x4_t data) {
    Stream.write((char*)&data.c0r0, sizeof(float));
    Stream.write((char*)&data.c1r0, sizeof(float));
    Stream.write((char*)&data.c2r0, sizeof(float));
    Stream.write((char*)&data.c3r0, sizeof(float));

    Stream.write((char*)&data.c0r1, sizeof(float));
    Stream.write((char*)&data.c1r1, sizeof(float));
    Stream.write((char*)&data.c2r1, sizeof(float));
    Stream.write((char*)&data.c3r1, sizeof(float));

    Stream.write((char*)&data.c0r2, sizeof(float));
    Stream.write((char*)&data.c1r2, sizeof(float));
    Stream.write((char*)&data.c2r2, sizeof(float));
    Stream.write((char*)&data.c3r2, sizeof(float));
}
void BinaryWriter::Write(Quaternion data) {
    Stream.write((char*)&data.one, sizeof(float));
    Stream.write((char*)&data.i, sizeof(float));
    Stream.write((char*)&data.j, sizeof(float));
    Stream.write((char*)&data.k, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryWriter::seek(int pos){
  if(Stream.fail()){
    Stream.close();
    Stream.open(_filename);
    Logger::Notice("Stream died, reopening file \"%s\"\n",_filename);
  }

  Stream.seekp(pos, std::ios::beg);
  if(Stream.tellp() != pos){
    Logger::Critical("BinaryWriter failed to seek the write cursor to %d!\n", pos);
    abort();
  }
  Stream.seekg(pos, std::ios::beg);
  if(Stream.tellg() != pos){
    Logger::Critical("BinaryWriter failed to seek the read cursor to %d!\n", pos);
  }
}
