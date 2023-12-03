#include <cstring>
#include <logger.hpp>
#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#else
#include <unistd.h>
#endif

bool isOnBlacklist(char* user){
  return (
    strcmp(user, "Dallas Regional Airport") == 0 ||
    strcmp(user, "Imply") == 0 ||
    strcmp(user, "Blessed Seal") == 0 ||
    strcmp(user, "Blessedseal") == 0 ||
    strcmp(user, "PROTOGENyeet") == 0 ||
    strcmp(user, "nebustellar") == 0
  );
}
bool CheckRegKey(const char* key){
  #ifdef WIN32
  HKEY hregkey;
  auto subkey = key;
  long retval = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_WRITE, &hregkey);
  LONG result = RegQueryValueExA(hregkey, "Location", NULL, NULL, NULL, NULL);
      
  if (result == ERROR_SUCCESS) {
    return true;
  }
  else{
    return false;
  }
  #else
  return false;
  #endif
}