#include <fstream>
#include <stdio.h>
#include <cstdarg>
#include <filesystem>
#include <string>
#include <logger.hpp>
#ifndef __linux
#include <windows.h>
#define PATHCHAR "\\"
#else
#define PATHCHAR "/"
#endif

FILE* Logger::LogStream;

void Logger::Init(){
  std::string ass = "." PATHCHAR "log.log";
  LogStream = fopen(ass.c_str(), "w+");
  //LogStream = std::ofstream(ass.c_str(), std::ofstream::trunc);
}
void Logger::End(){
  
  //LogStream.close();
}


#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

enum LogType{
  Info,
  Notice,
  Error,
  Critical,
  Debug
};

void printt(const char* ass){
  fprintf(stdout, ass);
  fprintf(Logger::LogStream, ass);
}

void CommonLog(LogType col, va_list args, const char* msg...){
  #ifndef __linux
  HANDLE hConsoleOut;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
  #endif
  
  switch(col){
    case Debug:
    #ifdef __linux
    printf(GREEN);
    #else
    SetConsoleTextAttribute(hConsoleOut, FOREGROUND_GREEN);
    #endif
    printt("  [DEBUG]  | ");
    break;
    
    case Notice:
    #ifdef __linux
    printf(YELLOW);
    #else
    SetConsoleTextAttribute(hConsoleOut, 14);
    #endif
    printt("  [NOTICE] | ");
    break;

    case Error:
    #ifdef __linux
    printf(RED);
    #else
    SetConsoleTextAttribute(hConsoleOut, FOREGROUND_RED);
    #endif
    printt("  [ERROR]  | ");
    break;
    
    case Critical:
    #ifdef __linux
    printf(MAGENTA);
    #else
    SetConsoleTextAttribute(hConsoleOut, 13);
    #endif
    printt("[CRITICAL] | ");
    break;

    case Info:
    #ifdef __linux
    printf(WHITE);
    #else
    SetConsoleTextAttribute(hConsoleOut, 15);
    #endif
    printt("  [INFO]   | ");
    break;
  }
  vfprintf(stdout,msg, args);
  vfprintf(Logger::LogStream,msg, args);
  #ifdef __linux
  printf(RESET);
  #else
  SetConsoleTextAttribute(hConsoleOut, 15);
  #endif
}
void Logger::Notice(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Notice, args, msg);
  va_end(args);
}
void Logger::Info(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Info, args, msg);
  va_end(args);
}
void Logger::Error(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Error, args, msg);
  va_end(args);
}
void Logger::Critical(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Critical, args, msg);
  va_end(args);
}
void Logger::Debug(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Debug, args,msg);
  va_end(args);

}