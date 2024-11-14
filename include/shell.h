#ifndef SHELL_H
#define SHELL_H

// 필요한 헤더 파일들
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// 상수 정의
#define MAX_CMD_SIZE (128)
#define MAX_ARGS (5)  // command를 포함한 현재 최대 인자 개수(ln)
#define ROOT_PATH "/tmp/test"

// 유틸리티 함수 선언
char *get_full_path(const char *current_dir, const char *path);
void check_directory();
char get_file_type(mode_t mode);
void mode_to_string(mode_t mode, char *str);

// 명령어 함수 선언
void help_command();
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

#endif
