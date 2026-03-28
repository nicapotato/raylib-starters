#include "platform_cwd.h"
#include <limits.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <stdlib.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <stdint.h>
#endif
#endif

int set_cwd_to_executable_dir(void) {
    char path[4096];
#if !defined(_WIN32)
    char resolved[4096];
#endif
    char *sep;

#if defined(_WIN32)
    DWORD n = GetModuleFileNameA(NULL, path, (DWORD)sizeof(path));
    if (n == 0 || n >= sizeof(path)) return -1;
    path[n] = '\0';
    sep = strrchr(path, '\\');
    if (!sep) sep = strrchr(path, '/');
#else
#if defined(__APPLE__)
    uint32_t bufsize = (uint32_t)sizeof(path);
    if (_NSGetExecutablePath(path, &bufsize) != 0) return -1;
#else
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len < 0) return -1;
    path[len] = '\0';
#endif
    if (realpath(path, resolved) != NULL) {
        strncpy(path, resolved, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
    }
    sep = strrchr(path, '/');
#endif
    if (!sep) return -1;
    *sep = '\0';
#if defined(_WIN32)
    if (!SetCurrentDirectoryA(path)) return -1;
#else
    if (chdir(path) != 0) return -1;
#endif
    return 0;
}
