#include <cstring>
#include <blacklist.hpp>
#include <logger.hpp>
#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#endif

bool isOnBlacklist(const char* user){
    return (
        strcmp(user, "nebustellar") == 0
    );
}

void Demolish_Weebs(){
    
#ifdef WIN32

    //IMAGINE PLAYING LEAGUE
    HKEY hregkey;
    auto subkey = "SOFTWARE\\WOW6432Node\\Riot Games, Inc\\League of Legends";
    long retval = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_WRITE, &hregkey);
    LONG result = RegQueryValueExA(hregkey, "Location", NULL, NULL, NULL, NULL);
        
    if (result == ERROR_SUCCESS) {
        MessageBox(NULL, "The application failed to start.\nError code: 0xF1D", "mdlshit", MB_OK);
        exit(0);
    }
    else if (result == ERROR_FILE_NOT_FOUND) {
        Logger::Debug("ay\n");
    }
    else{
        Logger::Debug("ay 2\n");
    }
    //username blackist lol
    char * user = getenv("username");
    if(
        isOnBlacklist(user)
        ){
            MessageBox(NULL, "The application failed to start.\nError code: 0xA2F", "mdlshit", MB_OK);
            exit(0);
        }
#endif
}