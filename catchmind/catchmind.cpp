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
#ifdef SANGHIE				//상희 전용
#include <mysql/mysql.h>	//DB헤더파일
#else
#include <mysql.h>
#endif

// 라이브러리 선언문
// #pragma comment 는 visual studio에서만 사용 가능
#pragma comment (lib, "libmysql.lib")	//mysql라이브러리
#pragma comment (lib, "SDL2")			//그래픽 라이브러리 1
#pragma comment (lib, "SDL2main")		//그래픽 라이브러리 2
#pragma comment (lib, "ws2_32.lib")		//소켓(네트워크)라이브러리
//#define 정의문
#define CLS system("cls")		//화면 지우기
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //커서이동
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //커서이동(같음)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))

//색깔출력 쉬운버전 
#define COL                   GetStdHandle(STD_OUTPUT_HANDLE)        // 콘솔창의 핸들정보 받기
#define BLACK                SetConsoleTextAttribute(COL, 0x0000);        // 검정색
#define DARK_BLUE         SetConsoleTextAttribute(COL, 0x0001);        // 파란색
#define GREEN                SetConsoleTextAttribute(COL, 0x0002);        // 녹색
#define BLUE_GREEN        SetConsoleTextAttribute(COL, 0x0003);        // 청녹색
#define BLOOD               SetConsoleTextAttribute(COL, 0x0004);        // 검붉은색
#define PURPLE               SetConsoleTextAttribute(COL, 0x0005);        // 보라색
#define GOLD                 SetConsoleTextAttribute(COL, 0x0006);        // 금색
#define ORIGINAL            SetConsoleTextAttribute(COL, 0x0007);        // 밝은 회색 (ORIGINAL CONSOLE COLOR)
#define GRAY                 SetConsoleTextAttribute(COL, 0x0008);        // 회색
#define BLUE                  SetConsoleTextAttribute(COL, 0x0009);        // 파란색
#define HIGH_GREEN       SetConsoleTextAttribute(COL, 0x000a);        // 연두색
#define SKY_BLUE           SetConsoleTextAttribute(COL, 0x000b);        // 하늘색
#define RED                   SetConsoleTextAttribute(COL, 0x000c);        // 빨간색
#define PLUM                SetConsoleTextAttribute(COL, 0x000d);        // 자주색
#define YELLOW             SetConsoleTextAttribute(COL, 0x000e);        // 노란색
#define WHITE                SetConsoleTextAttribute(COL, 0x000f);        // 흰색

//기본 함수들
void loadmysql(MYSQL *cons, char mysqlip[]); //MySQL에 연결하는 함수
char **mysqlquery(MYSQL *cons, char *query, int max);

int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함
{
	
	//변수 선언
	int i, j, k, v, result;
	MYSQL *cons = mysql_init(NULL);
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	char query[400];
	char mysqlip[30] = "10.80.161.182";
	//끝
	loadmysql(cons, mysqlip);
	return 0;

}


void loadmysql(MYSQL *cons, char mysqlip[])	//MYSQL 서버 불러오기
{
	CLS;
	printf("데이터베이스 불러오기 시도중...");
	if (cons == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("채팅 서버 불러오기 실패 (초기화 오류)\n");
		Sleep(1000);
		exit(1);
	}
	else
		printf("1");
	if (mysql_real_connect(cons, mysqlip, "root", "tbvjaos15", NULL, 0, NULL, 0) == NULL)
	{
		printf("\b실패... \n서버가 존재하지 않습니다.\n");
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("새로운 ip를 설정해 주세요.");
		scanf("%s", mysqlip);
		loadmysql(cons, mysqlip);
	}
	else {
		printf("\b2");
		printf("\n성공");
		mysql_set_character_set(cons, "euckr");
		CLS;
	}

	return;
}
char **mysqlquery(MYSQL *cons, char *query, int max) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	mysql_query(cons, query);
	sql_result = mysql_store_result(cons);
	return mysql_fetch_row(sql_result);

}