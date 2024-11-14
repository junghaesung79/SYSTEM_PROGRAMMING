#include "shell.h"

// 전체 경로 생성 함수
char *get_full_path(const char *current_dir, const char *path) {
    size_t len = strlen(ROOT_PATH) + strlen(current_dir) + strlen(path) + 3;
    char *full_path = malloc(len);  // 메모리 동적 할당. 이 함수 여러 번 호출 할 때 경로 정확히 붙이기 위함
    if (full_path == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // 절대 경로인 경우 (/로 시작하는 경우)
    if (path[0] == '/') {
        snprintf(full_path, len, "%s%s", ROOT_PATH, path);
    } else {
        // 상대 경로인 경우
        if (strcmp(current_dir, "/") == 0) {
            // 현재 디렉토리가 루트일 때
            snprintf(full_path, len, "%s/%s", ROOT_PATH, path);
        } else {
            // 현재 디렉토리가 루트가 아닐 때
            snprintf(full_path, len, "%s%s/%s", ROOT_PATH, current_dir, path);
        }
    }

    return full_path;
}

// /tmp/test 디렉토리가 있는 지 확인하고 생성 함수
void check_directory() {
    struct stat st = {0};
    if (stat(ROOT_PATH, &st) == -1) {
        if (mkdir(ROOT_PATH, 0755) == -1) {
            perror("Failed to create /tmp/test directory");
            exit(1);
        }
    }

    if (access(ROOT_PATH, R_OK | W_OK | X_OK) == -1) {
        fprintf(stderr, "Error: No permission to access /tmp/test directory.\n");
        exit(1);
    }
}

// 파일 종류를 문자로 반환하는 함수
char get_file_type(mode_t mode) {
    if (S_ISDIR(mode))
        return 'd';
    else if (S_ISREG(mode))
        return '-';
    else if (S_ISLNK(mode))
        return 'l';
    else if (S_ISCHR(mode))
        return 'c';
    else if (S_ISBLK(mode))
        return 'b';
    else if (S_ISFIFO(mode))
        return 'p';
    else if (S_ISSOCK(mode))
        return 's';
    else
        return '?';
}

// 접근 권한을 문자열로 변환하는 함수
void mode_to_string(mode_t mode, char *str) {
    strcpy(str, "---------");
    if (mode & S_IRUSR) str[0] = 'r';
    if (mode & S_IWUSR) str[1] = 'w';
    if (mode & S_IXUSR) str[2] = 'x';
    if (mode & S_IRGRP) str[3] = 'r';
    if (mode & S_IWGRP) str[4] = 'w';
    if (mode & S_IXGRP) str[5] = 'x';
    if (mode & S_IROTH) str[6] = 'r';
    if (mode & S_IWOTH) str[7] = 'w';
    if (mode & S_IXOTH) str[8] = 'x';
}
