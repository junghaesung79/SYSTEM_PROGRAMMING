#include "shell.h"

// 메인 함수
int main(int argc, char **argv) {
    ShellState *shell = initialize_shell();

    while (shell->running) {
        printf("%s $ ", shell->current_dir);

        static int eof_count = 0;

        // 컨트롤 + D 두 번 연속 입력 시 종료
        if (fgets(shell->command, MAX_CMD_SIZE - 1, stdin) == NULL) {
            if (feof(stdin)) {
                eof_count++;
                if (eof_count >= 2) {
                    printf("\nlogout\n");
                    break;
                }
                printf("\nPress Ctrl+D again to exit.\n");
                clearerr(stdin);
                continue;
            }
            continue;
        }
        eof_count = 0;

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
        } else if (strcmp(shell->args[0], "clear") == 0) {
            clear_command();
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
            ps_command(shell);
        } else if (shell->args[0][0] == '.' && shell->args[0][1] == '/') {
            execute_command(shell->arg_count, shell->args, shell->current_dir, shell);
        } else if (strcmp(shell->args[0], "kill") == 0) {
            kill_command(shell->arg_count, shell->args);
        } else {
            printf("Fail to run command\n");
        }
    }

    cleanup_shell(shell);
    return 0;
}
