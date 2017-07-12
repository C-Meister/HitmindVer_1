#pragma once
//전처리기
#ifndef CATCHMINDH
#define CATCHMINDH
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <process.h>		//process 멀티쓰레드용
#include <iostream>
//#include <WinSock2.h>		//소켓프로그래밍

//특수 헤더파일 (따로 설치) 
#include "SDL/SDL.h"			//SDL - 그래픽 헤더파일
#include "SDL/SDL_image.h"
//상희 전용
//#include <mysql/mysql.h>	//DB헤더파일

#include "mysql/mysql.h"

//extern 
INPUT_RECORD rec;
//extern
DWORD dwNOER;
//extern 
HANDLE COUT;    // 콘솔 출력 장치
//extern 
HANDLE CIN;        // 콘솔 입력 장치

//extern 
CRITICAL_SECTION cs;	//이벤트
extern char message[100];		//소켓 프로그래밍 문자열
extern char username[30];		//사용자 이름


#pragma warning (disable : 4101)		//사용하지 않은 지역변수입니다. 경고 무시
//#define 정의문
#define CLS system("cls")		//화면 지우기
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //커서이동
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //커서이동(같음)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//색깔설정 X글자 Y 배경
#define CHOP(x) x[strlen(x) - 1] = ' '	//fgets를 쓰면 엔터도 배열에남음. 엔터를 배열에서 삭제해주는것
//색깔출력 쉬운버전 
#define COL                   GetStdHandle(STD_OUTPUT_HANDLE)			// 콘솔창의 핸들정보 받기
#define BLACK                SetConsoleTextAttribute(COL, 0x0000);		// 검정색
#define DARK_BLUE         SetConsoleTextAttribute(COL, 0x0001);			// 파란색
#define GREEN                SetConsoleTextAttribute(COL, 0x0002);      // 녹색
#define BLUE_GREEN        SetConsoleTextAttribute(COL, 0x0003);         // 청녹색
#define BLOOD               SetConsoleTextAttribute(COL, 0x0004);       // 검붉은색
#define PURPLE               SetConsoleTextAttribute(COL, 0x0005);		// 보라색
#define GOLD                 SetConsoleTextAttribute(COL, 0x0006);      // 금색
#define ORIGINAL            SetConsoleTextAttribute(COL, 0x0007);       // 밝은 회색 (기본 콘솔 글씨 색깔)
#define GRAY                 SetConsoleTextAttribute(COL, 0x0008);      // 회색
#define BLUE                  SetConsoleTextAttribute(COL, 0x0009);     // 파란색
#define HIGH_GREEN       SetConsoleTextAttribute(COL, 0x000a);			// 연두색
#define SKY_BLUE           SetConsoleTextAttribute(COL, 0x000b);        // 하늘색
#define RED                   SetConsoleTextAttribute(COL, 0x000c);     // 빨간색
#define PLUM                SetConsoleTextAttribute(COL, 0x000d);       // 자주색
#define YELLOW             SetConsoleTextAttribute(COL, 0x000e);        // 노란색
#define WHITE                SetConsoleTextAttribute(COL, 0x000f);      // 흰색
//구조체 선언
typedef struct {
	char name[20];
	char id[30];
	char pass[50];
}LOG;



void ConsoleL(int x, int y);					//콘솔창의 크기를 설정하는 함수 x y의 너비가 같음
POINT MouseClick(void);							//마우스를 클릭하면 그 값을 바로 반환해주는 함수 반환값은 POINT이다 (x, y)
void disablecursor(bool a);						//커서 보이기, 숨기기  0 = 보이기 1 = 숨기기
void usermain(void);
//--------------------- 네트워크 함수들 -----------------------------------
void ErrorHandling(char *Message);				//소켓 에러 출력 하는 함수
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr, char *ServerIP); //서버 연결 해주는 함수
void recieve(SOCKET connect_sock);				//서버에서 데이터 받아오는 쓰레드용 함수
												//--------------------- MySQL 함수들 --------------------------------------
int sqllogin(MYSQL *cons);						//mysql에 저장된 데이터를 비교해 로그인을 하는 함수
int sqlsignup(MYSQL *cons);						//mysql에 유저데이터를 추가하는 함수
void loadmysql(MYSQL *cons, char mysqlip[]);	//mysql에 연결하는 함수
char **onemysqlquery(MYSQL *cons, char *query); //mysql명령어의 결과하나를 바로 반환해주는 함수
void writechating(MYSQL *cons);					//mysql에 채팅을 입력하는 함수
void readchating(MYSQL *cons);					//mysqm의 채팅을 읽는 함수

												// -------------------- SDL 그래픽 함수들 ---------------------------------
void SDL_ErrorLog(const char * msg);			//그래픽에러코드 출력 함수
void IMG_ErrorLog(const char * msg);			//이미지에러코드 출력 함수
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // 그래픽예외 처리 함수
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // 이미지예외 처리 함수
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // 텍스쳐에 이미지파일 로드하는 함수 선언
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // 텍스쳐에 이미지파일 다양하게 로드하는 함수 선언
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h);	//텍스쳐를 출력하는 함수 선언
																								// -------------------- 게임 내부 함수들 ----------------------------------
void mainatitleimage();
void maintitle(); //게임 메인타이틀 출력
void banglist();
int bangchose();
//-------------------------콘솔 함수들------------------------------------
void checkword(char*nowword, char*scanword);						//단어를 확인함
LOG login();                                    //id 비밀번호를 형식에 맞게 입력을함 
void click(int *xx, int *yy);
//--------------------------미니게임 숫자야구 함수들----------------------
void createainumber(int *ainum);
void scanfirst(int *usnum);
void scan(int(*usre)[100], int *ainum, int turn);
void aiscan(int(*aire)[100], int *usnum, int(*aiall)[10][10], int t);
void aicheck(int(*aiall)[10][10], int(*aire)[100], int t);
void numberbaseball();
//함수 선언 끝  될수 있으면 모든것을 함수로 만들어주시길 바랍니다.
#endif