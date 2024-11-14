#include "shell.h"

// 메인 함수
int main(int argc, char **argv) {
    // 루트 디렉토리 확인 및 생성
    check_directory();

    char *command, *tok_str;
    char current_dir[MAX_CMD_SIZE] = "/";  // 현재 디렉토리를 문자열 배열로 변경
    char *args[MAX_ARGS];                  // 명령어 인자를 저장할 배열 추가
    int arg_count;                         // 인자 개수를 저장할 변수 추가

    command = (char *)malloc(MAX_CMD_SIZE);
    if (command == NULL) {
        perror("malloc");
        exit(1);
    }

    do {
        printf("%s $ ", current_dir);

        if (fgets(command, MAX_CMD_SIZE - 1, stdin) == NULL) break;

        tok_str = strtok(command, " \n");
        if (tok_str == NULL) continue;

        if (strcmp(tok_str, "quit") == 0) {
            break;
        } else {
            // printf("your command: %s\n", tok_str); // 디버깅 시 사용
            args[0] = tok_str;  // 첫 번째 인자(명령어)를 args 배열에 저장
            arg_count = 1;      // 인자 개수 초기화

            // 모든 인자를 파싱하여 args 배열에 저장
            while ((tok_str = strtok(NULL, " \n")) != NULL && arg_count < MAX_ARGS) {
                args[arg_count++] = tok_str;
                // printf("and argument: "); // 디버깅 시 사용
                // printf("%s ", tok_str);
                // printf("\n");
            }

            // 각 명령어에 대한 처리 로직 추가, 각 명령어 당 필요한 인자의 개수로 성공 여부
            if (strcmp(args[0], "help") == 0) {
                help_command();
            } else if (strcmp(args[0], "cd") == 0) {
                if (arg_count > 1) {
                    cd_command(args[1], current_dir);
                } else {
                    printf("Usage: cd <path>\n");
                }
            } else if (strcmp(args[0], "mkdir") == 0) {
                if (arg_count > 1) {
                    mkdir_command(args[1], current_dir);
                } else {
                    printf("Usage: mkdir <directory_name>\n");
                }
            } else if (strcmp(args[0], "rmdir") == 0) {
                if (arg_count > 1) {
                    rmdir_command(args[1], current_dir);
                } else {
                    printf("Usage: rmdir <directory_name>\n");
                }
            } else if (strcmp(args[0], "ls") == 0) {
                ls_command(current_dir);
            } else if (strcmp(args[0], "rename") == 0) {
                if (arg_count > 2) {
                    rename_command(args[1], args[2], current_dir);
                } else {
                    printf("Usage: rename <old_name> <new_name>\n");
                }
            } else if (strcmp(args[0], "ln") == 0) {
                if (arg_count >= 3 && arg_count <= 4) {
                    ln_command(arg_count, args, current_dir);
                } else {
                    printf("Usage: ln [-s] <original_file> <new_file>\n");
                }
            } else if (strcmp(args[0], "rm") == 0) {
                if (arg_count > 1) {
                    rm_command(args[1], current_dir);
                } else {
                    printf("Usage: rm <file>\n");
                }
            } else if (strcmp(args[0], "chmod") == 0) {
                if (arg_count == 3) {
                    chmod_command(args[1], args[2], current_dir);
                } else {
                    printf("Usage: chmod <permission> <filename>\n");
                }
            } else if (strcmp(args[0], "cat") == 0) {
                if (arg_count == 2) {
                    cat_command(args[1], current_dir);
                } else {
                    printf("Usage: cat <filename>\n");
                }
            } else if (strcmp(args[0], "cp") == 0) {
                if (arg_count == 3) {
                    cp_command(args[1], args[2], current_dir);
                } else {
                    printf("Usage: cp <source_file> <destination_file>\n");
                }
            } else {
                printf("Fail to run command\n");
            }
        }
    } while (1);

    free(command);

    return 0;
}
