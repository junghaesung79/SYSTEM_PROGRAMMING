# Shell Project

- 학번: 2020120123
- 이름: 정해성

# **프로그램 설명**

이 프로그램은 리눅스 환경에서 동작하는 쉘을 구현했습니다.

## **주요 특징**

사용자의 입력을 받아 명령어를 실행함.

모든 파일 시스템 작업은 /tmp/test 디렉토리 내에서만 동작하도록 제한함.

기본적인 파일, 디렉토리, 프로세스 관리 기능을 제공함.

## 기능 설명

- 사용자 입력을 받아 공백과 개행을 기준으로 파싱함.
- 프롬프트에 현재 디렉토리 경로를 표시함.

### help

- 현재 구현된 모든 명령어의 사용법을 보여줌.
- 사용법, 인자 형식, 각 명령어의 간단한 설명을 출력함.
- ROOT_PATH 제한사항도 함께 표시됨.

### clear

- 화면을 지움.
- 새로운 프롬프트를 출력함.

## 디렉토리 탐색 및 관리

### ls

- 현재 디렉토리의 내용을 상세히 표시함.
- 파일의 권한, 링크 수, 소유자, 크기, 수정 시간을 보여줌.

### cd

- 지정된 경로로 디렉토리를 이동함.
- 상대 경로와 절대 경로 모두 지원함.
- '..'를 통한 상위 디렉토리 이동 시 ROOT_PATH를 벗어나지 않도록 제한함.

### mkdir

- 새로운 디렉토리를 생성함.
- 기본 권한은 755(rwxr-xr-x)로 설정됨.
- 이미 존재하는 디렉토리명을 사용할 경우 에러 메시지 출력.

### rmdir

- 비어있는 디렉토리를 삭제함.
- 디렉토리가 비어있지 않은 경우 삭제되지 않고 에러 메시지 출력.
- 존재하지 않는 디렉토리에 대한 에러 처리.
- 파일 조작

### touch

- 새로운 빈 파일을 생성하거나 기존 파일의 타임스탬프를 갱신함.
- 파일이 이미 존재하는 경우 접근 시간과 수정 시간을 현재 시간으로 업데이트.

### cat

- 지정된 파일의 내용을 화면에 출력함.
- 파일이 존재하지 않거나 읽기 권한이 없는 경우 에러 메시지 출력.

### cp

- 파일을 복사함.
- 원본 파일의 권한과 타임스탬프를 유지함.
- 대상 파일이 이미 존재하는 경우 덮어쓰기 수행.

### rm

- 지정된 파일을 삭제함.
- 디렉토리가 아닌 일반 파일만 삭제 가능.
- 존재하지 않는 파일에 대한 에러 처리.

### rename

- 파일 또는 디렉토리의 이름을 변경함.
- 대상 이름이 이미 존재하는 경우 에러 메시지 출력.
- 이동 경로가 ROOT_PATH를 벗어나지 않도록 검사.

## 권한 및 링크 관리

### chmod

- 파일의 권한을 변경함.
- 8진수 형식으로 권한을 지정 (예: 755, 644).
- 심볼릭 링크 파일의 경우 원본 파일의 권한이 변경됨.

### ln

- 하드링크 또는 심볼릭 링크를 생성함.
- s 옵션으로 심볼릭 링크 생성 지정.
- 링크 생성 시 ROOT_PATH 제한 검사.

## 프로세스 관리

### 프로그램 실행

- ./로 시작하는 명령어로 실행 파일을 실행함.
- 실행 권한 검사 및 ROOT_PATH 제한 확인.
- 명령어 뒤에 &를 붙여 백그라운드 실행 가능.

### ps

- 현재 실행 중인 쉘과 관련 프로세스들의 목록을 표시함.
- 프로세스의 PID, 상태, 명령어 이름을 보여줌.
- 백그라운드로 실행 중인 프로세스도 표시.

### kill

- 지정된 PID의 프로세스에 시그널을 전송함.
- 시그널 번호 지정 가능 (예: kill -9 PID).

## 종료

### quit

- 쉘 프로그램을 종료함.

### Ctrl + D 두 번

- 쉘 프로그램을 종료함.

## **제한사항**

- ROOT_PATH(/tmp/test) 외부 접근 불가
- 명령행 인자는 최대 5개로 제한
- 모든 경로와 파일명은 최대 128바이트로 제한

## **컴파일 및 실행 방법**

### Makefile 구성

- clang 컴파일러 사용
- 소스 파일들을 자동으로 탐지하여 컴파일
- 테스트 프로그램도 함께 컴파일
- 압축 해제 후 `make` 명령으로 실행

### 실행 환경

- Linux 환경에서 실행
- clang 컴파일러 설치 필요

## **프로그램 구조**

### 소스 파일 구성

- main.c: 메인 쉘 루프 및 명령어 파싱
- basic_commands.c: 기본 명령어 구현
- dir_commands.c: 디렉토리 관련 명령어
- file_commands.c: 파일 관련 명령어
- process_commands.c: 프로세스 관리 명령어
- utils.c: 유틸리티 함수들

### 헤더 파일

- shell.h: 모든 함수 선언과 구조체 정의
- 상수 정의 및 표준 라이브러리 포함

## **시그널 처리**

### SIGINT (Ctrl+C) 처리

- 쉘 프로그램 종료 방지
- 현재 프롬프트 재출력
- 실행 중인 자식 프로세스 종료

### 백그라운드 프로세스

- 좀비 프로세스 방지를 위한 wait 처리
- 백그라운드 프로세스 목록 관리

## **메모리 관리**

### 동적 할당

- 명령어 버퍼 동적 할당
- 경로 처리를 위한 임시 버퍼 할당
- 메모리 할당 실패 처리

### 메모리 해제

- 프로그램 종료 시 전체 메모리 정리
- 각 명령어 실행 후 임시 버퍼 해제
- 메모리 누수 방지를 위한 cleanup 함수

## **테스트 프로그램**

### count 프로그램

- 10부터 카운트다운하며 1초씩 대기
- 프로세스 상태 변화 테스트용
- PID 출력 기능

### infinite 프로그램

- 무한 루프로 실행되는 프로그램
- 백그라운드 실행 테스트용
- 1초 간격으로 동작 상태 출력
