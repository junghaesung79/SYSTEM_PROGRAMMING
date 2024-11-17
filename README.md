# Shell Project

- 학번: 2020120123
- 이름: 정해성

## 빌드 방법

```bash
make clean
make
./myshell
```

## 평가

* 일관되고 읽기 좋은 코딩 컨벤션
* 간결하면서 의미 있는 주석
  * 실제 명령어처럼 다양한 기능을 제공
* 구조에 대한 고민이 얼마나 반영 되었는지
  * 구조를 변경
  * config.h 등을 이용하여 필요한 기능 추가/삭제 등의 설정이 가능한지
  * Makefile을 얼마나 활용했는지

## 제출

1. 소스코드 묶음 (파일 형식: 1st_project_<학번>.zip) -> 압축 풀고 make 명령으로 바로 빌드 가능해야 함.
2. 소스코드의 각 기능의 동작 여부를 확인하고 설명하는 문서. 기능별 실행 결과 화면을 캡쳐하여 pdf 파일로 제출. (1st_project_<학번>.pdf)

## 구현 과정

- [x] shell 분리
- [x] 실행 방법 파악
- [x] 현재 기능들 실행 확인
- [x] 기능들 리팩토링
  - [x] 지금 있는 기능들 파악
  - [x] 새로운 기능 만들 때 어떤 식으로 명령어 파싱, 분기되는 지 파악
- [ ] 새로 만들 기능들
  - [x] Ctrl+C 입력 방지 구현
  - [x] 프로그램 실행
  - [x] 프로세스 목록 ps 명령어
  - [x] kill 명령어
  - [x] chroot 기능이 뭐지? (첫 과제에서 이야기한 chroot 기능 구현 (chroot함수 사용 금지, 기 공유된 코드 참고))
- [x] 내가 넣고 싶은 기능
  - [x] touch

- [ ] 각 기능에 대해서
  - [ ] 동작 여부를 확인 설명
  - [ ] 기능별 실행 결과 캡쳐
  - [ ] 캡처 pdf
    - [ ] help
    - [ ] cd
    - [ ] mkdir
    - [ ] rmdir
    - [ ] ls
    - [ ] rename
    - [ ] ln
    - [ ] rm
    - [ ] chmod
    - [ ] cat
    - [ ] cp
    - [ ] ps
    - [ ] kill
    - [ ] quit
- [ ] `zip -r 1st_project_2020120123.zip include/ src/ Makefile README.md`