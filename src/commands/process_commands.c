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
void ps_command(ShellState *shell) {
    printf("\n  PID STATE COMMAND\n");
    printf("------------------------\n");

    // 현재 쉘 프로세스 출력
    printf("%5d R     Current Shell\n", getpid());

    // 백그라운드 프로세스 상태 확인 및 출력
    for (int i = 0; i < shell->bg_process_count; i++) {
        if (shell->bg_processes[i].active) {
            int status;
            pid_t result = waitpid(shell->bg_processes[i].pid, &status, WNOHANG);

            if (result == 0) {  // 프로세스가 여전히 실행 중
                printf("%5d R     %s\n",
                       shell->bg_processes[i].pid,
                       shell->bg_processes[i].command);
            } else {
                // 프로세스가 종료됨
                shell->bg_processes[i].active = 0;
            }
        }
    }
}

// 프로그램을 실행하는 함수
void execute_command(int argc, char *args[], const char *current_dir, ShellState *shell) {
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
            // ... (기존 포그라운드 실행 코드)
        } else {
            // 백그라운드 실행
            if (shell->bg_process_count < MAX_BG_PROCESSES) {
                shell->bg_processes[shell->bg_process_count].pid = pid;
                strncpy(shell->bg_processes[shell->bg_process_count].command,
                        args[0], MAX_CMD_SIZE - 1);
                shell->bg_processes[shell->bg_process_count].active = 1;
                shell->bg_process_count++;
                printf("[%d] %s\n", pid, args[0]);
            }
        }
    }
}

// 프로세스 종료 함수
void kill_command(int argc, char *args[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: kill [-signal] pid\n");
        return;
    }

    pid_t pid;
    int signal = SIGTERM;  // 기본 시그널

    if (argc == 3) {
        // kill -9 1234 형식
        if (args[1][0] == '-') {
            signal = atoi(args[1] + 1);
            pid = atoi(args[2]);
        } else {
            printf("Invalid signal format. Use: kill -signal pid\n");
            return;
        }
    } else {
        // kill 1234 형식
        pid = atoi(args[1]);
    }

    if (pid <= 0) {
        printf("Invalid PID\n");
        return;
    }

    if (kill(pid, signal) == -1) {
        perror("kill failed");
    } else {
        printf("Signal %d sent to process %d\n", signal, pid);
    }
}
