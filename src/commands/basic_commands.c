#include "shell.h"

// 구현된 함수 설명하는 함수
void help_command() {
    printf("\nAvailable commands:\n");
    printf("\n[File and Directory Commands]\n");
    printf("  ls [path]             : List contents of a directory\n");
    printf("  cd <path>             : Change current directory\n");
    printf("  mkdir <path>          : Create a new directory\n");
    printf("  rmdir <path>          : Remove an empty directory\n");
    printf("  rename <old> <new>    : Rename a file or directory\n");
    printf("  rm <file>             : Delete a file\n");
    printf("  cp <source> <dest>    : Copy a file\n");

    printf("\n[File Permission and Link Commands]\n");
    printf("  chmod <perm> <file>   : Change file permissions\n");
    printf("  ln [-s] <orig> <new>  : Create a hard link or symbolic link (-s for symbolic)\n");

    printf("\n[File Content Commands]\n");
    printf("  cat <file>            : Display contents of a file\n");

    printf("\n[Process Commands]\n");
    printf("  ps                    : Display process list\n");
    printf("  kill [-signal] <pid>  : Send signal to process (default: SIGTERM)\n");
    printf("  ./program [args]      : Execute a program\n");
    printf("  program [args] &      : Execute a program in background\n");

    printf("\n[Other Commands]\n");
    printf("  help                  : Show this help message\n");
    printf("  quit                  : Exit from the shell\n");

    printf("\nNote: All operations are restricted to %s directory\n", ROOT_PATH);
    printf("\n");
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
