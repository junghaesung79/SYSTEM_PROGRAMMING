#include "shell.h"

void help_command() {
    printf("Available commands:\n");
    printf("  help                  : Show help messages\n");
    printf("  cd <path>             : Change current directory\n");
    printf("  mkdir <path>          : Create a new directory\n");
    printf("  rmdir <path>          : Remove an empty directory\n");
    printf("  rename <old> <new>    : Rename a file or directory\n");
    printf("  ls [path]             : List contents of a directory\n");
    printf("  ln [-s] <original> <new> : Create a hard link or symbolic link\n");
    printf("  rm <file>             : Delete a file\n");
    printf("  chmod <perm> <file>   : Change file permissions\n");
    printf("  cat <file>            : Display contents of a file\n");
    printf("  cp <source> <dest>    : Copy a file\n");
    printf("  quit                  : Escape from the shell\n");
}
