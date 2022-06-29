#pragma once
#include <mutex>
#include <string>
struct UI_TASK{
  private:
  std::mutex show_mutex;
  std::mutex name_mutex;
  std::mutex prog_mutex;
  bool show;
  std::string _name;
  float prog;
  public:
  bool Show();
  std::string Name();
  float Progress();
  void Begin(std::string name);
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