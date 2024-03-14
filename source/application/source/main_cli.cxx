#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

int main(const int argc, const char** argv)
{
    char currentPath[FILENAME_MAX];

    if (!GetCurrentDir(currentPath, sizeof(currentPath)))
    {
        std::cerr<<"Failed to get current directory"<<std::endl;
        return -1;
    }


    std::cout << "Current bin direcoty: " << currentPath << std::endl;
    std::cout << "Current application: " << argv[0] << std::endl;

    return 0;
}