//#include <fstream>
#include <stdio.h>
#include <cstdarg>
#include <string>
#include <logger.hpp>
#ifdef WIN32
#include <windows.h>
#define PATHCHAR "\\"
#else
#define PATHCHAR "/"
#endif

FILE* Logger::LogStream;

void Logger::Init(){
  std::string ass = "." PATHCHAR "log.log";
  //LogStream = fopen(ass.c_str(), "w+");
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

std::vector<LogMsg*> LoggerMessages;

enum ConsoleColor{
  Reset,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White
};
enum LogType{
  Info,
  Notice,
  Error,
  Critical,
  Debug
};
void SetConsoleColor(ConsoleColor color){
  int WindowsCol;
  const char* UnixCol;
  switch(color){
    case Reset:    WindowsCol = 15; UnixCol = RESET;   break;
    case Black:    WindowsCol = 0;  UnixCol = BLACK;   break;
    case Red:      WindowsCol = 12; UnixCol = RED;     break;
    case Green:    WindowsCol = 10; UnixCol = GREEN;   break;
    case Yellow:   WindowsCol = 14; UnixCol = YELLOW;  break;
    case Blue:     WindowsCol = 9;  UnixCol = BLUE;    break;
    case Magenta:  WindowsCol = 13; UnixCol = MAGENTA; break;
    case Cyan:     WindowsCol = 11; UnixCol = CYAN;    break;
    case White:    WindowsCol = 15; UnixCol = WHITE;   break;
  }
  #ifdef WIN32
  HANDLE hConsoleOut;
  hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsoleOut, WindowsCol);
  #else
  fprintf(stdout, "%s",UnixCol);
  #endif
}


void printt(const char* ass){
  printf("%s",ass);
  //fprintf(Logger::LogStream, ass);
}

void CommonLog(LogType col, va_list args, const char* msg...){
  
  
  
  
  switch(col){
    case Debug:
    SetConsoleColor(Green);
    printt("  [DEBUG]  | ");
    break;
    
    case Notice:
    SetConsoleColor(Yellow);
    printt("  [NOTICE] | ");
    break;

    case Error:
    SetConsoleColor(Red);
    printt("  [ERROR]  | ");
    break;
    
    case Critical:
    SetConsoleColor(Magenta);
    printt("[CRITICAL] | ");
    break;

    case Info:
    SetConsoleColor(White);
    printt("  [INFO]   | ");
    break;
  }
  char msg_buf[2048];
  snprintf(msg_buf,2048, msg, args);
  printf("%s",msg_buf);

  LogMsg* m = new LogMsg();
  m->msg = std::string(msg_buf);
  m->type = col;
  LoggerMessages.push_back(m);

  //vfprintf(Logger::LogStream,msg, args);
  SetConsoleColor(Reset);
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
