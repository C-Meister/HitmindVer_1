/*
캐치 마인드 with C
프로젝트 제작 일자 : 2017-07-10
프로젝트 제작 종료 목표 : 2017-07-18
팀원
배수한 -- 마우스구현, 메인화면 제작목표
신상호 -- 소켓 프로그래밍 server, client 구현
장민석 -- 그래픽 라이브러리를 사용하여 게임내부 구현
서상희 -- 전체적인 틀을 합침, mysql 구현
※Visual Studio 2013 이상에서 사용이 가능합니다
*/
//전처리기
#define _CRT_SECURE_NO_WARNINGS
// 헤더파일 선언문
//기본 헤더파일
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>	
#include <windows.h>
#include <process.h>		//process 멀티쓰레드용

//특수 헤더파일 (따로 설치) 
#include <SDL.h>			//SDL - 그래픽 헤더파일
			//상희 전용
//#include <mysql/mysql.h>	//DB헤더파일

#include "mysql/mysql.h"


// 라이브러리 선언문 라이브러리파일은 따로 추가안해도 됩니다.
// #pragma comment 는 visual studio에서만 사용 가능 *솔루션 플렛폼을 64비트로 해주세요
#pragma comment (lib, "libmysql.lib")	//mysql라이브러리
#pragma comment (lib, "SDL2")			//그래픽 라이브러리 1
#pragma comment (lib, "SDL2main")		//그래픽 라이브러리 2
#pragma comment (lib, "ws2_32.lib")		//소켓(네트워크)라이브러리

#pragma warning (disable : 4101)		//사용하지 않은 지역변수입니다. 경고 무시
//#define 정의문
#define CLS system("cls")		//화면 지우기
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //커서이동
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //커서이동(같음)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))
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

//기본 함수들
void ConsoleL(int x, int y);					//콘솔창의 크기를 설정하는 함수 x y의 너비가 같다
void loadmysql(MYSQL *cons, char mysqlip[]);	//MySQL에 연결하는 함수
char **onemysqlquery(MYSQL *cons, char *query); //mysql 명령어의 결과하나를 바로 반환해주는 함수
POINT MouseClick(void);							//마우스를 클릭하면 그 값을 바로 반환해주는 함수
void disablecursor(bool a);						//커서 보이기, 숨기기  0 = 보이기 1 = 숨기기
//함수 선언 끝  될수 있으면 모든것을 함수로 만들어주시길 바랍니다.



int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함 
{
	//변수 선언
	int i, j, k, v, result;
	POINT pos;
	MYSQL *cons = mysql_init(NULL);			//mysql 초기화
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[400];						//mysql 명령어를 저장함
	char mysqlip[30] = "10.80.161.182";		//mysql ip 상희ip입니다
	//변수 선언 끝
	disablecursor(1);
	ConsoleL(30, 30);
	loadmysql(cons, mysqlip);
	while (1) {
		pos = MouseClick();

		cur(0, 0);
		printf("%d %d  ", pos.x, pos.y);
	}
	return 0;

}


void loadmysql(MYSQL *cons, char mysqlip[])	//MYSQL 서버 불러오기
{
	CLS;
	printf("데이터베이스 불러오기 시도중...");
	if (cons == NULL) //cons가 초기화를 못했으면
	{
		fprintf(stderr, "%s\n", mysql_error(cons));		//에러 프린트, printf("%s")와 같음
		printf("채팅 서버 불러오기 실패 (초기화 오류)\n");
		Sleep(1000);
		exit(1);
	}
	else
		printf("1");
	if (mysql_real_connect(cons, mysqlip, "root", "tbvjaos15", NULL, 0, NULL, 0) == NULL) //mysql서버로 연결
	{
		printf("\b실패... \n서버가 존재하지 않습니다.\n");
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("새로운 ip를 설정해 주세요.");
		scanf("%s", mysqlip);
		loadmysql(cons, mysqlip);					//재귀함수 호출
	}
	else {
		printf("\b2");
		printf("\n성공");
		mysql_set_character_set(cons, "euckr");
		CLS;
	}

	return;
}
char **onemysqlquery(MYSQL *cons, char *query) {		//mysql 명령어의 결과하나를 바로 반환해주는 함수
	mysql_query(cons, query);
	return mysql_fetch_row(mysql_store_result(cons));

}
POINT MouseClick(void)			//마우스를 클릭하면 그 값을 바로 반환해주는 함수
{ 
	int x, y;
	HANDLE       hIn, hOut;
	DWORD        dwNOER;
	INPUT_RECORD rec;

	hIn=GetStdHandle(STD_INPUT_HANDLE);
	hOut=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	while( TRUE ){
		ReadConsoleInput(hIn,&rec,1,&dwNOER);

		if( rec.EventType == MOUSE_EVENT ){
			if( rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ){
				x = rec.Event.MouseEvent.dwMousePosition.X;
				y = rec.Event.MouseEvent.dwMousePosition.Y;
				return { x, y };
			}
		} 
	}
}
void ConsoleL(int x, int y) {			//콘솔창의 크기를 설정해주는 함수
	char buff[50];
	sprintf(buff, "mode con cols=%d lines=%d", x * 2, y);
	system(buff);
}
void disablecursor(bool a) {
	
	CONSOLE_CURSOR_INFO ConsoleCursor;			// 콘솔커서 정보 구조체
	if (a == true) {
		ConsoleCursor.bVisible = false;				// true 보임 , false 안보임
		ConsoleCursor.dwSize = 1;
	}// 커서 사이즈
	else
	{
		ConsoleCursor.bVisible = true;				// true 보임 , false 안보임
		ConsoleCursor.dwSize = 10;
	}
	SetConsoleCursorInfo(COL, &ConsoleCursor);	// 설정
}