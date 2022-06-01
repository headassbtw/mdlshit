#include "rendering/progress.hpp"
#include <rendering/progress.hpp>
#include <thread>

UI_PROGRESS UI::Progress;


bool UI_TASK::Show(){
  std::lock_guard<std::mutex> guard(show_mutex);
  return show;
}
string UI_TASK::Name(){
  std::lock_guard<std::mutex> guard(name_mutex);
  return _name;
}
float UI_TASK::Progress(){
  std::lock_guard<std::mutex> guard(prog_mutex);
  return prog;
}
void UI_TASK::Begin(string name){
  std::lock_guard<std::mutex> guard(show_mutex);
  show = true;
  std::lock_guard<std::mutex> guardname(name_mutex);
  _name = name;
}
void UI_TASK::Update(float fraction){
  //std::this_thread::sleep_for(std::chrono::milliseconds(1));
  std::lock_guard<std::mutex> guard(prog_mutex);
  prog = fraction;
}
void UI_TASK::End(){
  std::lock_guard<std::mutex> guard(show_mutex);
  show = false;
}