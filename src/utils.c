#include "shell.h"

// 상위 디렉토리 접근 체크 함수 추가
int contains_parent_dir(const char *path) {
    char *tmp = strstr(path, "..");
    while (tmp != NULL) {
        // ".."가 경로의 컴포넌트인지 확인
        if ((tmp == path || tmp[-1] == '/') &&
            (tmp[2] == '\0' || tmp[2] == '/')) {
            return 1;
        }
        tmp = strstr(tmp + 1, "..");
    }
    return 0;
}

// 경로 정규화 함수 추가
char *normalize_path(const char *path) {
    char *normalized = malloc(strlen(path) + 1);
    if (!normalized) return NULL;

    const char *src = path;
    char *dst = normalized;

    // 연속된 '/' 제거 및 경로 정규화
    while (*src) {
        if (*src == '/') {
            *dst++ = *src++;
            while (*src == '/') src++;  // 연속된 '/' 건너뛰기
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    return normalized;
}

// 전체 경로 생성 함수
char *get_full_path(const char *current_dir, const char *path) {
    // 상위 디렉토리 접근 체크
    if (contains_parent_dir(path)) {
        printf("Error: Cannot access parent directory\n");
        return NULL;
    }

    size_t len = strlen(ROOT_PATH) + strlen(current_dir) + strlen(path) + 3;
    char *full_path = malloc(len);
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
            snprintf(full_path, len, "%s/%s", ROOT_PATH, path);
        } else {
            snprintf(full_path, len, "%s%s/%s", ROOT_PATH, current_dir, path);
        }
    }

    // 경로 정규화
    char *normalized = normalize_path(full_path);
    free(full_path);
    if (normalized == NULL) {
        return NULL;
    }

    // ROOT_PATH 벗어나는지 확인
    if (strncmp(normalized, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot access outside of root directory\n");
        free(normalized);
        return NULL;
    }

    return normalized;
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
