#include <sys/wait.h>

#include "shell.h"

struct ProcessInfo {
    pid_t pid;
    char state;
    char command[256];
    char username[32];
};

// 프로세스 상태를 문자로 변환
char get_process_state(char *state) {
    switch (state[0]) {
        case 'R':
            return 'R';  // Running
        case 'S':
            return 'S';  // Sleeping
        case 'D':
            return 'D';  // Disk sleep
        case 'Z':
            return 'Z';  // Zombie
        case 'T':
            return 'T';  // Stopped
        default:
            return '?';
    }
}

// 프로세스 정보 읽기
int read_process_info(pid_t pid, struct ProcessInfo *info) {
    char path[256];
    char buffer[1024];
    FILE *fp;

    // /proc/[pid]/stat 파일 읽기
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fp = fopen(path, "r");
    if (!fp) return 0;

    if (fgets(buffer, sizeof(buffer), fp)) {
        char comm[256];
        // 사용하지 않는 변수들 제거
        sscanf(buffer, "%d %s %c", &info->pid, comm, &info->state);

        // 명령어 이름 정리 (괄호 제거)
        char *start = strchr(buffer, '(');
        char *end = strrchr(buffer, ')');
        if (start && end && end > start) {
            int len = end - start - 1;
            strncpy(info->command, start + 1, len);
            info->command[len] = '\0';
        }
    }
    fclose(fp);

    // 사용자 이름 가져오기
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strncmp(buffer, "Uid:", 4) == 0) {
                int uid;
                sscanf(buffer, "Uid: %d", &uid);
                struct passwd *pw = getpwuid(uid);
                if (pw) {
                    strncpy(info->username, pw->pw_name, sizeof(info->username) - 1);
                    info->username[sizeof(info->username) - 1] = '\0';
                }
                break;
            }
        }
        fclose(fp);
    }

    return 1;
}

// 프로세스 목록 출력
void ps_command() {
    DIR *proc_dir;
    struct dirent *entry;
    struct ProcessInfo processes[1024];
    int process_count = 0;

    // /proc 디렉토리 열기
    proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("Cannot open /proc");
        return;
    }

    // 헤더 출력
    printf("\n  PID USER     STATE COMMAND\n");
    printf("------------------------\n");

    // /proc 디렉토리 읽기
    while ((entry = readdir(proc_dir)) != NULL) {
        // PID 디렉토리만 처리 (숫자로 된 이름)
        if (isdigit(entry->d_name[0])) {
            pid_t pid = atoi(entry->d_name);
            struct ProcessInfo info;

            if (read_process_info(pid, &info)) {
                processes[process_count++] = info;
                if (process_count >= 1024) break;
            }
        }
    }
    closedir(proc_dir);

    // 프로세스 정보 출력
    for (int i = 0; i < process_count; i++) {
        printf("%5d %-8s %c %s\n",
               processes[i].pid,
               processes[i].username,
               processes[i].state,
               processes[i].command);
    }
    printf("\n");
}

// 프로그램을 실행하는 함수
void execute_command(int argc, char *args[], const char *current_dir) {
    pid_t pid;
    int background = 0;  // 백그라운드 실행 여부

    // 백그라운드 실행 확인 ('&' 체크)
    if (argc > 1 && strcmp(args[argc - 1], "&") == 0) {
        background = 1;
        args[argc - 1] = NULL;  // & 제거
        argc--;
    }

    pid = fork();
    if (pid < 0) {
        // fork 실패
        perror("fork failed");
        return;
    } else if (pid == 0) {
        // 자식 프로세스
        char *full_path = get_full_path(current_dir, args[0]);
        if (full_path == NULL) {
            printf("Error: Invalid path\n");
            exit(1);
        }

        // ROOT_PATH 제한 확인
        if (strncmp(full_path, ROOT_PATH, strlen(ROOT_PATH)) != 0) {
            printf("Error: Cannot execute outside of %s\n", ROOT_PATH);
            free(full_path);
            exit(1);
        }

        // 실행 권한 확인
        if (access(full_path, X_OK) == -1) {
            printf("Error: %s is not executable\n", args[0]);
            free(full_path);
            exit(1);
        }

        // 프로그램 실행
        execv(full_path, args);

        // execv가 실패하면 여기로 옴
        perror("execv failed");
        free(full_path);
        exit(1);
    } else {
        // 부모 프로세스
        if (!background) {
            // 포그라운드 실행: 자식 프로세스 종료 대기
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                // 정상 종료
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0) {
                    printf("Program exited with status %d\n", exit_status);
                }
            } else if (WIFSIGNALED(status)) {
                // 시그널에 의한 종료
                printf("Program terminated by signal %d\n", WTERMSIG(status));
            }
        } else {
            // 백그라운드 실행: 자식 프로세스 ID 출력
            printf("[%d] %s\n", pid, args[0]);
        }
    }
}
