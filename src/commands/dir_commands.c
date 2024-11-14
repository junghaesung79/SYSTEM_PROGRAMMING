#include "shell.h"

// 디렉토리를 이동하는 함수
void cd_command(char *path, char *current_dir) {
    char new_path[MAX_CMD_SIZE];

    if (strcmp(path, ".") == 0) {  // . 인 경우 현재 디렉토리 유지
        return;
    } else if (strcmp(path, "..") == 0) {  // .. 인 경우 부모 디렉토리로 이동
        if (strcmp(current_dir, "/") == 0) {
            printf("Already at root directory\n");
            return;
        }
        char temp_path[MAX_CMD_SIZE];
        strcpy(temp_path, current_dir);
        char *parent = dirname(temp_path);
        snprintf(new_path, sizeof(new_path), "%s", parent);
    } else if (path[0] != '/') {  // 상대경로 이동, 절대경로보다 사용빈도가 높다고 판단하여 앞에 배치
        if (strcmp(current_dir, "/") == 0) {
            snprintf(new_path, sizeof(new_path), "/%s", path);
        } else {
            snprintf(new_path, sizeof(new_path), "%s/%s", current_dir, path);
        }
    } else {  // 절대경로 이동
        strncpy(new_path, path, sizeof(new_path) - 1);
        new_path[sizeof(new_path) - 1] = '\0';
    }

    // 중복 슬래시 제거
    char *src = new_path, *dst = new_path;
    while (*src) {
        if (*src == '/' && *(src + 1) == '/') {
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    // ROOT_PATH와 new_path를 결합하여 전체 경로 생성
    char full_path[MAX_CMD_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_PATH, new_path);

    struct stat st;
    if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {           // 경로가 존재 + 디렉토리인지 확인
        if (strncmp(full_path, ROOT_PATH, strlen(ROOT_PATH)) == 0) {  // 새 경로가 ROOT_PATH 내에 있는지 확인
            strcpy(current_dir, new_path);
            printf("Changed directory to: %s\n", current_dir);
        } else {  // ROOT_PATH 외부로의 이동 시도 차단
            printf("Cannot move outside of %s\n", ROOT_PATH);
        }
    } else {  // 경로가 존재하지 않거나 디렉토리가 아닌 경우
        printf("cd: No such directory\n");
    }
}

// 새 디렉토리를 생성하는 함수
void mkdir_command(char *path, const char *current_dir) {
    char *full_path = get_full_path(current_dir, path);
    if (mkdir(full_path, 0755) == 0) {  // 0755 권한으로 디렉토리 생성 시도, 리눅스 시스템에서 디렉토리에 대한 표준적인 권한 설정임
        printf("Directory created: %s\n", path);
    } else {
        if (errno == EEXIST) {  // 이미 존재하는 경우
            printf("Error: Directory already exists: %s\n", path);
        } else {  // 기타 오류
            perror("mkdir failed");
        }
    }
}

// 빈 디렉토리를 삭제하는 함수
void rmdir_command(char *path, const char *current_dir) {
    char *full_path = get_full_path(current_dir, path);
    if (rmdir(full_path) == 0) {  // 디렉토리 삭제 시도
        printf("Directory removed: %s\n", path);
    } else {
        if (errno == ENOTEMPTY) {  // 디렉토리가 비어있지 않은 경우
            printf("Error: Directory is not empty: %s\n", path);
        } else if (errno == ENOENT) {  // 디렉토리가 존재하지 않는 경우
            printf("Error: Directory does not exist: %s\n", path);
        } else {  // 기타 오류
            perror("rmdir failed");
        }
    }
}
// 디렉토리의 내용을 나열하는 함수
void ls_command(const char *current_dir) {
    char full_path[MAX_CMD_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT_PATH, current_dir);

    DIR *dir = opendir(full_path);
    if (dir == NULL) {
        perror("Cannot open directory");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    char file_path[MAX_CMD_SIZE];
    char link_path[MAX_CMD_SIZE];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {  // 숨김 파일 제외
            snprintf(file_path, sizeof(file_path), "%s/%s", full_path, entry->d_name);

            if (lstat(file_path, &file_stat) == 0) {
                // 파일 종류 및 접근 권한
                char permissions[11] = {0};
                mode_to_string(file_stat.st_mode, permissions);

                // 하드 링크 수
                int link_count = file_stat.st_nlink;

                // 소유자 및 그룹 (숫자로 표시)
                int uid = file_stat.st_uid;
                int gid = file_stat.st_gid;

                // 파일 크기
                long long file_size = file_stat.st_size;

                // 수정 시간
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));

                // 파일명 (심볼릭 링크의 경우 링크 정보 포함)
                char file_name[MAX_CMD_SIZE];
                if (S_ISLNK(file_stat.st_mode)) {
                    int link_size = readlink(file_path, link_path, sizeof(link_path) - 1);
                    if (link_size != -1) {
                        link_path[link_size] = '\0';
                        snprintf(file_name, sizeof(file_name), "%s -> %s", entry->d_name, link_path);
                    } else {
                        strcpy(file_name, entry->d_name);
                    }
                } else {
                    strcpy(file_name, entry->d_name);
                }

                // 출력
                printf("%s %2d %5d %5d %8lld %s %s\n", permissions, link_count, uid, gid, file_size, time_str, file_name);
            }
        }
    }

    closedir(dir);
}
