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
//전역 변수들 (사용 비추천)
//이벤트
CRITICAL_SECTION cs;
char message[100];

//기본 함수들
void ConsoleL(int x, int y);					//콘솔창의 크기를 설정하는 함수 x y의 너비가 같음
POINT MouseClick(void);							//마우스를 클릭하면 그 값을 바로 반환해주는 함수 반환값은 POINT이다 (x, y)
void disablecursor(bool a);						//커서 보이기, 숨기기  0 = 보이기 1 = 숨기기
//--------------------- 네트워크 함수들 -----------------------------------
void ErrorHandling(char *Message);				//소켓 에러 출력 하는 함수
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr,char *ServerIP); //서버 연결 해주는 함수
void recieve(SOCKET connect_sock);	//서버에서 데이터 받아오는 쓰레드용 함수
//--------------------- MySQL 함수들 --------------------------------------
void loadmysql(MYSQL *cons, char mysqlip[]);	//MySQL에 연결하는 함수
char **onemysqlquery(MYSQL *cons, char *query); //mysql 명령어의 결과하나를 바로 반환해주는 함수
void writechating(MYSQL *cons);
void readchating(MYSQL *cons);

// -------------------- SDL 그래픽 함수들 ---------------------------------
void SDL_ErrorLog(const char * msg);			//그래픽에러코드 출력 함수
void IMG_ErrorLog(const char * msg);			//이미지에러코드 출력 함수
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // 그래픽예외 처리 함수
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // 이미지예외 처리 함수
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // 텍스쳐에 이미지파일 로드하는 함수 선언
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // 텍스쳐에 이미지파일 다양하게 로드하는 함수 선언
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h);	//텍스쳐를 출력하는 함수 선언
// -------------------- 게임 내부 함수들 ----------------------------------

//-------------------------콘솔 함수들------------------------------------
void checkword(char*nowword, char*scanword);
void login();
//함수 선언 끝  될수 있으면 모든것을 함수로 만들어주시길 바랍니다.



int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함 
{
	//변수 선언
	InitializeCriticalSection(&cs);
	int i, j, k, v, result;
	POINT pos;
	MYSQL *cons = mysql_init(NULL);			//mysql 초기화
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[400];						//mysql 명령어를 저장함
	char mysqlip[30] = "10.80.161.182";		//mysql ip 상희ip입니다
	WSADATA wsaData;						//소켓 WSAStartup()함수에 쓰이는 변수
	SOCKET connect_sock;					//서버 소켓변수
	SOCKADDR_IN connect_addr;				//서버 주소정보 저장하는 변수
	char *ServerIP = "10.80.162.41";		//소켓 ip 상호ip임
	char data[1000][30] = { 0, };           //단어데이터
	char nowword[30] = { 0, };              //랜덤선택 단어
	char scanword[30] = { 0, };             //내가 친 단어

	//변수 선언 끝

	disablecursor(1);
//	ConsoleL(30, 30);
	loadmysql(cons, mysqlip);
	uintptr_t pc = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)writechating, cons, 0, NULL);
	cur(120, 30);
	printf("-------------");
	while (1) {
		
		EnterCriticalSection(&cs);
		LeaveCriticalSection(&cs);
	//	cur(0, 0);
	//	printf("%d %d  ", pos.x, pos.y);
	}
	return 0;

}

void login() {
	//오류 없는 코드니까 회원가입이랑 로그인에 잘 적으시길
	char id[100] = { 0, }; //아이디저장 배열
	char pass[100];        //비밀번호저장 배열 
	int i = 0;

	/*비밀번호 암호화 구현*/
	printf("id : ");

	while (1) {
		id[i] = _getch();

		if (id[i] == 8) {
			if (i == 0) {
				id[0] = 0;
				continue;
			}
			printf("\b \b");
			id[i - 1] = 0;
			id[i--] = 0;
		}
		else if (id[i] == 13 && i > 3) {
			id[i] = 0;
			break;
		}
		else if (id[i] == 13) {
			id[i] = 0;
		}
		else if (!((id[i] >= '0' && id[i] <= '9') || (id[i] >= 'a' && id[i] <= 'z') || (id[i] >= 'A' && id[i] <= 'Z'))) {
			id[i] = 0;
		}
		else
			putchar(id[i++]);


	}

	/*비밀번호 암호화 구현*/
	i = 0;
	printf("\npassword : ");

	while (1) {
		pass[i] = _getch();

		if (pass[i] == 8) {
			if (i == 0) {
				pass[0] = 0;
				continue;
			}
			printf("\b \b");
			pass[i - 1] = 0;
			pass[i--] = 0;
		}
		else if (pass[i] == 13 && i > 3) {
			pass[i] = 0;
			break;
		}
		else if (pass[i] == 13) {
			pass[i] = 0;
		}
		else if (!((pass[i] >= '0' && pass[i] <= '9') || (pass[i] >= 'a' && pass[i] <= 'z') || (pass[i] >= 'A' && pass[i] <= 'Z'))) {
			pass[i] = 0;
		}
		else {
			printf("*");
			i++;
		}
	}
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
				EnterCriticalSection(&cs);
				
				buff = getch();
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
					buff = buff2;
				}
				buff = 0;
				LeaveCriticalSection(&cs);
			}
			cur(40, 20);
			printf("%S", buffer);
			cur(0, 0);
			Sleep(10);
		}
		CLS;
		i = 0;
		sprintf(query, "insert into catchmind.chating (name, mean) values ('서상희', '%S')", buffer);
		mysql_query(cons, query);
		memset(buffer, 0, sizeof(buffer));
		cur(40, 20);
		printf("                                                   ");
	}
}
void readchating(MYSQL *cons) {
	int v = 0;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	while (1) {
		mysql_query(cons, "select * from catchmind.chating");
		sql_result = mysql_store_result(cons);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			if (sql_row[0][0] - '0' != v)
			{

				cur(10,10 + (short)v);
				printf("%s : %s", sql_row[1], sql_row[2]);
				v++;
			}
		}
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
void ErrorHandling(char *Message) {
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);	//오류가 발생했으니 그대로 프로그램 종료
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
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	//소켓 프로그래밍 시작
		ErrorHandling("WSAStartup() error");
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock변수에 소켓 할당
	connect_addr.sin_family = AF_INET;				//연결할 서버의 주소 설정
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //서버 IP
	connect_addr.sin_port = htons(5555);					 //서버 포트
	if(connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //서버에 연결
		ErrorHandling("connect() error");
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, &connect_sock, 0, NULL); //서버에서 데이터를 받아오는 쓰레드 시작
	while (1) { //받아온 데이터 처리

	}
}
void recieve(SOCKET connect_sock) { //서버에서 데이터 받아오는 쓰레드용 함수
	while (1) {
		if (recv(connect_sock, message, 1023, 0)>0) { //서버에서 데이터를 받아와 message변수에 저장

		}
	}
}