#include "shell.h"

// 파일 내용 출력 함수
void cat_command(char *filename, const char *current_dir) {
    char *full_path = get_full_path(current_dir, filename);
    if (full_path == NULL) {
        printf("Error: Invalid path\n");
        return;
    }

    // ROOT_PATH 제한 확인
    if (strncmp(full_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot operate outside of %s\n", ROOT_PATH);
        free(full_path);
        return;
    }

    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        free(full_path);
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    if (ferror(file)) {
        perror("Error reading file");
    }

    fclose(file);
    free(full_path);
}

// 파일을 복사하는 함수
void cp_command(char *source_file, char *dest_file, const char *current_dir) {
    char *full_source_path = get_full_path(current_dir, source_file);
    char *full_dest_path = get_full_path(current_dir, dest_file);

    if (full_source_path == NULL || full_dest_path == NULL) {
        printf("Error: Invalid path\n");
        free(full_source_path);
        free(full_dest_path);
        return;
    }

    // ROOT_PATH 제한 확인
    if (strncmp(full_source_path, ROOT_PATH, strlen(ROOT_PATH)) != 0 ||
        strncmp(full_dest_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot operate outside of %s\n", ROOT_PATH);
        free(full_source_path);
        free(full_dest_path);
        return;
    }

    FILE *source = fopen(full_source_path, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        free(full_source_path);
        free(full_dest_path);
        return;
    }

    FILE *dest = fopen(full_dest_path, "wb");
    if (dest == NULL) {
        perror("Error creating destination file");
        fclose(source);
        free(full_source_path);
        free(full_dest_path);
        return;
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest) != bytes_read) {
            perror("Error writing to destination file");
            fclose(source);
            fclose(dest);
            free(full_source_path);
            free(full_dest_path);
            return;
        }
    }

    fclose(source);
    fclose(dest);

    // 파일 속성 복사
    struct stat st;
    if (stat(full_source_path, &st) == 0) {
        chmod(full_dest_path, st.st_mode);
    }

    printf("File copied successfully: %s -> %s\n", source_file, dest_file);

    free(full_source_path);
    free(full_dest_path);
}

// 파일 삭제 함수
void rm_command(char *file_to_remove, const char *current_dir) {
    char *file_path = get_full_path(current_dir, file_to_remove);

    // 파일 존재 여부 확인
    struct stat st;
    if (stat(file_path, &st) != 0) {
        printf("Error: File does not exist: %s\n", file_to_remove);
        return;
    }

    // 파일인지 확인 (디렉토리가 아닌지)
    if (!S_ISREG(st.st_mode)) {
        printf("Error: Not a regular file: %s\n", file_to_remove);
        return;
    }

    // 파일 삭제
    if (unlink(file_path) == 0) {
        printf("File deleted successfully: %s\n", file_to_remove);
    } else {
        perror("Failed to delete file");
    }
}

// 권한을 변경하는 함수
void chmod_command(char *permission_str, char *filename, const char *current_dir) {
    char *full_path = get_full_path(current_dir, filename);
    if (full_path == NULL) {
        printf("Error: Invalid path\n");
        return;
    }

    // ROOT_PATH 제한 확인
    if (strncmp(full_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot operate outside of %s\n", ROOT_PATH);
        free(full_path);
        return;
    }

    // 권한을 8진수로 변환
    mode_t mode = (mode_t)strtol(permission_str, NULL, 8);

    // 파일 정보 확인
    struct stat st;
    if (lstat(full_path, &st) != 0) {
        perror("Error getting file information");
        free(full_path);
        return;
    }

    // 심볼릭 링크 처리
    if (S_ISLNK(st.st_mode)) {
        printf("Warning: %s is a symbolic link. Changing link permissions is not supported.\n", filename);
        free(full_path);
        return;
    }

    // chmod 시스템 콜 호출
    if (chmod(full_path, mode) == 0) {
        // 권한 변경 확인
        if (stat(full_path, &st) == 0 && (st.st_mode & 07777) == mode) {
            printf("Changed permissions of %s to %s\n", filename, permission_str);
        } else {
            printf("Warning: Permissions may not have been changed as expected.\n");
        }
    } else {
        perror("chmod failed");
    }

    free(full_path);
}

// 하드 링크 생성 함수
void ln_command(int argc, char *args[], const char *current_dir) {
    if (argc < 3 || argc > 4) {
        printf("Usage: ln [-s] <original_file> <new_file>\n");
        return;
    }

    int is_symbolic = 0;
    char *original_file, *new_file;

    if (strcmp(args[1], "-s") == 0) {
        if (argc != 4) {
            printf("Usage: ln -s <original_file> <new_file>\n");
            return;
        }
        is_symbolic = 1;
        original_file = args[2];
        new_file = args[3];
    } else {
        if (argc != 3) {
            printf("Usage: ln <original_file> <new_file>\n");
            return;
        }
        original_file = args[1];
        new_file = args[2];
    }

    char *full_original_path = get_full_path(current_dir, original_file);
    char *full_new_path = get_full_path(current_dir, new_file);

    if (full_original_path == NULL || full_new_path == NULL) {
        printf("Error: Invalid path\n");
        free(full_original_path);
        free(full_new_path);
        return;
    }

    // ROOT_PATH 제한 확인
    if (strncmp(full_original_path, ROOT_PATH, strlen(ROOT_PATH)) != 0 ||
        strncmp(full_new_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot operate outside of %s\n", ROOT_PATH);
        free(full_original_path);
        free(full_new_path);
        return;
    }

    int result;
    if (is_symbolic) {
        // 심볼릭 링크 생성
        result = symlink(original_file, full_new_path);
        if (result == 0) {
            printf("Symbolic link created: %s -> %s\n", new_file, original_file);
        } else {
            perror("Failed to create symbolic link");
        }
    } else {
        // 하드 링크 생성
        result = link(full_original_path, full_new_path);
        if (result == 0) {
            printf("Hard link created: %s -> %s\n", new_file, original_file);
        } else {
            perror("Failed to create hard link");
        }
    }

    free(full_original_path);
    free(full_new_path);
}

//
void touch_command(char *filename, const char *current_dir) {
    char *full_path = get_full_path(current_dir, filename);
    if (full_path == NULL) {
        printf("Error: Invalid path\n");
        return;
    }

    // ROOT_PATH 제한 확인
    if (strncmp(full_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
        printf("Error: Cannot operate outside of %s\n", ROOT_PATH);
        free(full_path);
        return;
    }

    // 파일 열기 (없으면 생성)
    FILE *fp = fopen(full_path, "a");
    if (fp == NULL) {
        perror("touch failed");
        free(full_path);
        return;
    }
    fclose(fp);

    // 현재 시간으로 타임스탬프 업데이트
    struct utimbuf new_times;
    time_t current_time = time(NULL);
    new_times.actime = current_time;   // 접근 시간
    new_times.modtime = current_time;  // 수정 시간

    if (utime(full_path, &new_times) == -1) {
        perror("Failed to update timestamp");
    }

    free(full_path);
}
