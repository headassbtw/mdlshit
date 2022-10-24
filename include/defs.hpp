#pragma once
#include <string>

enum ErrorType{
  Blocking,
  Critical,
  Warning,
  Neutral,
  Success
};

struct Error{
  ErrorType type;
  std::string message;
};