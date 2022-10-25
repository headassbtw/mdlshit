//#include <fstream>
#include <fstream>
#include <sstream>
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

FILE* LogStream;
bool LogForceEnabled = false; //these should be init'd already, but just being safe here
bool LogEnabled = false;

void Logger::Init(){
  std::string ass = "." PATHCHAR "log.log";
  if(!LogEnabled) LogStream = fopen(ass.c_str(),"w");
  Info("Log file opened\n");
}

void Logger::End(){
  if(LogEnabled) fclose(LogStream);
  Info("Log file closed\n");
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
  if(LogEnabled) fprintf(LogStream, "%s", ass);
}
void LoggerPrefix(LogType col){
  switch(col){
    case Debug:    SetConsoleColor(Green);   printt("  [DEBUG]  | "); break;

    case Notice:   SetConsoleColor(Yellow);  printt("  [NOTICE] | "); break;

    case Error:    SetConsoleColor(Red);     printt("  [ERROR]  | "); break;

    case Critical: SetConsoleColor(Magenta); printt("[CRITICAL] | "); break;

    case Info:     SetConsoleColor(White);   printt("  [INFO]   | "); break;
  }
}

void CommonLog(LogType col, const char* msg...){
  switch(col){
    case Debug:    SetConsoleColor(Green);   printt("  [DEBUG]  | "); break;
    
    case Notice:   SetConsoleColor(Yellow);  printt("  [NOTICE] | "); break;

    case Error:    SetConsoleColor(Red);     printt("  [ERROR]  | "); break;
    
    case Critical: SetConsoleColor(Magenta); printt("[CRITICAL] | "); break;

    case Info:     SetConsoleColor(White);   printt("  [INFO]   | "); break;
  }
  char msg_buf[2048];
  
  va_list args;
  va_start(args, msg);
  vfprintf(stdout,msg,args);
  va_end(args);
  printf("%s",msg_buf);

  

  //LogStream.write(msg_buf, 2048);

  SetConsoleColor(Reset);
}

void AddToInAppLogger(char* message, LogType type){
  LogMsg* m = new LogMsg();
  m->msg = std::string(message);
  m->type = type;
  LoggerMessages.push_back(m);
}

void help(const char* p, va_list args){
  printf(p,args);
}


void CommonLog(LogType type, const char *fmt, va_list args1){
  LogType _type = (LogType)type;
  LoggerPrefix(_type);
  va_list args2;
  va_copy(args2, args1);
  char ass[2048];
  vsprintf(ass,fmt,args2);
  AddToInAppLogger(ass, _type);
  va_end(args2);
  va_copy(args2,args1);
  vfprintf(stdout,fmt,args2);
  va_end(args2);
  if(LogEnabled){
    va_copy(args2,args1);
    vfprintf(LogStream,fmt,args2);
    va_end(args2);
  }
  SetConsoleColor(Reset);
}

void Logger::Notice(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Notice, msg,args);
  va_end(args);
  
}
void Logger::Info(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Info, msg,args);
  va_end(args);
}
void Logger::Error(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Error, msg,args);
  va_end(args);
}
void Logger::Critical(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Critical, msg,args);
  va_end(args);
}
void Logger::Debug(const char* msg...){
  va_list args;
  va_start(args, msg);
  CommonLog(LogType::Debug, msg,args);
  va_end(args);
}
