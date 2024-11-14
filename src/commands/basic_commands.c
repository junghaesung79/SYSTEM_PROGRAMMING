#include "shell.h"

// 구현된 함수 설명하는 함수
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

// 파일 또는 디렉토리 이름 변경 함수
void rename_command(char *source, char *target, const char *current_dir) {
    char *full_source = get_full_path(current_dir, source);
    char *full_target = get_full_path(current_dir, target);

    printf("Attempting to rename: %s to %s\n", full_source, full_target);

    struct stat st;

    // 소스 파일/디렉토리 존재 여부 확인
    if (stat(full_source, &st) != 0) {
        printf("Error: Source does not exist: %s\n", source);
        return;
    }

    // 대상 파일/디렉토리가 이미 존재하는지 확인
    if (stat(full_target, &st) == 0) {
        printf("Error: Target already exists: %s\n", target);
        return;
    }

    // 파일 이름 변경 시도
    if (rename(full_source, full_target) == 0) {
        printf("Successfully renamed: %s -> %s\n", source, target);
    } else {
        printf("Failed to rename: %s to %s\n", source, target);
        printf("Error details: %s\n", strerror(errno));
    }
}
