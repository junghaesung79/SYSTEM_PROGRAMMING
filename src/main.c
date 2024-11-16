#include "shell.h"

// 전역 ShellState 포인터 (시그널 핸들러에서 사용)
static ShellState *global_shell_state = NULL;

// SIGINT 시그널 핸들러
void sigint_handler(int signo) {
    if (global_shell_state) {
        printf("\n%s $ ", global_shell_state->current_dir);
        fflush(stdout);
    }
}

// 셸 초기화 함수
ShellState *initialize_shell() {
    // 루트 디렉토리 확인 및 생성
    check_directory();

    // 셸 상태 구조체 할당
    ShellState *shell = (ShellState *)malloc(sizeof(ShellState));
    if (shell == NULL) {
        perror("Failed to allocate shell state");
        exit(1);
    }

    // 명령어 버퍼 할당
    shell->command = (char *)malloc(MAX_CMD_SIZE);
    if (shell->command == NULL) {
        perror("Failed to allocate command buffer");
        free(shell);
        exit(1);
    }

    // 초기 상태 설정
    strcpy(shell->current_dir, "/");
    shell->arg_count = 0;
    shell->running = 1;
    memset(shell->args, 0, sizeof(shell->args));

    // 전역 포인터 설정
    global_shell_state = shell;

    // SIGINT 핸들러 설정
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        free(shell->command);
        free(shell);
        exit(1);
    }

    return shell;
}

// 셸 정리 함수
void cleanup_shell(ShellState *shell) {
    if (shell) {
        free(shell->command);
        free(shell);
    }
}

// 메인 함수
int main(int argc, char **argv) {
    ShellState *shell = initialize_shell();

    while (shell->running) {
        printf("%s $ ", shell->current_dir);

        if (fgets(shell->command, MAX_CMD_SIZE - 1, stdin) == NULL) {
            if (errno == EINTR) {
                // 시그널에 의해 중단된 경우
                continue;
            }
            break;
        }

        char *tok_str = strtok(shell->command, " \n");
        if (tok_str == NULL) continue;

        if (strcmp(tok_str, "quit") == 0) {
            break;
        }

        shell->args[0] = tok_str;
        shell->arg_count = 1;

        while ((tok_str = strtok(NULL, " \n")) != NULL && shell->arg_count < MAX_ARGS) {
            shell->args[shell->arg_count++] = tok_str;
        }

        // 명령어 처리 로직
        if (strcmp(shell->args[0], "help") == 0) {
            help_command();
        } else if (strcmp(shell->args[0], "cd") == 0) {
            if (shell->arg_count > 1) {
                cd_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: cd <path>\n");
            }
        } else if (strcmp(shell->args[0], "mkdir") == 0) {
            if (shell->arg_count > 1) {
                mkdir_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: mkdir <directory_name>\n");
            }
        } else if (strcmp(shell->args[0], "rmdir") == 0) {
            if (shell->arg_count > 1) {
                rmdir_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: rmdir <directory_name>\n");
            }
        } else if (strcmp(shell->args[0], "ls") == 0) {
            ls_command(shell->current_dir);
        } else if (strcmp(shell->args[0], "rename") == 0) {
            if (shell->arg_count > 2) {
                rename_command(shell->args[1], shell->args[2], shell->current_dir);
            } else {
                printf("Usage: rename <old_name> <new_name>\n");
            }
        } else if (strcmp(shell->args[0], "ln") == 0) {
            if (shell->arg_count >= 3 && shell->arg_count <= 4) {
                ln_command(shell->arg_count, shell->args, shell->current_dir);
            } else {
                printf("Usage: ln [-s] <original_file> <new_file>\n");
            }
        } else if (strcmp(shell->args[0], "rm") == 0) {
            if (shell->arg_count > 1) {
                rm_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: rm <file>\n");
            }
        } else if (strcmp(shell->args[0], "chmod") == 0) {
            if (shell->arg_count == 3) {
                chmod_command(shell->args[1], shell->args[2], shell->current_dir);
            } else {
                printf("Usage: chmod <permission> <filename>\n");
            }
        } else if (strcmp(shell->args[0], "cat") == 0) {
            if (shell->arg_count == 2) {
                cat_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: cat <filename>\n");
            }
        } else if (strcmp(shell->args[0], "cp") == 0) {
            if (shell->arg_count == 3) {
                cp_command(shell->args[1], shell->args[2], shell->current_dir);
            } else {
                printf("Usage: cp <source_file> <destination_file>\n");
            }
        } else if (strcmp(shell->args[0], "touch") == 0) {
            if (shell->arg_count == 2) {
                touch_command(shell->args[1], shell->current_dir);
            } else {
                printf("Usage: touch <filename>\n");
            }
        } else if (strcmp(shell->args[0], "ps") == 0) {
            ps_command();
        } else if (shell->args[0][0] == '.' && shell->args[0][1] == '/') {
            // 실행 파일 실행 (./program 형식)
            execute_command(shell->arg_count, shell->args, shell->current_dir);
        } else {
            printf("Fail to run command\n");
        }
    }

    cleanup_shell(shell);
    return 0;
}
