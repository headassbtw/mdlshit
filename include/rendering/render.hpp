#pragma once
namespace UI{
  int Run();
  void SetupAboutWindow();
  void RenderAboutWindow(int grunt, float grunt_x, float grunt_y);
  void SetupReadMdlWindow();
  void SetReadMdlFile(const char* path);
  void RenderReadMdlWindow(int x, int y);
}