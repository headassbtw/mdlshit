#pragma once
#include <fstream>
#include <string>
#include <vector>

struct Logger{
  static void Init();
  static void Notice(const char* msg...);
  static void Info(const char* msg...);
  static void Error(const char* msg...);
  static void Critical(const char* msg...);
  static void Debug(const char* msg...);
  static void End();
};
struct LogMsg{
  std::string msg;
  int type;
};
extern FILE* LogStream;
extern std::vector<LogMsg*> LoggerMessages;
extern bool LogForceEnabled;
extern bool LogEnabled;
extern bool LogDisabled;
