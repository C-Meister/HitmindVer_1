/*
캐치 마인드 with C
프로젝트 제작 일자 : 2017-07-10
프로젝트 제작 종료 목표 : 2017-07-18
팀원
배수한 -- 마우스구현, 메인화면 제작목표
신상호 -- 소켓 프로그래밍 server, client 구현
장민석 -- 그래픽 라이브러리를 사용하여 게임내부 구현
서상희 -- 전체적인 틀을 합침, mysql 구현
※Visual Studio 2017에서 사용이 용이합니다
*/
// 헤더파일 선언문
//기본 헤더파일
#include <stdio.h>
#include <conio.h>	
#include <windows.h>
#include <process.h>		//process 멀티쓰레드용

//특수 헤더파일 (따로 설치)
#include <SDL.h>			//SDL - 그래픽 헤더파일
#include <mysql/mysql.h>	//mysql 만약 오류가있으면 <mysql.h>로 변환


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
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4));
//기본 함수들

int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함
{

	cur(5, 5);
	printf("hello앙");
	return 0;

}
