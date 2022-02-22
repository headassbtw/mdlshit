#pragma once
#include <cstdio>
#include <structs.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;
class BinaryReader{
  const char* _filename;
  public:
  std::fstream Stream;
    BinaryReader(const char* filename);
    BinaryReader(const BinaryReader&);
    ~BinaryReader();
    int size;
    void read(char* data, int size);
    void seek(int pos);
    
    void Read(char** data);
    //void Read(const char** data, int size);
    void Read(char** data[], int size);
    //void Read(char** data, int size);
    void Read(int* data);
    void Read(int data[], int size);
    void Read(float* data);
    //void Read(long* data);
    int Position();
    void Read(Vector* data);
    void Read(byte* data);
    void Read(byte* data[], int size);

};
class BinaryWriter{
  const char* _filename;
  public:
  std::fstream Stream;
    BinaryWriter(const char* filename);
    BinaryWriter(const BinaryWriter&);
    ~BinaryWriter();
    int size;
    void write(const char* data, int size);
    void seek(int pos);
    void Write(char* data);
    void Write(char* data[]);
    void Write(int data);
    void Write(int data[]);
    void Write(float data);
    //void Write(long data);
    int Position();
    void Write(Vector data);
    void Write(byte data);
    void Write(byte data[], int size);
};