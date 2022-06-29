#include <cstring>
#include <blacklist.hpp>
#include <logger.hpp>
#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#else
#include <unistd.h>
#endif

bool isOnBlacklist(char* user){
  return (
    strcmp(user, "Imply") == 0 ||
    //strcmp(user, "headass") == 0 ||
    strcmp(user, "Blessed Seal") == 0 ||
    strcmp(user, "Blessedseal") == 0 ||
    strcmp(user, "PROTOGENyeet") == 0 ||
    strcmp(user, "Dallas Regional Airport") == 0 ||
    strcmp(user, "nebustellar") == 0
  );
}

bool CheckBlacklist(char*& error_out)
{
  #ifdef WIN32
  //IMAGINE PLAYING LEAGUE
  HKEY hregkey;
  auto subkey = "SOFTWARE\\WOW6432Node\\Riot Games, Inc\\League of Legends";
  long retval = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_WRITE, &hregkey);
  LONG result = RegQueryValueExA(hregkey, "Location", NULL, NULL, NULL, NULL);
      
  if (result == ERROR_SUCCESS) {
    //MessageBox(NULL, "The application failed to start.\nError code: 0xF1D", "mdlshit", MB_OK);
    error_out = "0xF1D";
    return true;
  }
  else if (result == ERROR_FILE_NOT_FOUND) {
    Logger::Debug("ay\n");
  }
  else{
    Logger::Debug("ay 2\n");
  }
  //username blackist lol
  char * user = getenv("username");
  if(isOnBlacklist(user))
  {
    error_out = "0xA2F";
    return true;
  }
#else
  //username blacklsit, but linux
  char buf[100];
  getlogin_r(buf, 100);
  if(isOnBlacklist(buf))
  {
    error_out = "0xA2F";
    return true;
  }
#endif
  return false;
}