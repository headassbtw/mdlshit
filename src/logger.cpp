#include <stdio.h>
#include <cstdarg>
#include <logger.hpp>
#ifndef __linux
#include <windows.h>
#endif

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
    printf("  [DEBUG]  | ");
    break;
    
    case Notice:
    #ifdef __linux
    printf(YELLOW);
    #else
    SetConsoleTextAttribute(hConsoleOut, 14);
    #endif
    printf("  [NOTICE] | ");
    break;

    case Error:
    #ifdef __linux
    printf(RED);
    #else
    SetConsoleTextAttribute(hConsoleOut, FOREGROUND_RED);
    #endif
    printf("  [ERROR]  | ");
    break;
    
    case Critical:
    #ifdef __linux
    printf(MAGENTA);
    #else
    SetConsoleTextAttribute(hConsoleOut, 13);
    #endif
    printf("[CRITICAL] | ");
    break;

    case Info:
    #ifdef __linux
    printf(WHITE);
    #else
    SetConsoleTextAttribute(hConsoleOut, 15);
    #endif
    printf("  [INFO]   | ");
    break;
  }
  vfprintf(stdout,msg, args);
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