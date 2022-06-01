#pragma once
#include <mutex>
#include <string>
using namespace std;
struct UI_TASK{
  private:
  mutex show_mutex;
  mutex name_mutex;
  mutex prog_mutex;
  bool show;
  string _name;
  float prog;
  public:
  bool Show();
  string Name();
  float Progress();
  void Begin(string name);
  void Update(float fraction);
  void End();
};
struct UI_PROGRESS{
  UI_TASK SubTask;
  UI_TASK MainTask;
};
namespace UI{
  extern UI_PROGRESS Progress;
}