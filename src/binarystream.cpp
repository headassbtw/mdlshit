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


void BinaryReader::seek(int pos){
  Stream.seekg(pos);
  Stream.seekp(pos);
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
  return Stream.tellp();
}
void BinaryWriter::Write(Vector data){
  Stream.write((char*)&data.x, sizeof(float));
  Stream.write((char*)&data.y, sizeof(float));
  Stream.write((char*)&data.z, sizeof(float));
}


void BinaryWriter::seek(int pos){
  Stream.seekg(pos);
  Stream.seekp(pos);
}
