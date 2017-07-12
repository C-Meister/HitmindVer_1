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
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// 헤더파일 선언문
//기본 헤더파일
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


// 라이브러리 선언문 라이브러리파일은 따로 추가안해도 됩니다.
// #pragma comment 는 visual studio에서만 사용 가능 *솔루션 플렛폼을 64비트로 해주세요
#pragma comment (lib, "libmysql.lib")	//mysql라이브러리
#pragma comment (lib, "SDL2")			//그래픽 라이브러리 1
#pragma comment (lib, "SDL2main")		//그래픽 라이브러리 2
#pragma comment (lib, "SDL2_image")
#pragma comment (lib, "ws2_32.lib")		//소켓(네트워크)라이브러리

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

//전역 변수들 (사용 비추천)

INPUT_RECORD rec;
DWORD dwNOER;
HANDLE COUT = 0;    // 콘솔 출력 장치
HANDLE CIN = 0;        // 콘솔 입력 장치

CRITICAL_SECTION cs;	//이벤트
char message[100];		//소켓 프로그래밍 문자열
char username[30];		//사용자 이름

//기본 함수들

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



int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함 
{
	//변수 선언
	InitializeCriticalSection(&cs);
	int i, j, k, v, result;
	POINT pos;								//x, y좌표 표현 )pos.x, pos.y
	MYSQL *cons = mysql_init(NULL);			//mysql 초기화
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[400];						//mysql 명령어를 저장함
	char mysqlip[30] = "10.80.162.92";		//mysql ip 상희ip입니다	지금 윈도우7버전
	WSADATA wsaData;						//소켓 WSAStartup()함수에 쓰이는 변수
	SOCKET connect_sock;					//서버 소켓변수
	SOCKADDR_IN connect_addr;				//서버 주소정보 저장하는 변수
	char *ServerIP = "10.80.162.41";		//소켓 ip 상호ip임
	char data[1000][30] = { 0, };           //단어데이터
	char nowword[30] = { 0, };              //랜덤선택 단어
	char scanword[30] = { 0, };             //내가 친 단어
	int bangnum = 0;						//고른 방의 번호

	//변수 선언 끝
	disablecursor(1);
	//	ConsoleL(30, 30);
	bangchose();

	return 0;

}

