#pragma once
#include <string>
using namespace std;
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

enum ErrorType{
  Blocking,
  Critical,
  Warning,
  Neutral,
  Success
};

struct Error{
  ErrorType type;
  string message;
};