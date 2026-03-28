#ifndef PLATFORM_CWD_H
#define PLATFORM_CWD_H

/* Set process cwd to the executable's directory (so relative paths like src/main.lua work). */
int set_cwd_to_executable_dir(void);

#endif
