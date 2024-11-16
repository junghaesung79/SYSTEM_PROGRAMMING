#include <sys/wait.h>

#include "shell.h"

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
