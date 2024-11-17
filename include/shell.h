#ifndef SHELL_H
#define SHELL_H

// 필요한 헤더 파일들
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

// 상수 정의
#define MAX_CMD_SIZE (128)
#define MAX_ARGS (5)  // command를 포함한 현재 최대 인자 개수(ln)
#define ROOT_PATH "/tmp/test"
#define MAX_BG_PROCESSES 32

typedef struct {
    char *command;
    char current_dir[MAX_CMD_SIZE];
    char *args[MAX_ARGS];
    int arg_count;
    volatile sig_atomic_t running;

    struct {
        pid_t pid;
        char command[MAX_CMD_SIZE];
        int active;  // 프로세스 활성 상태
    } bg_processes[MAX_BG_PROCESSES];
    int bg_process_count;
} ShellState;

// 유틸리티 함수 선언
char *get_full_path(const char *current_dir, const char *path);
void check_directory();
char get_file_type(mode_t mode);
void mode_to_string(mode_t mode, char *str);
int contains_parent_dir(const char *path);
char *normalize_path(const char *path);
ShellState *initialize_shell();
void cleanup_shell(ShellState *shell);
void sigint_handler(int signo);

// 명령어 함수 선언
void help_command();
void clear_command();
void cd_command(char *path, char *current_dir);
void mkdir_command(char *path, const char *current_dir);
void rmdir_command(char *path, const char *current_dir);
void rename_command(char *source, char *target, const char *current_dir);
void ls_command(const char *current_dir);
void ln_command(int argc, char *args[], const char *current_dir);
void rm_command(char *file_to_remove, const char *current_dir);
void chmod_command(char *permission_str, char *filename, const char *current_dir);
void cat_command(char *filename, const char *current_dir);
void cp_command(char *source_file, char *dest_file, const char *current_dir);
void touch_command(char *filename, const char *current_dir);
void ps_command(ShellState *shell);
void execute_command(int argc, char *args[], const char *current_dir, ShellState *shell);
void kill_command(int argc, char *args[]);

#endif
