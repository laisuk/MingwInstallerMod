#include "home.hpp"

#ifdef _WIN32
#include <shlobj.h>
#include <algorithm>
#include <stdexcept>


std::string home()
{
    char p[MAX_PATH];
    LPSTR path = p;
    if(SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path))){
        std::string res {path};
        res+="/";
        std::replace(res.begin(), res.end(), '\\', '/');
        return res;
    }
    throw std::runtime_error("cant get home path");
}

#else

std::string home()
{
    //Hardcoded for debug on Linux
    return "/home/vuniverse";
}

#endif
