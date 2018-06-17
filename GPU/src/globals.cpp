#include "globals.h"
#include <unistd.h>
#include <iostream>


std::string setKernelRootDir() {
    char path[1000];
    getcwd(path, sizeof(path));
    return std::string(path) + "/../src/";
}
std::string KERNEL_ROOT_DIR = setKernelRootDir();