void usermain(void) {
#ifdef SANGHO
#endif
#ifdef SOOHAN
#endif
#ifdef SANGHIE
	
	
#endif
#ifdef MINSUK
#endif
	exit(1);
}
LOG login() {
	//오류 없는 코드니까 회원가입이랑 로그인에 잘 적으시길
	LOG user = { 0, 0 };
	int i = 0;

	/*아이디 암호화 X*/
	printf("id : ");


	while (1) {
		user.id[i] = _getch();

		if (user.id[i] == 8) {
			if (i == 0) {
				user.id[0] = 0;
				continue;
			}
			printf("\b \b");
			user.id[i - 1] = 0;
			user.id[i--] = 0;
		}
		else if (user.id[i] == 13 && i > 3) {
			user.id[i] = 0;
			break;
		}
		else if (user.id[i] == 13) {
			user.id[i] = 0;
		}
		else if (!((user.id[i] >= '0' && user.id[i] <= '9') || (user.id[i] >= 'a' && user.id[i] <= 'z') || (user.id[i] >= 'A' && user.id[i] <= 'Z'))) {
			user.id[i] = 0;
		}
		else
			putchar(user.id[i++]);


	}
	/*비밀번호 암호화 구현*/
	i = 0;
	printf("\npassword : ");

	while (1) {
		user.pass[i] = _getch();

		if (user.pass[i] == 8) {
			if (i == 0) {
				user.pass[0] = 0;
				continue;
			}
			printf("\b \b");
			user.pass[i - 1] = 0;
			user.pass[i--] = 0;
		}
		else if (user.pass[i] == 13 && i > 3) {
			user.pass[i] = 0;
			break;
		}
		else if (user.pass[i] == 13) {
			user.pass[i] = 0;
		}
		else if (!((user.pass[i] >= '0' && user.pass[i] <= '9') || (user.pass[i] >= 'a' && user.pass[i] <= 'z') || (user.pass[i] >= 'A' && user.pass[i] <= 'Z'))) {
			user.pass[i] = 0;
		}
		else {
			printf("*");
			i++;
		}
	}
	return user;
}
void checkword(char*nowword, char*scanword) {

	int cnt = 0;
	int i = 0;
	while (cnt != i - 1) { //단어가 맞을때 까지 돌림

		cnt = 0;

		for (i = 0; scanword[i] != 0; i++) //초기화
			scanword[i] = 0;

		fgets(scanword, sizeof(scanword), stdin); // 상희 채팅서버와 연동

		for (i = 0; nowword[i] != 0; i++) //비교
			if (nowword[i] == scanword[i])
				cnt++;

		if (scanword[i] != 0) //비교(오류방지)
			cnt = 0;

	}
	printf("끝");
} //알아서 비교해줌
void writechating(MYSQL *cons)
{
	char query[300];
	int i = 0;
	TCHAR buff;
	char buff2 = 0;
	TCHAR buffer[100] = { 0, };
	while (1) {

		while (1)
		{
			if (_kbhit())
			{


				buff = _getch();
				if (buff == 13)
					break;
				else if (buff == 8 && i != 0) {
					CLS;
					buffer[i--] = 0;
				}
				else
				{
					if (buff == buff2)
						i++;
					buffer[i++] = buff;
					buffer[i + 1] = 0;
					buff = buff2;
				}
				buff = 0;

			}

			EnterCriticalSection(&cs);
			cur(40, 20);
			printf("%ls", buffer);
			cur(0, 0);
			LeaveCriticalSection(&cs);
			Sleep(10);
		}
		CLS;
		i = 0;
		sprintf(query, "insert into catchmind.chating (name, mean) values ('서상희', '%S')", buffer);
		mysql_query(cons, query);
		memset(buffer, 0, sizeof(buffer));
	}
}
void readchating(MYSQL *cons) {
	int v = 0;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	while (1) {

		v = 0;
		mysql_query(cons, "select * from catchmind.chating order by time desc limit 10");
		sql_result = mysql_store_result(cons);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			EnterCriticalSection(&cs);
			cur(10, 20 - (short)v);
			printf("%s : %s", sql_row[2], sql_row[3]);
			v++;
			LeaveCriticalSection(&cs);

		}
		Sleep(500);
	}

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

	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	while (TRUE) {
		ReadConsoleInput(hIn, &rec, 1, &dwNOER);

		if (rec.EventType == MOUSE_EVENT) {
			if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
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
void ErrorHandling(char *Message) {
	fputs(Message, stderr);
	fputc('\n', stderr);
	return;
}
void SDL_ErrorLog(const char * msg) {//에러코드 출력 함수
	std::cout << msg << " Error: " << SDL_GetError() << std::endl;
	return;
}
void IMG_ErrorLog(const char * msg) {//에러코드 출력 함수
	std::cout << msg << " Error: " << IMG_GetError() << std::endl;
	return;
}
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step) {// 예외 처리 함수
	SDL_ErrorLog(msg);//에러코드 출력
	switch (step) {
	case 3:
		SDL_DestroyRenderer(Renderer);// SDL 렌더러 파괴
	case 2:
		SDL_DestroyWindow(Window);//SDL 윈도우 파괴
	case 1:
		SDL_Quit();//SDL 종료
		getchar();//에러코드 확인하기 위해 콘솔창 대기
		return;
	}
}
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window) {
	SDL_DestroyRenderer(Renderer);// SDL 렌더러 파괴
	SDL_DestroyWindow(Window);//SDL 윈도우 파괴
	SDL_Quit();//SDL 종료
	getchar();//에러코드 확인하기 위해 콘솔창 대기
	return;
}
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file) { // 텍스쳐에 이미지파일 로드하는 함수 선언
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG파일과 PNG파일 로드 가능
	if (IMG_Init(imgFlags) != imgFlags) {//IMG 초기화하고 초기화 안되면 if문 실행
		IMG_ErrorLog("IMG_Init");
		return nullptr;//널포인터 반환
	}
	SDL_Surface* Surface = IMG_Load(file);//서피스에 이미지로드
	if (Surface == nullptr) {//서피스에 이미지로드가 안되면 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG 종료
		return nullptr;// 널포인터 반환
	}
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//서피스로부터 텍스쳐 생성
	SDL_FreeSurface(Surface);// 서피스 메모리해제
	if (Texture == nullptr) {// 텍스쳐 생성 실패시 if문실행
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// 에러 코드 출력
		IMG_Quit();// IMG 종료
		return nullptr;// 널포인터 반환
	}
	IMG_Quit();// IMG 종료
	return Texture;// Texture포인터 반환
}
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip) { // 텍스쳐에 이미지파일 로드하는 함수 선언
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG파일과 PNG파일 로드 가능
	if (IMG_Init(imgFlags) != imgFlags) {//IMG 초기화하고 초기화 안되면 if문 실행
		IMG_ErrorLog("IMG_Init");
		return nullptr;//널포인터 반환
	}
	SDL_Surface* Surface = IMG_Load(file);//서피스에 이미지로드
	if (Surface == nullptr) {//서피스에 이미지로드가 안되면 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG 종료
		return nullptr;// 널포인터 반환
	}
	SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, r, g, b));// r,g,b값에 해당하는 색상을 지우는 함수임
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//서피스로부터 텍스쳐 생성
	SDL_FreeSurface(Surface);// 서피스 메모리해제
	if (Texture == nullptr) {// 텍스쳐 생성 실패시 if문실행
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// 에러 코드 출력
		IMG_Quit();// IMG 종료
		return nullptr;// 널포인터 반환
	}
	IMG_Quit();// IMG 종료
	return Texture;// Texture포인터 반환
}
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h) {//텍스쳐를 출력하는 함수 선언
	SDL_Rect Src;// 직사각형 선언
	Src.x = 0;// 직사각형의 왼쪽위 꼭짓점의 x좌표초기화
	Src.y = 0;// 직사각형의 왼쪽위 꼭짓점의 y좌표초기화
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h); // Texture의 너비와 높이 정보를 Src.w, Src.h에 저장
	SDL_Rect Dst;
	Dst.x = x;//매개변수x를 왼쪽위 꼭짓점의 x좌표에 대입
	Dst.y = y;//매개변수y를 왼쪽위 꼭짓점의 y좌표에 대입
	Dst.w = w;//매개변수w를 직사각형의 너비에 대입
	Dst.h = h;//매개변수h를 직사각형의 높이에 대입
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst);//Src의 정보를 가지고 있는 Texture를 Dst의 정보를 가진 Texture 로 변환하여 렌더러에 저장
}
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr, char *ServerIP) { //서버 연결 해주는 함수
	int status[4] = { 0, };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	//소켓 프로그래밍 시작
		ErrorHandling("WSAStartup() error");
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock변수에 소켓 할당
	connect_addr.sin_family = AF_INET;				//연결할 서버의 주소 설정
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //서버 IP
	connect_addr.sin_port = htons(5555);					 //서버 포트
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //서버에 연결
		ErrorHandling("connect() error");
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, &connect_sock, 0, NULL); //서버에서 데이터를 받아오는 쓰레드 시작
	system("cls");
	send(connect_sock, "player connect",1023,0);
	while (1) { //받아온 데이터 처리
		if (strcmp(message, "player 1 connect") == 0) {
			status[0] = 1;
		}
		if (strcmp(message, "player 2 connect") == 0) {
			status[1] = 1;
		}
		if (strcmp(message, "player 3 connect") == 0) {
			status[2] = 1;
		}
		if (strcmp(message, "player 4 connect") == 0) {
			status[3] = 1;
		}
		if (strcmp(message, "player 1 ready") == 0) {
			status[0] = 2;
		}
		if (strcmp(message, "player 2 ready") == 0) {
			status[1] = 2;
		}
		if (strcmp(message, "player 3 ready") == 0) {
			status[2] = 2;
		}
		if (strcmp(message, "player 4 ready") == 0) {
			status[3] = 2;
		}
		if (strcmp(message, "player 1 not ready") == 0) {
			status[0] = 1;
		}
		if (strcmp(message, "player 2 not ready") == 0) {
			status[1] = 1;
		}
		if (strcmp(message, "player 3 not ready") == 0) {
			status[2] = 1;
		}
		if (strcmp(message, "player 4 not ready") == 0) {
			status[3] = 1;
		}
		gotoxy(0, 3);
		WHITE
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ PLAYER 1 ]                                                                             ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ STATUS ] : ");
		if (status[0] == 0) {
			GRAY
			gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
			printf("DISCONNECTED");
			WHITE
		}
		else if (status[0] == 1) {
			HIGH_GREEN
			gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
				printf("JOIN");
			WHITE
		}
		if (status[0] == 2) {
			SKY_BLUE
			gotoxy(24, 7);
			printf("                   ");
			gotoxy(24, 7);
				printf("READY");
			WHITE
		}
		gotoxy(100, 7);
		printf("■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ PLAYER 2 ]                                                                             ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ STATUS ] : ");
		if (status[1] == 0) {
			GRAY
			gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
				printf("DISCONNECTED");
			WHITE
		}
		else if (status[1] == 1) {
			HIGH_GREEN
				gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
				printf("JOIN");
			WHITE
		}
		if (status[1] == 2) {
			SKY_BLUE
				gotoxy(24, 16);
			printf("                   ");
			gotoxy(24, 16);
				printf("READY");
			WHITE
		}
		gotoxy(100, 16);
		printf("■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ PLAYER 3 ]                                                                             ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ STATUS ] : ");
		if (status[2] == 0) {
			GRAY
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
				printf("DISCONNECTED");
			WHITE
		}
		else if (status[2] == 1) {
			HIGH_GREEN
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
				printf("JOIN");
			WHITE
		}
		if (status[2] == 2) {
			SKY_BLUE
				gotoxy(24, 25);
			printf("                   ");
			gotoxy(24, 25);
				printf("READY");
			WHITE
		}
		gotoxy(100, 25);
		printf("■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■   [ PLAYER 4 ]                                                                             ■\n");
		printf("      ■                                                                                            ■\n"); //92
		printf("      ■   [ STATUS ] : ");
		if (status[3] == 0) {
			GRAY
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
				printf("DISCONNECTED");
			WHITE
		}
		else if (status[3] == 1) {
			HIGH_GREEN
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
				printf("JOIN");
			WHITE
		}
		if (status[3] == 2) {
			SKY_BLUE
				gotoxy(24, 34);
			printf("                   ");
			gotoxy(24, 34);
				printf("READY");
			WHITE
		}
		gotoxy(100, 34);
		printf("■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■                                                                                            ■\n");
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("      ■                ■                                                        ■                ■\n");
		printf("      ■                ■                                                        ■                ■\n");
		printf("      ■                ■                                                        ■                ■\n");
		printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	}
}
void recieve(SOCKET connect_sock) { //서버에서 데이터 받아오는 쓰레드용 함수
	while (1) {
		recv(connect_sock, message, 1023, 0); //서버에서 데이터를 받아와 message변수에 저장

	}
}
int sqllogin(MYSQL *cons) {
	LOG user;								//사용자 정보 구조체
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[100];
	user = login();							//login 함수를 사용
	sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id를 DB에서 찾음
	mysql_query(cons, query);
	sql_result = mysql_store_result(cons);

	if (mysql_fetch_row(sql_result) == NULL)									//해댱 id가 없으면 
	{
		printf("\n아이디가 존재하지 않습니다.\n");
		return 0;
	}
	else
		printf("\n아이디 OK");
	sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass); 
	mysql_query(cons, query);	//password는 DB에 암호화되어 있어서 값을 비교할때도 서로 암호화해서 비교를함
	sql_result = mysql_store_result(cons);
	if (mysql_fetch_row(sql_result) == NULL)
	{
		printf("\n비밀번호가 틀렸습니다.\n");
		return 0;
	}
	else {
		printf("\n로그인 성공");
		return 1;
	}

}
int sqlsignup(MYSQL *cons) {
	LOG user;
	char query[100];
	printf("회원가입을 합니다. 비밀번호는 암호화되어 저장이됩니다.\n");
	user = login();
	printf("\n이름 : ");
	fgets(user.name, sizeof(user.name), stdin);
	CHOP(user.name);							//문자열에 스페이스바를 지워버림
	sprintf(query, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);
	if (!(mysql_query(cons, query)))											//		 password는 mysql에서 지원하는 암호화 형식임.
	{
		printf("\n회원가입 성공");
		return 1;
	}
	else
	{
		printf("\n회원가입 실패");
		return 0;
	}

}
void mainatitleimage() {
	gotoxy(6, 3);
	printf("■■■■■  ■  ■      ■■■    ■    ■■■■■  ■        ■■■    ■      ■■■■■                                           ■■■");
	gotoxy(6, 4);
	printf("        ■  ■  ■                ■    ■      ■  ■      ■      ■  ■      ■                                                 ■      ■");
	gotoxy(6, 5);
	printf("  ■■■■  ■  ■    ■■■■■  ■    ■      ■  ■      ■      ■  ■      ■              ■      ■   ■    ■    ■      ■");
	gotoxy(6, 6);
	printf("      ■    ■■■        ■      ■    ■      ■  ■■      ■■■    ■      ■■■■■      ■      ■       ■■■  ■      ■");
	gotoxy(6, 7);
	printf("    ■      ■  ■      ■  ■    ■    ■      ■  ■                                          ■  ■  ■   ■    ■    ■■    ■");
	gotoxy(6, 8);
	printf("  ■        ■  ■    ■      ■  ■    ■      ■  ■        ■                                ■  ■  ■   ■    ■    ■  ■    ■      ■");
	gotoxy(6, 9);
	printf("■          ■  ■    ■      ■  ■    ■■■■■  ■        ■■■■■■    ■■■■■■■      ■  ■     ■    ■    ■  ■      ■■■");
	gotoxy(12, 20);
	printf("■■■■■■■■■                      ■■■■■■■■■                      ■■■■■■■■■");
	gotoxy(12, 21);
	printf("■              ■                      ■              ■                      ■              ■");
	gotoxy(12, 22);
	printf("■              ■                      ■              ■                      ■              ■");
	gotoxy(12, 23);
	printf("■              ■                      ■              ■                      ■              ■");
	gotoxy(12, 24);
	printf("■              ■                      ■              ■                      ■              ■");
	gotoxy(12, 25);
	printf("■              ■                      ■              ■                      ■              ■");
	gotoxy(12, 26);
	printf("■■■■■■■■■                      ■■■■■■■■■                      ■■■■■■■■■");
}
void maintitle() { //게임 메인타이틀 출력
	ConsoleL(100, 60);
	disablecursor(true);
	int xx = 0, yy = 0;
	while (1) {
		printf("%3d %3d\n", xx, yy);
		mainatitleimage();
		click(&xx, &yy);

		if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25)
			break;

		gotoxy(0, 0);
	}
	CLS;
}
void click(int *xx, int *yy) {
	DWORD mode;
	CIN = GetStdHandle(STD_INPUT_HANDLE); //마우스 재활성화
	GetConsoleMode(CIN, &mode);
	SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);
	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER); // 콘솔창 입력을 받아들임.
	if (rec.EventType == MOUSE_EVENT) {// 마우스 이벤트일 경우

		if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // 좌측 버튼이 클릭되었을 경우
			int mouse_x = rec.Event.MouseEvent.dwMousePosition.X; // X값 받아옴
			int mouse_y = rec.Event.MouseEvent.dwMousePosition.Y; // Y값 받아옴

			*xx = mouse_x / 2;
			*yy = mouse_y;
		}
		else {
			*xx = 0;
			*yy = 0;
		}
	}

}
void banglist() {																				//마우스에서 2를 나눈값을 받는다
#ifdef SANGHIE
	printf("                ┌──────────────┬──────────────┐\n"); //방만들기 9 ~ 22 , 2
	printf("                │          방만들기          │          빠른시작          │\n"); //빠른시작 24 ~ 37 , 2
	printf("                ┟━━━━━━━━━━━━━━┷━━━━━━━━━━━━━━┪\n");
	printf("                ┃                          방목록                          ┃\n"); //방목록 9 ~ 37, 4
	printf("                ┣━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┫\n");
	printf("                ┃                            ┃                            ┃\n"); //방1 9 ~ 22 , 6 ~ 8 
	printf("                ┃           1번방            ┃                            ┃\n"); //방2 24 ~ 37 , 6 ~ 8
	printf("                ┃                            ┃                            ┃\n");
	printf("                ┣━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━┫\n");
	printf("                ┃                            ┃                            ┃\n"); //방3 9 ~ 22 , 10 ~ 12  
	printf("                ┃                            ┃                            ┃\n"); //방4 24 ~ 37 ,10 ~ 12 
	printf("                ┃                            ┃                            ┃\n");
	printf("                ┣━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━┫\n");
	printf("                ┃                            ┃                            ┃\n"); //방5 9 ~ 22 , 14 ~ 16
	printf("                ┃                            ┃                            ┃\n"); //방6 24 ~ 37 , 14 ~ 16
	printf("                ┃                            ┃                            ┃\n");
	printf("                ┗━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┛\n");
#else
	printf("                □□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n"); //방만들기 9 ~ 22 , 2
	printf("                □          방만들기          □          빠른시작          □\n"); //빠른시작 24 ~ 37 , 2
	printf("                □□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("                □                          방목록                          □\n"); //방목록 9 ~ 37, 4
	printf("                ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("                ■                            ■                            ■\n"); //방1 9 ~ 22 , 6 ~ 8 
	printf("                ■           1번방            ■                            ■\n"); //방2 24 ~ 37 , 6 ~ 8
	printf("                ■                            ■                            ■\n");
	printf("                ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("                ■                            ■                            ■\n"); //방3 9 ~ 22 , 10 ~ 12  
	printf("                ■                            ■                            ■\n"); //방4 24 ~ 37 ,10 ~ 12 
	printf("                ■                            ■                            ■\n");
	printf("                ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("                ■                            ■                            ■\n"); //방5 9 ~ 22 , 14 ~ 16
	printf("                ■                            ■                            ■\n"); //방6 24 ~ 37 , 14 ~ 16
	printf("                ■                            ■                            ■\n");
	printf("                ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
#endif
	
}
int bangchose() {

	int xx = 0, yy = 0;
	banglist();
	while (1) {
		printf("%3d %3d\n", xx, yy);
		
		click(&xx, &yy);

		if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)
			return 1;

		gotoxy(0, 0);
	}

}
void numberbaseball() {

	srand((unsigned int)time(NULL));

	int i, j, k;

	int aire[5][100] = { 0, }; //3번째 : s 4번째 : b 5번째 총합
	int aiall[10][10][10] = { 0, };
	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++)
			for (k = 0; k < 10; k++)
				if (i == j || j == k || i == k)
					aiall[i][j][k] = -1;
	int ainum[10] = { 0, };

	int usre[5][100] = { 0, };
	int usnum[3] = { 0, };

	int turn = 0;

	createainumber(ainum);
	scanfirst(usnum);


	while (aire[3][turn - 1] != 3) {

		scan(usre, ainum, turn);
		aiscan(aire, usnum, aiall, turn);
		aicheck(aiall, aire, turn);

		turn++;
	}



	return;

}
void createainumber(int *ainum) {

	int i;

	for (i = 0; i < 3; i++) {

		ainum[i] = rand() % 10;

		if (i == 1 && ainum[i] == ainum[0])
			ainum[i--] = 0;
		else if (i == 2 && (ainum[i] == ainum[1] || ainum[i] == ainum[0]))
			ainum[i--] = 0;
		//printf("%d", ainum[i]);
	}
	printf("\n");

}
void scanfirst(int *usnum) {
	int i;
	printf("당신의 숫자를 선택해 주세요 : ");

	for (i = 0; i < 3; i++) {
		usnum[i] = _getch() - '0';

		if (usnum[i] < 0 || usnum[i] > 9)
			usnum[i--] = 0;
		else if (i == 1 && usnum[i] == usnum[0])
			i--;
		else if (i == 2 && (usnum[i] == usnum[1] || usnum[i] == usnum[0])) {
			i--;
		}
		else
			printf("%d", usnum[i]);

	}

	printf("\n\n    나                    AI\n\n\n");
}
void scan(int(*usre)[100], int *ainum, int t) {

	int i, s = 0, b = 0;
	int tmp1 = 0, tmp2 = 0;

	for (i = 0; i < 3; i++) {

		usre[i][t] = _getch() - '0';

		if (usre[i][t] == 8 - '0') {
			usre[i - 1][t] = 0;
			i -= 2;
			printf("\b \b");
		}
		else if (usre[i][t] < 0 || usre[i][t] > 9)
			usre[i--][t] = 0;
		else if (i == 1 && usre[i][t] == usre[0][t])
			usre[i--][t] = 0;
		else if (i == 2 && (usre[i][t] == usre[0][t] || usre[i][t] == usre[1][t])) {
			usre[i--][t] = 0;
		}
		else
			printf("%d", usre[i][t]);

	}

	for (i = 0; i < 3; i++) {

		tmp1 = (i + 1) % 3;
		tmp2 = (i + 2) % 3;
		if (usre[i][t] == ainum[i])
			s++;
		else if (usre[i][t] == ainum[tmp1])
			b++;
		else if (usre[i][t] == ainum[tmp2])
			b++;
	}

	usre[3][t] = s;
	usre[4][t] = b;
	printf("    %ds %db        ", s, b);

}
void aiscan(int(*aire)[100], int *usnum, int(*aiall)[10][10], int t) {

	int i, s = 0, b = 0;
	int x = 0, y = 0, z = 0;
	int tmp1, tmp2;

	while (aiall[x][y][z] == -1) {
		x = y = z = 0;

		for (i = 0; i < 3; i++)
			aire[i][t] = rand() % 10;

		x = aire[0][t];
		y = aire[1][t];
		z = aire[2][t];

	}

	for (i = 0; i < 3; i++) {

		tmp1 = (i + 1) % 3;
		tmp2 = (i + 2) % 3;
		if (aire[i][t] == usnum[i])
			s++;
		else if (aire[i][t] == usnum[tmp1])
			b++;
		else if (aire[i][t] == usnum[tmp2])
			b++;
	}

	aire[3][t] = s;
	aire[4][t] = b;

	printf("%d %d %d    %ds %db\n", aire[0][t], aire[1][t], aire[2][t], s, b);
}
void aicheck(int(*aiall)[10][10], int(*aire)[100], int t) {

	int i, j, k;
	int s = 0, b = 0;
	int t0 = aire[0][t], t1 = aire[1][t], t2 = aire[2][t];

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			for (k = 0; k < 10; k++) {

				s = 0, b = 0;

				if (i == aire[0][t])
					s++;
				else if (i == aire[1][t])
					b++;
				else if (i == aire[2][t])
					b++;

				if (j == aire[1][t])
					s++;
				else if (j == aire[0][t])
					b++;
				else if (j == aire[2][t])
					b++;

				if (k == aire[2][t])
					s++;
				else if (k == aire[0][t])
					b++;
				else if (k == aire[1][t])
					b++;

				if (s != aire[3][t] || b != aire[4][t])
					aiall[i][j][k] = -1;
			}
		}
	}
}