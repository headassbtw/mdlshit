#pragma once
struct Logger{
  static void Notice(const char* msg...);
  static void Info(const char* msg...);
  static void Error(const char* msg...);
  static void Critical(const char* msg...);
  static void Debug(const char* msg...);
};