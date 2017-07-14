
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
#include <math.h>
#include <windows.h>
#include <process.h>		//process 멀티쓰레드용
#include <winapifamily.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
//#include <WinSock2.h>		//소켓프로그래밍

//특수 헤더파일 (따로 설치) 
#include "SDL/SDL.h"			//SDL - 그래픽 헤더파일
#include "SDL/SDL_image.h"
#include "SDL/sdl_ttf.h"
//#include "SDL/render/SDL_sysrender.h"
#include "mysql/mysql.h"
#define nullptr 0

// 라이브러리 선언문 라이브러리파일은 따로 추가안해도 됩니다.
// #pragma comment 는 visual studio에서만 사용 가능 *솔루션 플렛폼을 64비트로 해주세요
#pragma comment (lib, "libmysql.lib")	//mysql라이브러리
#pragma comment (lib, "SDL2")			//그래픽 라이브러리 1
#pragma comment (lib, "SDL2main")		//그래픽 라이브러리 2
#pragma comment (lib, "SDL2_image")		//그래픽 라이브러리 3

#pragma comment (lib, "ws2_32.lib")		//소켓(네트워크)라이브러리


#pragma warning (disable : 4700)
#pragma warning (disable : 4244)
#pragma warning (disable : 4101)		//사용하지 않은 지역변수입니다. 경고 무시
//#define 정의문
#define CLS system("cls")		//화면 지우기
//#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //커서이동
//#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //커서이동(같음)
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
typedef struct {
	LONG x;
	LONG y;
}MOUSEPOINT;
typedef struct {
	char roomname[30];
	char password[30];
	char ip[20];
}ROOM;
typedef struct tagPOINT *PPOINT;
typedef struct tagPOINT *LPPOINT;
struct {
	bool eraser;
	bool pencil;
}clicks = { false,false };
struct on {
	bool eraser;
	bool pencil;
	bool new;
}on = { false,false,false };
//전역 변수들 (사용 비추천)

CRITICAL_SECTION cs;	//이벤트
//char message[100];		//소켓 프로그래밍 문자열
char status[4];			//소켓용
char username[30];		//사용자 이름
char friendname[4][30] = { "Player 1", "Player 2", "Player 3", "Player 4" };
WSADATA wsaData;						//소켓 WSAStartup()함수에 쓰이는 변수
SOCKET connect_sock, Sconnect_sock[4], listen_sock;	//서버 소켓변수
SOCKADDR_IN connect_addr, listen_addr;			//서버 주소정보 저장하는 변수
int sockaddr_in_size;
ROOM connectroom[6];
uintptr_t threads[10] = { 0, };
char signalmode;
char querys[10][100];
bool lead = false;
char SOCKETCOUNT = 0;
char clientcatchmind[50];
MYSQL *con;
bool SDL_Clear = false;
SDL_Rect ReceiveRect = { 0 };
int SDLCLOCK = 0;



//기본 함수들
void gotoxy(short x, short y);
void cur(short x, short y);
void exitsignal(void);
void signalall(void);
void ConsoleL(int x, int y);					//콘솔창의 크기를 설정하는 함수 x y의 너비가 같음
POINT MouseClick(void);							//마우스를 클릭하면 그 값을 바로 반환해주는 함수 반환값은 POINT이다 (x, y)
void disablecursor(bool a);						//커서 보이기, 숨기기  0 = 보이기 1 = 숨기기
void usermain(void);
//--------------------- 네트워크 함수들 -----------------------------------
void ErrorHandling(char *Message);				//소켓 에러 출력 하는 함수
int Connect_Server(char *ServerIP);			//서버 연결 해주는 함수
void recieve(void);								//서버에서 데이터 받아오는 쓰레드용 함수
void sendall(char *message);					//하나를받으면 전부전송
int waitroom(void);							//네트워크 대기방
void Clnt_1(int v);								//서버 - 클라이언트 1통신
void makeroom(int *count);							//방만들기(네트워크)
IN_ADDR GetDefaultMyIP(void);					//내 ip 얻기
bool exitallthread(void);

//--------------------- MySQL 함수들 --------------------------------------
int sqllogin(MYSQL *cons);						//mysql에 저장된 데이터를 비교해 로그인을 하는 함수
int sqlsignup(MYSQL *cons);						//mysql에 유저데이터를 추가하는 함수
void loadmysql(MYSQL *cons, char mysqlip[]);	//mysql에 연결하는 함수
char **onemysqlquery(MYSQL *cons, char *query); //mysql명령어의 결과하나를 바로 반환해주는 함수
void writechating(MYSQL *cons);					//mysql에 채팅을 입력하는 함수
void readchating(MYSQL *cons);					//mysql의 채팅을 읽는 함수
void inserttopic(MYSQL *cons);
void sqlmakeroom(MYSQL *cons);					//방을 만드는 함수

// -------------------- SDL 그래픽 함수들 ---------------------------------
void SDL_ErrorLog(const char * msg);//에러코드 출력 함수			//그래픽에러코드 출력 함수
void IMG_ErrorLog(const char * msg);			//이미지에러코드 출력 함수
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // 그래픽예외 처리 함수
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // 이미지예외 처리 함수
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // 텍스쳐에 이미지파일 로드하는 함수 선언
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // 텍스쳐에 이미지파일 다양하게 로드하는 함수 선언
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect);	//텍스쳐를 출력하는 함수 선언
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Rect Track, SDL_Rect Box, SDL_Rect Eraser, SDL_Rect Pencil, SDL_Rect New, SDL_Rect Font, float strong, int r, int g, int b);	//랜더러를 올리는 함수
void SDL_FontUpdate(SDL_Renderer * Renderer, SDL_Rect Font, SDL_Rect Track, float strong, int r, int g, int b);
void SDL_RenderRemoveEdge(SDL_Renderer* Renderer, SDL_Rect * Rect);
void SDL_RenderDrawEdge(SDL_Renderer* Renderer, SDL_Rect * Rect, bool clicks);
int SDL_MAINS(void);
// -------------------- 게임 내부 함수들 ----------------------------------
void mainatitleimage(void);						//게임 메인타이틀 출력
int maintitle(void);							//게임 메인타이틀 출력및 선택
void banglist(MYSQL *cons, int j);						//게임 방 출력
int bangchose(MYSQL *cons);						//게임 방 출력및 선택
int chooseroom(int roomnum);
void waitroomtema();
void logintema(void);							//로그인 디자인
void jointema(void);							//회원가입 디자인
LOG login(int m);								//기본적인 로그인 입력
void zeroprint(int xx, int yy, int lr, int m);  //디자인

//-------------------------콘솔 함수들------------------------------------
void checkword(char*nowword, char*scanword);	//단어를 확인함
void click(int *xx, int *yy, int *lr);					//클릭함수 두번째, xx값과 yy값을 변환함

//--------------------------미니게임 숫자야구 함수들----------------------


int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함 
{

	//SDL_MAIN();
	//변수 선언
	int i, j, k, v, result;

	signalall();
	char mainchoose = 0;
	char bangchoose;
	char chooseroomcount;
	POINT pos;								//x, y좌표 표현 )pos.x, pos.y
	MYSQL *cons = mysql_init(NULL);			//mysql 초기화
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[400];						//mysql 명령어를 저장함
	char mysqlip[30] = "10.80.161.182";		//mysql ip 상희ip입니다	지금 윈도우7버전
//	char *ServerIP = "10.80.162.41";		//소켓 ip 상호ip임
	char data[1000][30] = { 0, };           //단어데이터
	char nowword[30] = { 0, };              //랜덤선택 단어
	char scanword[30] = { 0, };             //내가 친 단어
	int bangnum = 0;						//고른 방의 번호
	char serverreturn = 0;
	mysql_query(cons, "use catchmind");
	memset(&wsaData, 0, sizeof(wsaData));
	memset(&connect_sock, 0, sizeof(connect_sock));
	memset(&connect_addr, 0, sizeof(connect_addr));
	//SDL_MAINS();
	// 초기화 끝

	loadmysql(cons, mysqlip);				//mysql 서버 불러오기
	con = cons;

	disablecursor(1);
	while (1) {								//로그인 반복문
		mainchoose = maintitle();				//main 화면
		CLS;
		if (mainchoose == 1) {				//main에서 첫번째를 고르면
			ConsoleL(30, 30);
			if (sqllogin(cons) != 1)		//로그인에 성공하지 못하면 처음으로
				continue;

			ConsoleL(50, 20);
			while (1) {						//방 반복문
				gotoxy(0, 0);
				bangchoose = bangchose(cons);	//방을 고름	
				if (bangchoose == 0) {			//방만들기를 클릭하면 방만들기로 이동
					sqlmakeroom(cons);
					break;
				}
				else if (bangchoose == 1)		//방 빠른 접속 -추후추가
				{

				}
				else                            //방 선택 접속
				{
					chooseroomcount = chooseroom(bangchoose);
					if (chooseroomcount == -1)		//return -1은 해당 방이없을때
					{
						continue;
					}
					if (chooseroomcount == 0)		//return 0은 비밀번호가 틀릴때
					{
						cur(14, 1);
						printf("(비밀번호가 틀렸습니다)");
						_getch();
						CLS;
						fflush(stdin);
						continue;
					}
					if (chooseroomcount == 1)		//return 1 은 비밀번호까지 맞을때
					{

						serverreturn = Connect_Server(connectroom[bangchoose - 2].ip);		//서버 대기방 접속
						if (serverreturn == 3) {											//return값 3이면 종료 버튼
							exitallthread();
							if (lead == true)
								closesocket(listen_sock);
							else
								closesocket(connect_sock);
							continue;
						}
						if (serverreturn == 1)												//리턴값 1이면 start
						{
							CLS;
							continue;
						}
						break;
					}
				}
			}
			break;
		}
		if (mainchoose == 2)
		{
			inserttopic(cons);
			continue;
		}
	}return 0;
}


//함수 내용들		전부 최소화 Ctrl + M + O  전부 보이기 Ctrl + M + L

void inserttopic(MYSQL *cons)
{
	char topic[50];
	char query[100];
	int num;
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	while (1) {
		CLS;
		mysql_query(cons, "select * from catchmind.topic order by num");
		sql_result = mysql_store_result(cons);
		printf("내용들입니다. 엔터를 입력해주세요\n");
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			printf("%s : %s\n", sql_row[0], sql_row[1]);
			num = atoi(sql_row[0]);

		}
		num++;
		getchar();
		CLS;
		printf("\n주제를 입력해 주세요. 나가려면 p를 입력해주세요\n-> %d : ", num);
		fgets(topic, sizeof(topic), stdin);
		CHOP(topic);
		if (strcmp(topic, "p ") == 0 || topic[0] == ' ' || topic[0] == 0)
		{
			break;
		}

		sprintf(query, "insert into catchmind.topic (top) values ('%s')", topic);
		mysql_query(cons, query);
	}

}
void sqlmakeroom(MYSQL *cons) {
	while (1) {
		CLS;
		int count = 0;
		int i = 0;
		IN_ADDR addr;

		addr = GetDefaultMyIP();	//디폴트 IPv4 주소 얻어오기
		char * myip = inet_ntoa(addr);
		ROOM myroom = { 0, 0, 0 };
		disablecursor(0);
		WHITE
			printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("■                                              ■\n");
		printf("■            캐치마인드 방 만들기              ■\n");
		printf("■          내 ip :  %s \n", myip);
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("■    제목    □                              □■\n");
		printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
		printf("■  Password  □                              □■\n");
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		gotoxy(48, 3);
		printf("■");
		cur(16, 5);
		scanf("%[^\n]s", myroom.roomname);
		getchar();
		cur(16, 7);
		while (1) {


			myroom.password[i] = _getch();
			if (myroom.password[i] == 8) {
				if (i == 0) {
					myroom.password[0] = 0;
					continue;
				}
				printf("\b \b");
				myroom.password[i - 1] = 0;
				myroom.password[i--] = 0;
			}
			else if (myroom.password[i] == 13) {
				myroom.password[i] = 0;
				break;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((myroom.password[i] >= '0' && myroom.password[i] <= '9') || (myroom.password[i] >= 'a' && myroom.password[i] <= 'z') || (myroom.password[i] >= 'A' && myroom.password[i] <= 'Z'))) {
				myroom.password[i] = 0;
			}
			else {
				printf("*");
				i++;
			}
		}

		char query[100];
		sprintf(query, "insert into catchmind.room (ip, name, password) values ('%s', '%s', '%s')", myip, myroom.roomname, myroom.password);
		if (!(mysql_query(cons, query)))
		{
			cur(10, 1);
			printf("(알수 없는 오류. 중복이 안되게 만들어 주세요)");
		}
		disablecursor(1);
		CLS;
		printf("방 제작중....");
		threads[5] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)makeroom, &count, 0, 0);
		while (1) {
			if (count == 1)
			{
				lead = true;
				CLS;
				Connect_Server(myip);
				return;
			}
			Sleep(1000);
		}
		break;
	}
	disablecursor(0);
}
void waitroomtema() {
	WHITE
		gotoxy(0, 3);
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■   [ STATUS ] :                                                                             ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■   [ STATUS ] :                                                                             ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■   [ STATUS ] :                                                                             ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n"); //92
	printf("      ■   [ STATUS ] :                                                                             ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■                                                                                            ■\n");
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("      ■                ■                                                        ■                ■\n");		// 4, 40		//11, 40		//42, 40		49, 40
	printf("      ■     ready      ■                                                        ■     exit       ■\n");
	printf("      ■                ■                                                        ■                ■\n");		// 4, 42		//11, 42		//42, 42		49, 42
	printf("      ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
}
int waitroom(void)
{
	disablecursor(1);
	int xx = 0, yy = 0, lr = 0;
	int togl = -1;
	ConsoleL(54, 50);
	char query[100] = { 0, };
	int mode = 0;
	if (lead == true)
		signalmode = 2;
	else
		signalmode = 1;
	waitroomtema();
	while (1) { //받아온 데이터 처리


		cur(11, 5);
		printf("[ %s ]             ", friendname[0]);
		if (status[0] == 0) {

			GRAY
				gotoxy(24, 7);
			printf("DISCONNECTED            ");
			WHITE
		}
		else if (status[0] == 1) {
			HIGH_GREEN
				gotoxy(24, 7);
			printf("JOIN               ");
			WHITE
		}
		if (status[0] == 2) {
			SKY_BLUE
				gotoxy(24, 7);
			printf("READY             ");
			WHITE
		}


		cur(11, 14);
		printf("[ %s ]             ", friendname[1]);
		if (status[1] == 0) {
			GRAY

				gotoxy(24, 16);
			printf("DISCONNECTED                  ");
			WHITE
		}
		else if (status[1] == 1) {
			HIGH_GREEN

				gotoxy(24, 16);
			printf("JOIN                 ");
			WHITE
		}
		if (status[1] == 2) {
			SKY_BLUE
				gotoxy(24, 16);
			printf("READY                ");
			WHITE
		}

		cur(11, 23);
		printf("[ %s ]          ", friendname[2]);
		if (status[2] == 0) {
			GRAY
				gotoxy(24, 25);
			printf("DISCONNECTED              ");
			WHITE
		}
		else if (status[2] == 1) {
			HIGH_GREEN
				gotoxy(24, 25);
			printf("JOIN                  ");
			WHITE
		}
		if (status[2] == 2) {
			SKY_BLUE
				gotoxy(24, 25);
			printf("READY              ");
			WHITE
		}

		cur(11, 32);
		printf("[ %s ]             ", friendname[3]);
		if (status[3] == 0) {
			GRAY
				gotoxy(24, 34);
			printf("DISCONNECTED        ");
			WHITE
		}
		else if (status[3] == 1) {
			HIGH_GREEN
				gotoxy(24, 34);
			printf("JOIN                 ");
			WHITE
		}
		if (status[3] == 2) {
			SKY_BLUE
				gotoxy(24, 34);
			printf("READY                ");
			WHITE
		}

		gotoxy(51, 41);
		if (!(lead == true && status[0] != 1 && status[1] != 1 && status[2] != 1 && status[3] != 1))
			printf("          ");



		if (status[0] == 10)
		{
			status[0] = 2;
			CLS;
			printf("게임을 시작합니다.");
			if (lead == true)
			{
				signalmode = 3;
				sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
				mysql_query(con, query);
				
			}
			for (int i = 0; i < 3; i++)
			{
				cur(0, 1);
				printf("%d초후 시작", 3 - i);
			}
			SDL_MAINS();
			return 1;
		}
		POINT a;
		GetCursorPos(&a);
		SetCursorPos(a.x, a.y);
		click(&xx, &yy, &lr);
		if (lr == 1) {
			if (lead == true && status[0] != 1 && status[1] != 1 && status[2] != 1 && status[3] != 1) {
				if (xx > 13 && xx < 41 && yy < 43 && yy > 39) {
					send(connect_sock, "game start", 45, 0);
					xx = 0; yy = 0;
				}

			}
			if (xx > 3 && xx < 12 && yy < 43 && yy > 39) {
				if (mode == 0 && togl == -1)
					togl *= -1;
				else if (mode == 1 && togl == 1)
					togl *= -1;
				else if (mode == 0) {
					mode = 1;
					send(connect_sock, "player ready", 45, 0);
				}
				else if (mode == 1) {
					mode = 0;
					send(connect_sock, "player not ready", 45, 0);
				}

				xx = 0;
				yy = 0;
			}
			if (xx > 42 && xx < 49 && yy < 43 && yy > 39) {
				send(connect_sock, "player exit", 45, 0);
				return 3;
			}
			xx = 0;
			yy = 0;
		}
		else if (lr == 0) {
			if (lead == true && status[0] != 1 && status[1] != 1 && status[2] != 1 && status[3] != 1) {
				if (xx > 13 && xx < 41 && yy < 43 && yy > 39) {
					gotoxy(51, 41);
					HIGH_GREEN printf("start!");
				}
				else {
					gotoxy(51, 41);
					WHITE printf("start!");
				}

			}
			else if (xx > 3 && xx < 12 && yy < 43 && yy > 39) {
				gotoxy(13, 41);
				HIGH_GREEN printf("ready");
				gotoxy(89, 41);
				WHITE printf("exit");
			}
			else if (xx > 42 && xx < 49 && yy < 43 && yy > 39) {
				gotoxy(89, 41);
				HIGH_GREEN printf("exit");
				gotoxy(13, 41);
				WHITE printf("ready");
			}
			else {
				gotoxy(13, 41);
				WHITE printf("ready");
				gotoxy(89, 41);
				WHITE printf("exit");
			}
			WHITE
		}
	}
	Sleep(50);

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
void zeroprint(int xx, int yy, int lr, int m) {
	disablecursor(1);
	if (9 <= yy && yy <= 11) {
		if (1 <= xx && xx <= 7) {
			gotoxy(3, 10);
			HIGH_GREEN printf("개발자사이트");
			WHITE

				if (m == 2) {
					gotoxy(22, 10);
					printf("나가기");
				}
				else {
					gotoxy(21, 10);
					printf("회원가입");
				}
				gotoxy(38, 10);
				printf("초기화");
		}
		else if (9 <= xx && xx <= 15) {

			HIGH_GREEN

				if (m == 2) {
					gotoxy(22, 10);
					printf("나가기");
				}
				else {
					gotoxy(21, 10);
					printf("회원가입");
				}
				WHITE
					gotoxy(3, 10);
				printf("개발자사이트");
				gotoxy(38, 10);
				printf("초기화");
		}
		else if (17 <= xx && xx <= 23) {
			gotoxy(38, 10);
			HIGH_GREEN printf("초기화");
			WHITE
				gotoxy(3, 10);
			printf("개발자사이트");
			if (m == 2) {
				gotoxy(22, 10);
				printf("나가기");
			}
			else {
				gotoxy(21, 10);
				printf("회원가입");
			}
		}
	}
	else {
		WHITE
			gotoxy(3, 10);
		printf("개발자사이트");
		if (m == 2) {
			gotoxy(22, 10);
			printf("나가기");
		}
		else {
			gotoxy(21, 10);
			printf("회원가입");
		}
		gotoxy(38, 10);
		printf("초기화");
	}
}
LOG login(int m) { // 1이면 로그인 2이면 회원가입 필수!!
				   //오류 없는 코드니까 회원가입이랑 로그인에 잘 적으시길
	int to = -1;
	int b = 0;
	int n = 0;
restart:

	POINT a;
	gotoxy(0, 0);
	if (m == 1)
		logintema();

	LOG user = { 0, 0, 0 };
	int i = 0, j = 0;
	int cnt = 0;
	int xx = 0, yy = 0, lr = 0;
	/*닉네임 생성*/
	disablecursor(0);
	if (m == 2) {
		ConsoleL(30, 30);
		jointema();
		gotoxy(9, 13);
		printf("닉네임 적을때는 마우스 사용 불가능");
		gotoxy(16, 3);
		fgets(user.name, 13, stdin);
		CHOP(user.name);
		gotoxy(9, 13);
		printf("                                       ");
	}
	gotoxy(16, 5);
	while (1) {
		if (_kbhit()) {

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
			else if ((user.id[i] == 9 || user.id[i] == 13) && i > 3) {
				user.id[i] = 0;
				break;
			}
			else if (user.id[i] == 13) {
				user.id[i] = 0;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((user.id[i] >= '0' && user.id[i] <= '9') || (user.id[i] >= 'a' && user.id[i] <= 'z') || (user.id[i] >= 'A' && user.id[i] <= 'Z'))) {
				user.id[i] = 0;
			}
			else
				putchar(user.id[i++]);
		}
		else {
			GetCursorPos(&a);
			SetCursorPos(a.x, a.y);
			click(&xx, &yy, &lr);
			//gotoxy(20,20);
			//printf("%3d %3d", xx, yy); //login 19~23 5~7      개발자 사이트 1~7 9~11    회원가입 9~15      초기화 17~23
			if (lr == 1) {
				if (9 <= yy && yy <= 11) {
					if (1 <= xx && xx <= 7 && to == 1) {
						system("start https://blog.naver.com/dgsw102");
						system("start https://blog.naver.com/soohan530");
						to *= -1;
					}
					else if (1 <= xx && xx <= 7 && to == -1) {
						to *= -1;
					}
					else if (n < 1)
						n++;
					else if (9 <= xx && xx <= 15) {
						for (i = 0; i <= 15; i++) {
							user.id[i] = 0;
							user.pass[i] = 0;
							user.name[i] = 0;
						}
						return user; //함수를 끝내고 제로값이 도착하면 알아서 걸러내길
					}
					else if (17 <= xx && xx <= 23) {
						goto restart;
					}
				}
			}
			else if (lr == 0) {
				zeroprint(xx, yy, lr, m);
			}
			gotoxy(16 + i, 5);
			disablecursor(0);
		}

		Sleep(20);
	}
	i = 0;

	/*비밀번호 암호화 구현*/
	gotoxy(16, 7);
	while (1) {

		if (_kbhit()) {
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
			else if (i >= 15) {
				continue;
			}
			else if (!((user.pass[i] >= '0' && user.pass[i] <= '9') || (user.pass[i] >= 'a' && user.pass[i] <= 'z') || (user.pass[i] >= 'A' && user.pass[i] <= 'Z'))) {
				user.pass[i] = 0;
			}
			else {
				printf("*");
				i++;
			}
		}
		else {
			GetCursorPos(&a);
			SetCursorPos(a.x, a.y);
			click(&xx, &yy, &lr);

			if (lr == 1) {
				if (9 <= yy && yy <= 11) {
					if (1 <= xx && xx <= 7 && to == 1) {
						system("start https://blog.naver.com/dgsw102");
						system("start https://blog.naver.com/soohan530");
						to *= -1;
					}
					else if (1 <= xx && xx <= 7 && to == -1) {
						to *= -1;
					}
					else if (9 <= xx && xx <= 15) {
						for (i = 0; i <= 15; i++) {
							user.id[i] = 0;
							user.pass[i] = 0;
							user.name[i] = 0;
						}
						break;  //함수를 끝내고 제로값이 도착하면 알아서 걸러내길
					}
					else if (17 <= xx && xx <= 23) {
						goto restart;
					}
				}
				else if (m == 1 && 19 <= xx && xx <= 23 && 5 <= yy && yy <= 7 && b == 0 && i > 3) {
					b++;
				}
				else if (m == 2 && 19 <= xx && xx <= 23 && 3 <= yy && yy <= 7 && b == 0 && i > 3) {
					b++;
				}
				else if (m == 1 && 19 <= xx && xx <= 23 && 5 <= yy && yy <= 7 && b == 1 && i > 3) {
					break;
				}
				else if (m == 2 && 19 <= xx && xx <= 23 && 3 <= yy && yy <= 7 && b == 1 && i > 3) {
					break;
				}
			}
			else if (lr == 0) {
				if (9 <= yy && yy <= 11)
					zeroprint(xx, yy, lr, m);
				else if (m == 1 && 19 <= xx && xx <= 23 && 5 <= yy && yy <= 7) {
					HIGH_GREEN
						gotoxy(41, 6);
					printf("login");
					WHITE
						gotoxy(3, 10);
					printf("개발자사이트");

					gotoxy(21, 10);
					printf("회원가입");

					gotoxy(38, 10);
					printf("초기화");
				}
				else if (m == 2 && 19 <= xx && xx <= 23 && 3 <= yy && yy <= 7) {
					HIGH_GREEN
						gotoxy(39, 5);
					printf("회원가입");
					WHITE
						gotoxy(3, 10);
					printf("개발자사이트");
					gotoxy(22, 10);
					printf("나가기");
					gotoxy(38, 10);
					printf("초기화");
				}
				else {
					WHITE
						gotoxy(3, 10);
					printf("개발자사이트");
					if (m == 2) {
						gotoxy(22, 10);
						printf("나가기");
						gotoxy(39, 5);
						printf("회원가입");
					}
					else {
						gotoxy(21, 10);
						printf("회원가입");
						gotoxy(41, 6);
						printf("login");
					}
					gotoxy(38, 10);
					printf("초기화");

				}
			}
			gotoxy(16 + i, 7);
			disablecursor(0);
		}
		Sleep(20);
	}




	disablecursor(1);
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
		printf("새로운 ip를 설정해 주세요.\n->");
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
	POINT pos;
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
				pos.x = rec.Event.MouseEvent.dwMousePosition.X;
				pos.y = rec.Event.MouseEvent.dwMousePosition.Y;
				return pos;
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
	system("cls");
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);
}
int Connect_Server(char *ServerIP) { //서버 연결 해주는 함수
	char query[100];
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock변수에 소켓 할당
	connect_addr.sin_family = AF_INET;				//연결할 서버의 주소 설정
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //서버 IP
	connect_addr.sin_port = htons(5555);					 //서버 포트
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //서버에 연결
		ErrorHandling("connect() error");
	threads[0] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, NULL, 0, NULL); //서버에서 데이터를 받아오는 쓰레드 시작
	CLS;
	sprintf(query, "player   connect %s", username);
	send(connect_sock, query, 45, 0);
	Sleep(200);

	return waitroom();
}
void recieve(void) { //서버에서 데이터 받아오는 쓰레드용 함수
	char message[50] = { 0, };
	while (1) {

		if (recv(connect_sock, message, 45, 0) > 0) { //서버에서 데이터를 받아와 message변수에 저장
			if (strncmp("player 1 connect", message, 15) == 0) {
				sscanf(message, "player 1 connect %s", friendname[0]);
				status[0] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 connect", message, 15) == 0) {
				sscanf(message, "player 2 connect %s", friendname[1]);
				status[1] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 connect", message, 15) == 0) {
				sscanf(message, "player 3 connect %s", friendname[2]);
				status[2] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 connect", message, 15) == 0) {
				sscanf(message, "player 4 connect %s", friendname[3]);
				status[3] = 1;
				ZeroMemory(message, sizeof(message));
			}
			if (strncmp("player 1 ready", message, 13) == 0) {
				sscanf(message, "player 1 ready %s", friendname[0]);
				status[0] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 ready", message, 13) == 0) {
				sscanf(message, "player 2 ready %s", friendname[1]);
				status[1] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 ready", message, 13) == 0) {
				sscanf(message, "player 3 ready %s", friendname[2]);
				status[2] = 2;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 ready", message, 13) == 0) {
				sscanf(message, "player 4 ready %s", friendname[3]);
				status[3] = 2;
				ZeroMemory(message, sizeof(message));
			}
			if (strncmp("player 1 not ready", message, 16) == 0) {
				sscanf(message, "player 1 not ready %s", friendname[0]);
				status[0] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 not ready", message, 16) == 0) {
				sscanf(message, "player 2 not ready %s", friendname[1]);
				status[1] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 not ready", message, 16) == 0) {
				sscanf(message, "player 3 not ready %s", friendname[2]);
				status[2] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 not ready", message, 16) == 0) {
				sscanf(message, "player 4 not ready %s", friendname[3]);
				status[3] = 1;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 1 exit", message, 12) == 0) {
				status[0] = 0;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 exit", message, 12) == 0) {
				status[1] = 0;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 exit", message, 12) == 0) {
				status[2] = 0;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 exit", message, 12) == 0) {
				status[3] = 0;
				ZeroMemory(message, sizeof(message)); 
			}
			else if (strcmp("game start", message) == 0) {
				status[0] = 10;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp(message, "0 ", 2) == 0 || strncmp(message, "1 ", 2) == 0)
			{
				SDLCLOCK++;
				cur(0, 0);
			//	printf("%s                    ", message);
				strcpy(clientcatchmind, message);
			}
			else if (strcmp(message, "SDLCLEAR") == 0)
			{
				SDLCLOCK++;
				SDL_Clear = true;
			}
		}
		//	Sleep(100);
	}

}
int sqllogin(MYSQL *cons) {
	char query[100];
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	LOG user;								//사용자 정보 구조체
	while (1) {

		int check = 0;

		user = login(1);							//login 함수를 사용
		if (user.id[0] == 0)
		{
			fflush(stdin);
			check = sqlsignup(cons);
			gotoxy(2, 3);
			if (check == 1)
				printf("               (회원가입 성공)           ");
			else if (check == -1)
				printf("             (아이디가 중복됩니다)              ");
			else
				printf("               (회원가입 실패)          ");
		}
		else {
			sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id를 DB에서 찾음
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (mysql_fetch_row(sql_result) == NULL)									//해댱 id가 없으면 
			{
				gotoxy(2, 3);
				printf("           아이디가 존재하지 않습니다     ");

			}
			else {

				sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass);
				mysql_query(cons, query);	//password는 DB에 암호화되어 있어서 값을 비교할때도 서로 암호화해서 비교를함
				sql_result = mysql_store_result(cons);



				if ((sql_row = mysql_fetch_row(sql_result)) == NULL)
				{
					gotoxy(2, 3);
					printf("              비밀번호가 틀렸습니다        ");
				}
				else {
					strcpy(username, sql_row[1]);
					sprintf(query, "title %s님 히트마인드에 오신것을 환영합니다!", username);
					system(query);
					return 1; //로그인 성공
				}
			}
		}


	}


}
int sqlsignup(MYSQL *cons) {
	LOG user;
	char query3[100];
	char query2[100];
	user = login(2);
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수


	if (user.name[0] == 0)
		return 0;

	sprintf(query2, "select * from catchmind.login where id = '%s'", user.id);	//id를 DB에서 찾음
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -1;
	}

	sprintf(query3, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);

	if (!(mysql_query(cons, query3)))											//		 password는 mysql에서 지원하는 암호화 형식임.
		return 1; //성공
	else
		return 0; //실패

}
void mainatitleimage(void) {
	WHITE
		gotoxy(6, 3);
	printf("        ■              ■■■■■      ■■■■■  ■        ■■■    ■      ■■■■■                                           ■■■"); gotoxy(6, 4);
	printf("    ■■■■■  ■      ■              ■      ■  ■      ■      ■  ■      ■                                                 ■      ■"); gotoxy(6, 5);
	printf("                ■      ■■■■■      ■      ■  ■      ■      ■  ■      ■              ■      ■   ■    ■    ■      ■"); gotoxy(6, 6);
	printf("      ■■■    ■      ■              ■      ■  ■■      ■■■    ■      ■■■■■      ■      ■       ■■■  ■      ■"); gotoxy(6, 7);
	printf("    ■      ■  ■      ■■■■■      ■      ■  ■                                          ■  ■  ■   ■    ■    ■■    ■"); gotoxy(6, 8);
	printf("    ■      ■  ■                      ■      ■  ■        ■                                ■  ■  ■   ■    ■    ■  ■    ■      ■"); gotoxy(6, 9);
	printf("      ■■■    ■    ■■■■■■■    ■■■■■  ■        ■■■■■■    ■■■■■■■      ■  ■     ■    ■    ■  ■      ■■■");


	gotoxy(12, 20);
	printf("■■■■■■■■■                      ■■■■■■■■■                      ■■■■■■■■■"); gotoxy(12, 21);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 22);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 23);
	printf("■  게임 시작   ■                      ■   주제 추가  ■                      ■              ■"); gotoxy(12, 24);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 25);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 26);
	printf("■■■■■■■■■                      ■■■■■■■■■                      ■■■■■■■■■");
}
int maintitle(void) { //게임 메인타이틀 출력
	ConsoleL(100, 60);
	disablecursor(true);
	int xx = 0, yy = 0, lr = 0;
	mainatitleimage();
	while (1) {
		printf("%3d %3d\n", xx, yy);
		WHITE
			click(&xx, &yy, &lr);

		if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 0) {
			gotoxy(16, 23);
			HIGH_GREEN printf("게임 시작");
		}
		else if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 1)
			return 1;
		else if (27 <= xx && xx <= 33 && 21 <= yy && yy <= 25 && lr == 0)
		{
			gotoxy(56, 23);
			HIGH_GREEN printf("주제 추가");
		}
		else if (27 <= xx && xx <= 33 && 21 <= yy && yy <= 25 && lr == 1)
		{
			return 2;
		}
		else if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 1)
			return 1;
		else {
			gotoxy(16, 23);
			WHITE printf("게임 시작");
			gotoxy(56, 23);
			WHITE printf("주제 추가");
		}

		gotoxy(0, 0);
	}
	CLS;
}
void click(int *xx, int *yy, int *lr) {//마우스에서 2를 나눈값을 받는다

	HANDLE       hIn, hOut;
	DWORD        dwNOER;
	INPUT_RECORD rec;


	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER); // 콘솔창 입력을 받아들임.
	if (rec.EventType == MOUSE_EVENT) {// 마우스 이벤트일 경우
		int mouse_x = rec.Event.MouseEvent.dwMousePosition.X; // X값 받아옴
		int mouse_y = rec.Event.MouseEvent.dwMousePosition.Y; // Y값 받아옴

		*xx = mouse_x / 2;
		*yy = mouse_y;

		if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // 좌측 버튼이 클릭되었을 경우
			*lr = 1;
			Sleep(100);
		}
		else {
			*lr = 0;
		}
	}


}
void bangtema() {
	WHITE
		printf("\n"); //좌표값때문에 한칸 밀어냄
	printf("                □□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n"); //방만들기 9 ~ 22 , 2
	printf("                □          방만들기          □          빠른시작          □\n"); //빠른시작 24 ~ 37 , 2
	printf("                □□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("                □                          방목록                          □\n"); //방목록 9 ~ 37, 4
	printf("                ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("                ■                            ■                            ■\n"); //방1 9 ~ 22 , 6 ~ 8 
	printf("                ■                            ■                            ■\n"); //방2 24 ~ 37 , 6 ~ 8
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

}
void banglist(MYSQL *cons, int j) {
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	short i = 0;
	mysql_query(cons, "select ip, name, password from catchmind.room");
	sql_result = mysql_store_result(cons);

	memset(connectroom, 0, sizeof(connectroom));

	while (i < 6) {

		if (i % 2 == 0)
			cur(25, 6 + (i * 2));
		else
			cur(55, 6 + ((i / 2) * 4));
		printf("                 ");

		if (i % 2 == 0)
			cur(25, 7 + (i * 2));
		else
			cur(55, 7 + ((i / 2) * 4));
		printf("                 ");

		i++;
	}
	i = 0;
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{

		if (i % 2 == 0)
			cur(25, 6 + (i * 2));
		else
			cur(55, 6 + ((i / 2) * 4));

		if (j == i) {
			HIGH_GREEN printf("%s", sql_row[0]);
		}
		else {
			WHITE printf("%s", sql_row[0]);
		}
		/*for (short j = 0; sql_row[1][j] != 0; j++) {
			if (i % 2 == 0 && j < 10)
				cur(25 + j, 7 + (i * 2));
			else if (i % 2 == 0)
				cur(25 + j - 10, 8 + (i * 2));
			else if (j < 10)
				cur(55 + j, 7 + (i / 2) * 4);
			else
				cur(55 + j - 10, 8 + (i / 2) * 4);
			printf("%c", sql_row[1][j]);
		}*/
		if (i % 2 == 0)
			cur(25, 7 + (i * 2));
		else
			cur(55, 7 + ((i / 2) * 4));

		if (j == i) {
			HIGH_GREEN printf("%-7s", sql_row[1]);
		}
		else {
			WHITE printf("%-7s", sql_row[1]);
		}

		strcpy(connectroom[i].ip, sql_row[0]);
		strcpy(connectroom[i].roomname, sql_row[1]);
		strcpy(connectroom[i].password, sql_row[2]);
		i++;

	}

	i = 0;

}
int bangchose(MYSQL *cons) {

	int xx = 0, yy = 0, lr = 0;
	int j = 0;
	POINT a;
	bangtema();
	j = -1;
	while (1) {
		banglist(cons, j);
		gotoxy(0, 0);
		printf("%3d %3d\n", xx, yy);

		GetCursorPos(&a);
		SetCursorPos(a.x, a.y);
		click(&xx, &yy, &lr);

		if (lr == 1) {
			if (9 <= xx && xx <= 22 && 2 == yy)			//방만들기
				return 0;
			else if (24 <= xx && xx <= 37 && 2 == yy)		//빠른시작
				return 1;
			else if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)	//방 1
				return 2;
			else if (24 <= xx && xx <= 37 && 6 <= yy && yy <= 8)	//방 2
				return 3;
			else if (9 <= xx && xx <= 22 && 10 <= yy && yy <= 12)	//방 3
				return 4;
			else if (24 <= xx && xx <= 37 && 10 <= yy && yy <= 12)	//방 4
				return 5;
			else if (9 <= xx && xx <= 22 && 14 <= yy && yy <= 16)	//방 5
				return 6;
			else if (24 <= xx && xx <= 37 && 14 <= yy && yy <= 16)	//방 6
				return 7;
		}
		else if (lr == 0) {
			if (9 <= xx && xx <= 22 && 2 == yy) {			//방만들기
				gotoxy(28, 2);
				HIGH_GREEN printf("방만들기");
				WHITE
					gotoxy(58, 2);
				printf("빠른시작");
			}

			else if (24 <= xx && xx <= 37 && 2 == yy) {		//빠른시작
				gotoxy(58, 2);
				HIGH_GREEN printf("빠른시작");
				WHITE
					gotoxy(28, 2);
				printf("방만들기");
			}

			else if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)	//방 1
				j = 0;
			else if (24 <= xx && xx <= 37 && 6 <= yy && yy <= 8)	//방 2
				j = 1;
			else if (9 <= xx && xx <= 22 && 10 <= yy && yy <= 12)	//방 3
				j = 2;
			else if (24 <= xx && xx <= 37 && 10 <= yy && yy <= 12)	//방 4
				j = 3;
			else if (9 <= xx && xx <= 22 && 14 <= yy && yy <= 16)	//방 5
				j = 4;
			else if (24 <= xx && xx <= 37 && 14 <= yy && yy <= 16)	//방 6
				j = 5;
			else {
				WHITE
					j = -1;
				gotoxy(28, 2);
				printf("방만들기");
				gotoxy(58, 2);
				printf("빠른시작");
			}

		}
		Sleep(50);

	}

}
int chooseroom(int roomnum) {
	roomnum -= 2;
	char roompassword[30] = { 0, };
	int i = 0;
	if (connectroom[roomnum].ip[0] == 0)
		return -1;
	CLS;
	WHITE
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■                                              ■\n");
	printf("■              캐치마인드 방 접속              ■\n");
	printf("■          접속 ip :  %s\n", connectroom[roomnum].ip);
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■  방 제목   □                              □■\n");			// x = 17 y = 5
	printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
	printf("■  Password  □                              □■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	gotoxy(48, 3);
	printf("■");
	cur(17, 5);
	printf("%s", connectroom[roomnum].roomname);
	cur(17, 7);
	disablecursor(0);
	while (1) {

		roompassword[i] = _getch();
		if (roompassword[i] == 8) {
			if (i == 0) {
				roompassword[0] = 0;
				continue;
			}
			printf("\b \b");
			roompassword[i - 1] = 0;
			roompassword[i--] = 0;
		}
		else if (roompassword[i] == 13) {
			roompassword[i] = 0;
			break;
		}
		else if (i >= 15) {
			continue;
		}
		else if (!((roompassword[i] >= '0' && roompassword[i] <= '9') || (roompassword[i] >= 'a' && roompassword[i] <= 'z') || (roompassword[i] >= 'A' && roompassword[i] <= 'Z'))) {
			roompassword[i] = 0;
		}
		else {
			printf("*");
			i++;
		}
	}
	disablecursor(1);
	if (!(strcmp(connectroom[roomnum].password, roompassword)))
	{
		return 1;
	}
	else
		return 0;
}
void logintema(void) {
	WHITE
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■                                              ■\n");
	printf("■            히트마인드 서버에 로그인          ■\n");
	printf("■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■     ID     □                    □          ■\n");
	printf("■□□□□□□□□□□□□□□□□□□   login  ■\n");
	printf("■  Password  □                    □          ■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■              ■              ■              ■\n");
	printf("■ 개발자사이트 ■   회원가입   ■    초기화    ■\n");
	printf("■              ■              ■              ■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	gotoxy(48, 3);
	printf("■");
}
void jointema(void) {
	WHITE
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■  회원가입 : 닉네임은 공백 특수문자 안됩니다  ■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■    닉네임  □                    □          ■\n");
	printf("■□□□□□□□□□□□□□□□□□□          ■\n");
	printf("■     ID     □                    □ 회원가입 ■\n");
	printf("■□□□□□□□□□□□□□□□□□□          ■\n");
	printf("■  Password  □                    □          ■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■              ■              ■              ■\n");
	printf("■ 개발자사이트 ■    나가기    ■    초기화    ■\n");
	printf("■              ■              ■              ■\n");
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
}
void sendall(char *message) {
	if (Sconnect_sock[0] != 0)
		send(Sconnect_sock[0], message, 45, 0);
	//	printf("Client 1 <- Server : %s\n", message);
	if (Sconnect_sock[1] != 0) {
		send(Sconnect_sock[1], message, 45, 0);
		//		printf("Client 2 <- Server : %s\n", message);
	}
	if (Sconnect_sock[2] != 0) {
		send(Sconnect_sock[2], message, 45, 0);
		//		printf("Client 3 <- Server : %s\n", message);
	}
	if (Sconnect_sock[3] != 0) {
		send(Sconnect_sock[3], message, 45, 0);
		//		printf("Client 4 <- Server : %s\n", message);
	}
	ZeroMemory(message, sizeof(message));

}
void processclnt(void) {

}
void Clnt_1(int v)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (i == v)
			continue;
		if (Sconnect_sock[i] != 0)
			send(Sconnect_sock[v], querys[i], 45, 0);
	}
	/*if (Sconnect_sock[1] != 0)
		send(Sconnect_sock[v], querys[1], 40, 0);
	if (Sconnect_sock[2] != 0)
		send(Sconnect_sock[v], querys[2], 40, 0);
	if (Sconnect_sock[3] != 0)
		send(Sconnect_sock[v], querys[3], 40, 0);
	if (Sconnect_sock[0] != 0)
		send(Sconnect_sock[v], querys[0], 40, 0);*/

	char message[100];
	while (1) {
		if (recv(Sconnect_sock[v], message, 45, 0) > 0) {
			if (strncmp(message, "player   connect", 16) == 0) {
				message[7] = v + '0' + 1;
				strcpy(querys[v], message);

			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				sprintf(message, "player %d ready %s", v + 1, friendname[i]);
				strcpy(querys[v], message);
			}
			else if (strcmp(message, "player not ready") == 0) {
				ZeroMemory(message, sizeof(message));
				sprintf(message, "player %d not ready %s", v + 1, friendname[i]);
				strcpy(querys[v], message);
			}
			else if (strcmp(message, "exit") == 0)
			{
				ZeroMemory(message, sizeof(message));
				sprintf(message, "player %d exit", v + 1);
				closesocket(Sconnect_sock[v]);
				SOCKETCOUNT = v;
				Sconnect_sock[v] = 0;
				strcpy(querys[v], message);
			}
			else if (strcmp(message, "game start") == 0)
			{
				cur(0, 0);
				printf("%s", message);
				sendall(message);
			}

			sendall(message);
			ZeroMemory(message, sizeof(message));
		}
		//	Sleep(100);
	}
}
void makeroom(int *count) {
	int i = 0;
	char message[100];
	IN_ADDR serverip = GetDefaultMyIP();
	listen_sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성 후 소켓에 대한 정보를 listen_sock변수에 대입					
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling("socket() error");
	printf("소켓 생성 완료!\n");
	memset(&listen_addr, 0, sizeof(listen_addr)); // 서버의 주소 초기화
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 서버의 주소를 내 주소로 (아마도)
	listen_addr.sin_port = htons(5555); // 서버 포트 
	printf("주소 지정 완료!\n");
	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) // 지금까지 설정한 주소를 listen_sock에 bind()로 지정
		ErrorHandling("bind() error");
	printf("bind() 완료!\n");
	if (listen(listen_sock, 5) == SOCKET_ERROR)	// 클라이언트가 접속할때 까지 기다림
		ErrorHandling("listen() error");
	printf("listen() 완료!\n");
	sockaddr_in_size = sizeof(connect_addr);
	*count = 1;
	while (1) {
		if (Sconnect_sock[SOCKETCOUNT] == 0) {
			Sconnect_sock[SOCKETCOUNT] = accept(listen_sock, (SOCKADDR*)&connect_addr, &sockaddr_in_size); // 접속하면 accept() 해줌
			threads[SOCKETCOUNT + 1] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_1, (int *)SOCKETCOUNT, 0, NULL);
		}
		else {
			SOCKETCOUNT++;
			if (SOCKETCOUNT == 4)
				SOCKETCOUNT = 0;
			continue;
		}
		/*else {
			SOCKETCOUNT++;
			if (SOCKETCOUNT == 4)
				SOCKETCOUNT = 0;
			continue;
		}
		if (Sconnect_sock[SOCKETCOUNT] != 0) {
			switch (SOCKETCOUNT) {
			case 0:threads[1] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_1, NULL, 0, NULL); break;
			case 1:threads[2] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_2, NULL, 0, NULL); break;
			case 2:threads[3] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_3, NULL, 0, NULL); break;
			case 3:threads[4] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)Clnt_4, NULL, 0, NULL); break;
			default: break;
			}
		}*/

		Sleep(100);
	}

}
IN_ADDR GetDefaultMyIP()
{

	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//호스트 이름 얻어오기
	{
		return addr;
	}
	HOSTENT *ptr = gethostbyname(localhostname);//호스트 엔트리 얻어오기
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 주소 타입일 때
		{
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);//메모리 복사
			break;//반복문 탈출
		}
		ptr++;
	}
	return addr;
}
bool exitallthread(void)
{
	for (int i = 0; i < 10; i++)
	{
		if (threads[i] != 0)
			ExitThread(threads[i]);

	}
	return true;
}
void signalall(void)
{
	signal(SIGINT, (_crt_signal_t)exitsignal);
	signal(SIGBREAK, (_crt_signal_t)exitsignal);
	//	signal(SIGILL, (_crt_signal_t)exitsignal);
	//	signal(SIGFPE, (_crt_signal_t)exitsignal);
	//	signal(SIGSEGV, (_crt_signal_t)exitsignal);
}
void exitsignal(void)
{
	char query[100];
	if (signalmode == 1)
	{
		send(connect_sock, "exit", 40, 0);
	}
	if (signalmode == 2)
	{
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(con, query);

	}
	mysql_close(con);

}
void gotoxy(short x, short y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void cur(short x, short y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void SDL_ErrorLog(const char * msg) {//에러코드 출력 함수
	printf("%s Error: %s\n", msg, SDL_GetError());
	return;
}
void IMG_ErrorLog(const char * msg) {//에러코드 출력 함수
	printf("%s Error: %s\n", msg, IMG_GetError());
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
void SDL_RenderDrawEdge(SDL_Renderer* Renderer, SDL_Rect * Rect, bool clicks) {
	if (clicks == true)
		SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 64);// 클릭했을 경우는 더진한 파랑
	else
		SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 8);// 마우스가 올려져 있을 경우는 좀 연한 파랑
	SDL_Rect Edge = { 0 };
	Edge.x = Rect->x - 10;
	Edge.y = Rect->y - 10;
	Edge.w = Rect->w + 20;
	Edge.h = Rect->h + 20;
	SDL_RenderDrawRect(Renderer, &Edge);// 파란 테두리를 그림
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);// 알파값을 이용해 투명하게 칠하는 것을 가능하게 함
	SDL_RenderFillRect(Renderer, &Edge);// 조금 투명한 파란 사각형 그림
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);// 투명하게 칠하는 걸 불가능하게 함
}
void SDL_RenderRemoveEdge(SDL_Renderer* Renderer, SDL_Rect * Rect) {
	SDL_Rect Edge = { 0 };
	Edge.x = Rect->x - 10;
	Edge.y = Rect->y - 10;
	Edge.w = Rect->w + 20;
	Edge.h = Rect->h + 20;
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 하얀색
	SDL_RenderFillRect(Renderer, &Edge);// 조금 투명한 파란 사각형 그림
}
void SDL_FontUpdate(SDL_Renderer * Renderer, SDL_Rect* Font, SDL_Rect Track, float strong, int r, int g, int b) {
	if (clicks.pencil == true) {
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 흰색으로 정함
		SDL_RenderFillRect(Renderer, Font);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
		Font->x = Track.x + 35 - strong / 2;// 왼쪽 꼭짓점 좌표를 다시설정
		Font->y = Track.y - 50 - strong / 2;// 오른쪽 꼭짓점 좌표를 다시설정
		Font->h = Font->w = strong;// 굵기 다시설정
		SDL_SetRenderDrawColor(Renderer, r, g, b, 0);//색깔을 얻어옴
		SDL_RenderFillRect(Renderer, Font);// 폰트를 출력함
		if (strong > 5) {// if문에 굵기가 5초과인 경우만 이라고 한이유는 굵기가 5이하이면 테두리때문에 검은색으로 보일수도 있기 때문
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);// 검은색으로 설정
			SDL_RenderDrawRect(Renderer, Font);// 테두리를 그려줌
		}
	}
	else if (clicks.eraser == true) {
		strong *= 80 / 50.0;
		int l;
		int x1 = sin(3.14 / 180 * 0)*strong / 2, y1 = cos(3.14 / 180 * 0)*strong / 2, x2, y2;// 원을 출력하기 위한 변수들 선언
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 흰색으로 정함
		Font->w += 2;//원이 정확한 원이 아니라서 살짝 삐져나옴
		Font->h += 2;
		SDL_RenderFillRect(Renderer, Font);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
		Font->x = Track.x + 35 - strong / 2;// 왼쪽 꼭짓점 좌표를 다시설정
		Font->y = Track.y - 50 - strong / 2;// 오른쪽 꼭짓점 좌표를 다시설정
		Font->h = Font->w = strong;
		for (l = 0; l < 180; l++) {
			x1 = sin(3.14 / 180 * l)*strong / 2;
			y1 = cos(3.14 / 180 * l)*strong / 2;
			x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
			y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
			SDL_RenderDrawLine(Renderer, x1 + Font->x + strong / 2, y1 + Font->y + strong / 2, x2 + Font->x + strong / 2, y2 + Font->y + strong / 2);
		}
		if (strong > 5) {// if문에 굵기가 5초과인 경우만 이라고 한이유는 굵기가 5이하이면 테두리때문에 검은색으로 보일수도 있기 때문
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);// 검은색으로 설정
			for (l = 0; l < 360; l++) {
				x1 = sin(3.14 / 180 * l)*(strong / 2);
				y1 = cos(3.14 / 180 * l)*(strong / 2);
				x2 = sin(3.14 / 180 * (l + 1))*(strong / 2);
				y2 = cos(3.14 / 180 * (l + 1))*(strong / 2);
				SDL_RenderDrawLine(Renderer, x1 + Font->x + strong / 2, y1 + Font->y + strong / 2, x2 + Font->x + strong / 2, y2 + Font->y + strong / 2);

			}
		}
		strong *= 50 / 80.0;
	}
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
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect) {//텍스쳐를 출력하는 함수 선언
	SDL_Rect Src;// 직사각형 선언
	Src.x = 0;// 직사각형의 왼쪽위 꼭짓점의 x좌표초기화
	Src.y = 0;// 직사각형의 왼쪽위 꼭짓점의 y좌표초기화
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h); // Texture의 너비와 높이 정보를 Src.w, Src.h에 저장
	SDL_Rect Dst;
	Dst.x = Rect->x;//매개변수x를 왼쪽위 꼭짓점의 x좌표에 대입
	Dst.y = Rect->y;//매개변수y를 왼쪽위 꼭짓점의 y좌표에 대입
	Dst.w = Rect->w;//매개변수w를 직사각형의 너비에 대입
	Dst.h = Rect->h;//매개변수h를 직사각형의 높이에 대입
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst);//Src의 정보를 가지고 있는 Texture를 Dst의 정보를 가진 Texture 로 변환하여 렌더러에 저장
	return;
}
void ReceiveRender(SDL_Renderer* Renderer4, bool eraser, bool pencil, bool drag, int x, int y, float strong, float r, float g, float b) {
	cur(0, 0);
//	printf("eraser: %d pencil: %d drag: %d x: %d y: %d strong: %f r: %f g: %f b: %f       \n", eraser, pencil, drag, x, y, strong, r, g, b);
	if (SDL_Clear == true) {
		printf("clear문 실행할게슴\n");
		SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);
		SDL_RenderClear(Renderer4);
		SDL_RenderPresent(Renderer4);
		SDL_Clear = false;
	}
	else {
		if (pencil == true && drag == false) {//eraser 상태에서 클릭한 경우
			ReceiveRect.x = x - strong / 2;
			ReceiveRect.y = y - strong / 2;// 굵기만큼의 사각형을 만듬
			ReceiveRect.w = ReceiveRect.h = strong;// 굵기 설정
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			SDL_RenderFillRect(Renderer4, &ReceiveRect);// 렌더러에 그림
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == false) {
			strong *= 80 / 50.0;
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);
			int x1, y1, x2, y2, l;
			ReceiveRect.x = x;
			ReceiveRect.y = y;// 굵기만큼의 사각형을 만듬
			for (l = 0; l < 180; l++) {
				x1 = sin(3.14 / 180 * l)*strong / 2;
				y1 = cos(3.14 / 180 * l)*strong / 2;
				x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
				y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
				SDL_RenderDrawLine(Renderer4, x1 + ReceiveRect.x, y1 + ReceiveRect.y, x2 + ReceiveRect.x, y2 + ReceiveRect.y);
			}
			strong *= 50.0 / 80;\
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (pencil == true && drag == true) {

			printf("pencil 드래그문 실행할게슴\n");
			float i = 0, j = 0, k = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// 두점사이의 길이를 피타고라스의 정리로 구함. 이때 두점은 전에 찍힌 점과 드래그한 곳의 점을 말함
		//	printf("length:%f\n", length);
			if (length == 0) return;
		//	printf("clicks.pencil %d\n", clicks.pencil);
		//	if (pencil == true) {// 펜슬일 경우
				SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
				printf("rgb값은 %d %d %d임\n", r, g, b);
				i = (x - (ReceiveRect.x + ReceiveRect.w / 2)) / length;// i는 두점의 x좌표의 차이를 길이로 나눈 것임.
				j = (y - (ReceiveRect.y + ReceiveRect.h / 2)) / length;// j는 두점의 y좌표의 차이를 길이로 나눈 것임.
				k = 0;// while문안에 쓸 변수 초기화.
				xpos = ReceiveRect.x + ReceiveRect.w / 2 - strong / 2;// 전에찍은점 x좌표를 따로 저장
				ypos = ReceiveRect.y + ReceiveRect.h / 2 - strong / 2;// 전에찍은점 y좌표를 따로 저장
				ReceiveRect.w = ReceiveRect.h = strong;// 굵기설정
				for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
					ReceiveRect.x = xpos + k*i;// 찍을 점의 왼쪽위 꼭짓점의 x좌표를 설정 
					ReceiveRect.y = ypos + k*j;// 찍을 점의 왼쪽위 꼭짓점의 y좌표를 설정
					SDL_RenderFillRect(Renderer4, &ReceiveRect);//사각형 렌더러에 저장
				}
		//	}
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == true) {
			strong *= 80 / 50.0;
			float i = 0, j = 0, k = 0, l = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// 두점사이의 길이를 피타고라스의 정리로 구함. 이때 두점은 전에 찍힌 점과 드래그한 곳의 점을 말함
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);// 지우개니깐 무조건 하얀색으로									
			i = (x - ReceiveRect.x) / length;// i는 두점의 x좌표의 차이를 길이로 나눈 것임.
			j = (y - ReceiveRect.y) / length;// j는 두점의 y좌표의 차이를 길이로 나눈 것임.
			k = 0;// while문안에 쓸 변수 초기화.
			xpos = ReceiveRect.x;// 전에찍은점 x좌표를 따로 저장
			ypos = ReceiveRect.y;// 전에찍은점 y좌표를 따로 저장
			ReceiveRect.w = ReceiveRect.h = strong;// 굵기설정
			for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
				ReceiveRect.x = xpos + k*i;// 찍을 점의 중심 점 x좌표를 설정 
				ReceiveRect.y = ypos + k*j;// 찍을 점의 중심 점 y좌표를 설정
				int x1, y1, x2, y2;
				for (l = 0; l < 180; l++) {
					x1 = sin(3.14 / 180 * l)*strong / 2;
					y1 = cos(3.14 / 180 * l)*strong / 2;
					x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
					y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
					SDL_RenderDrawLine(Renderer4, x1 + ReceiveRect.x, y1 + ReceiveRect.y, x2 + ReceiveRect.x, y2 + ReceiveRect.y);
				}
			}
			strong *= 50 / 80.0;
			SDL_RenderPresent(Renderer4);
			return;
		}
	}
}
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Rect Track, SDL_Rect Box, SDL_Rect Eraser, SDL_Rect Pencil, SDL_Rect New, SDL_Rect *Font, float strong, int r, int g, int b) {
	SDL_SetRenderDrawColor(Renderer2, r, g, b, 0);// 색깔설정
	RenderTexture(Renderer, TraTexture, &Track);// 렌더러에 저장하기
	RenderTexture(Renderer, BoxTexture, &Box);// 렌더러에 저장하기
	RenderTexture(Renderer, EraTexture, &Eraser);// 렌더러에 저장하기
	RenderTexture(Renderer, PenTexture, &Pencil);// 렌더러에 저장하기
	RenderTexture(Renderer, NewTexture, &New);// 렌더러에 저장하기
	if (on.eraser == true || clicks.eraser == true) { // eraser가 클릭 되어 있거나 eraser아이콘위에 마우스가 있으면
		SDL_RenderRemoveEdge(Renderer, &Eraser);
		RenderTexture(Renderer, EraTexture, &Eraser);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, &Eraser, clicks.eraser);
	}
	if (on.pencil == true || clicks.pencil == true) { // pencil이 클릭 되어 있거나 pencil아이콘위에 마우스가 있으면
		SDL_RenderRemoveEdge(Renderer, &Pencil);
		RenderTexture(Renderer, PenTexture, &Pencil);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, &Pencil, clicks.pencil);
	}
	if (on.new == true) {
		SDL_RenderRemoveEdge(Renderer, &New);
		RenderTexture(Renderer, NewTexture, &New);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, &New, 0);
	}
	if (clicks.eraser == true || clicks.pencil == true)
		SDL_FontUpdate(Renderer, Font, Track, strong, r, g, b);
	SDL_RenderPresent(Renderer);// 렌더러 출력
	SDL_RenderPresent(Renderer2);
	SDL_RenderPresent(Renderer3);
	SDL_RenderRemoveEdge(Renderer, &Eraser);
	SDL_RenderRemoveEdge(Renderer, &Pencil);
	SDL_RenderRemoveEdge(Renderer, &New);
}
int SDL_MAINS(void) {// 이 메인은 SDL.h에 선언된 메인함수로 우리가 흔히 쓰는 메인이 아님, 따라서 매개변수도 맞춰줘야함
	SDL_Window * Window;//SDL 윈도우 선언
	SDL_Renderer * Renderer;// SDL 렌더러 선언 
	SDL_Window * Window2;
	SDL_Renderer * Renderer2;
	SDL_Window * Window3;
	SDL_Renderer * Renderer3;
	SDL_Rect center = { 0 };
	char query[256];
	// 텍스쳐와 사각형 선언
	SDL_Texture * RgbTexture = nullptr;// 알지비 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * PenTexture = nullptr;// 펜 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * EraTexture = nullptr;// 지우개 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * NewTexture = nullptr;// 새로만들기 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * TraTexture = nullptr;// 스크롤 트랙 이미지를 담기위한 텍스쳐선언
	SDL_Texture * BoxTexture = nullptr;// 스크롤 박스 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * ChaTexture = nullptr;// 채팅창 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * StaTexture = nullptr;// 상태창 이미지를 담기위한 텍스쳐 선언
	SDL_Rect RgbCode = { 0 };// RgbCode 이미지의 정보를 담기위한 사각형변수 선언
	SDL_Rect Pencil = { 0 }; // Pencil 이미지의 정보를 담기위한 사각형 변수 선언
	SDL_Rect Eraser = { 0 }; // Eraser 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect New = { 0 }; // New 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Track = { 0 };// Track 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Box = { 0 };//Box 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Chat = { 0 };// Chat 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Status = { 0 };//Status 이미지의 정보를 담기 위한 사각형 변수 선언
							// 텍스쳐와 사각형 선언 끝
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {// SDL_Init함수로 SDL 초기화하고 초기화 안되면 if문 실행 SDL_Init의 인수는 다양함(ex : SDL_INIT_VIDEO)
		SDL_ExceptionRoutine(Renderer, Window, "SDL_Init", 1);//단계1의 예외 처리 루틴실행
		return 0;// 종료
	};
	// 윈도우창 3개로 나누는 기준 x좌표는 1920 - 1310/4-10이고, 1080-900/4-10은 y좌표의 기준이다.
	Window = SDL_CreateWindow("HIT MIND WITH C", (1920 - 1310 / 4 - 10)*(1), 0, (1310 / 4 + 10)*(1), 1080 * (1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window == nullptr) {// 윈도우 생성 실패시 if문 실행
		SDL_ExceptionRoutine(Renderer, Window, "SDL_CreateWindow", 2);//단계2의 예외처리루틴실행
		return 0;//종료
	}
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Renderer == nullptr) {
		SDL_ExceptionRoutine(Renderer, Window, "SDL_CreateRenderer", 3);
		return 0;
	}
	Window2 = SDL_CreateWindow("HIT MIND WITH C 2", 0, 0, (1920 - 1310 / 4 - 10)*(1), (1080 - 900 / 4 - 10)*(1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window2 == nullptr) {// 윈도우 생성 실패시 if문 실행
		SDL_ExceptionRoutine(Renderer2, Window2, "SDL_CreateWindow2", 2);//단계2의 예외처리루틴실행
		return 0;//종료
	}
	Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer 함수로 SDL Renderer 생성 함수 호출시 넘겨주는 인수는 SDL_Window *, 드라이버 설정(-1이면 알아서 맞춰줌), 플래그(지금은 하드웨어가속과 수직동기화 사용을 허용함)
	if (Renderer2 == nullptr) {// 렌더러 생성 실패시 if문 실행
		SDL_ExceptionRoutine(Renderer2, Window2, "SDL_CreateRenderer2", 3);//단계3의 예외 처리 루틴 실행
		return 0;// 종료
	}
	Window3 = SDL_CreateWindow("HIT MIND WITH C 3", 0, (1080 - 900 / 4 - 10)*(1), (1920 - 1310 / 4 - 10)*(1), (900 / 4 + 10)*(1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window3 == nullptr) {// 윈도우 생성 실패시 if문 실행
		SDL_ExceptionRoutine(Renderer3, Window3, "SDL_CreateWindow3", 2);//단계2의 예외처리루틴실행
		return 0;//종료
	}
	Renderer3 = SDL_CreateRenderer(Window3, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer 함수로 SDL Renderer 생성 함수 호출시 넘겨주는 인수는 SDL_Window *, 드라이버 설정(-1이면 알아서 맞춰줌), 플래그(지금은 하드웨어가속과 수직동기화 사용을 허용함)
	if (Renderer3 == nullptr) {// 렌더러 생성 실패시 if문 실행
		SDL_ExceptionRoutine(Renderer3, Window3, "SDL_CreateRenderer3", 3);//단계3의 예외 처리 루틴 실행
		return 0;// 종료
	}

	// 흰색으로 세팅
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 그리기 색깔 설정
	SDL_RenderClear(Renderer);// 렌더러 모두 지움 (그리기 색깔로 화면이 채워짐)
	SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);// 그리기 색깔 설정
	SDL_RenderClear(Renderer2);// 렌더러 모두 지움 (그리기 색깔로 화면이 채워짐)
	SDL_SetRenderDrawColor(Renderer3, 255, 255, 255, 0);// 그리기 색깔 설정
	SDL_RenderClear(Renderer3);// 렌더러 모두 지움 (그리기 색깔로 화면이 채워짐)
							   // 끝
	SDL_Event event;//SDL_Event 변수 선언
	const Uint8 * keystate;// Key 상태 배열을 받기 위한 포인터 선언
						   // RgbCode 이미지
	RgbTexture = LoadTextureEx(Renderer, "RgbCode.jpg", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (RgbTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	SDL_QueryTexture(RgbTexture, NULL, NULL, &RgbCode.w, &RgbCode.h);// RgbCode 이미지의 가로세로 읽어오기. 윈도우 창을 3개로 나누는 기준이 되므로 윈도우창 선언전에 읽어옴
	RgbCode.w /= 4;
	RgbCode.w *= (1);
	RgbCode.h /= 4;
	RgbCode.h *= (1);
	RgbCode.x = 5 * (1);// 이미지의 x,y좌표와 너비와 높이 설정
	RgbCode.y = 1080 * (1) - RgbCode.h - 10 * (1);
	// 끝
	// Track 이미지
	TraTexture = LoadTextureEx(Renderer, "Track.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (TraTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}

	SDL_QueryTexture(TraTexture, NULL, NULL, &Track.w, &Track.h);//이미지 정보 불러오기
	Track.w /= 4;
	Track.w *= (1);
	Track.h /= 8;
	Track.h *= (1);
	Track.x = RgbCode.x;
	Track.y = RgbCode.y - Track.h - 25 * (1);
	// 끝
	// Box 이미지
	BoxTexture = LoadTextureEx(Renderer, "Box.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (BoxTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	SDL_QueryTexture(BoxTexture, NULL, NULL, &Box.w, &Box.h);//이미지 정보 불러오기
	Box.w /= 2;
	Box.w *= (1);
	Box.h /= 2;
	Box.h *= (1);
	Box.x = Track.x + 50 * (1);
	Box.y = Track.y + Track.h / 2 - Box.h / 2;
	// 끝
	// Pencil 이미지
	PenTexture = LoadTexture(Renderer, "Pencil.jpg"); // 이미지 불러오기
	if (PenTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	SDL_QueryTexture(PenTexture, NULL, NULL, &Pencil.w, &Pencil.h);
	Pencil.w /= 15;
	Pencil.h /= 15;
	Pencil.x = Track.x + 50 + 40;
	Pencil.y = Track.y - 30 - Pencil.h;
	// 끝
	// Eraser 이미지
	EraTexture = LoadTexture(Renderer, "Eraser.jpg"); // 이미지 불러오기
	if (EraTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	Eraser.w = Pencil.w;
	Eraser.h = Pencil.h;
	Eraser.x = Pencil.x + 50 * (1) + 30 * (1);;
	Eraser.y = Pencil.y;
	// 끝
	// New 이미지
	NewTexture = LoadTexture(Renderer, "New.jpg"); // 이미지 불러오기
	if (NewTexture == nullptr) {// 에러코드 잡기
		IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	New.w = Eraser.w;
	New.h = Eraser.h;
	New.x = Eraser.x + 50 * (1) + 30 * (1);
	New.y = Eraser.y;

	// 끝
	//DWORD th = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)thread, &SDL, 0, 0);
	bool quit = false;//불 변수 선언
	bool drag = false;// 드래그중인지 확인하는 변수 선언
	bool happen = true;
	int alpha;// 명도와 채도를 담기위한 변수 선언
	int x, y; // 움직이고 있지않은 마우스의 좌표를 담기위한 변수 선언
	float r = 0, g = 0, b = 0; //rgb값을 가질 변수 선언 나누기 연산을 하므로 실수형으로 선언
	float i = 0, j = 0, k = 0, l = 0, length = 0;// for문에서 쓸 변수선언
	int newclick = 0;
	float xpos = 0, ypos = 0;// 마우스 x좌표 y좌표를 저장하는 변수선언 
	float strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// 굵기의 선언
	SDL_Rect Rect = { 0 }; // 그릴 사각형의 변수를 반복문 밖에서 선언
	SDL_Rect Font = { Track.x - strong / 2 + 35 ,Track.y - strong / 2 - 50,strong,strong };// 색깔, 굵기등을 보여주기 위한 사각형 변수 선언
	SDL_Rect Edge = { 0 };// 테두리를 그리기 위한 사각형 변수 선언 
						  // 끝
						  // while문에서 쓸 변수의 초기값 설정
	RenderTexture(Renderer, RgbTexture, &RgbCode);// 렌더러에 저장하기
												  // 변수 초기값 설정끝
	if (lead == false)
	{
		char click_eraser, click_pencil;
		char dragging;
		int buff = 0;
		CLS;
		SDL_RenderUpdate(Renderer, Renderer2, Renderer3, TraTexture, BoxTexture, EraTexture, PenTexture, NewTexture, Track, Box, Eraser, Pencil, New, &Font, strong, r, g, b);
		while (1) {
			if (buff <= SDLCLOCK) {
				buff++;
				sscanf(clientcatchmind, "%hhd %hhd %hhd %d %d %f %f %f %f", &click_eraser, &click_pencil, &dragging, &x, &y,  &strong, &r, &g, &b);
		//		printf("%d %d %d %d %d %f %f %f %f\n", click_eraser, click_pencil, dragging, x, y, strong, r, g, b);
				ReceiveRender(Renderer2, (bool)click_eraser, (bool)click_pencil, (bool)dragging, x, y, strong, r, g, b);
			}
		}

	}
	while (!quit) {// quit가 true가 아닐때 동안 무한반복
		if (SDL_PollEvent(&event)) {//이벤트가 있으면 if문 실행
			switch (event.type) {//이벤트 타입에 따라 케이스문 실행
			case SDL_WINDOWEVENT://SDL종료 타입일 경우
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:// 다수 창에서의 닫기이벤트가 발생할경우
					quit = true;// quit를 true로 변경
					break;// 브레이크
				case SDL_WINDOWEVENT_ENTER:// 윈도우
					SDL_RaiseWindow(SDL_GetWindowFromID(event.window.windowID));//포커스 이동시킴
					break;
				case SDL_WINDOWEVENT_LEAVE:
					drag = false;//마우스가 창에서 나갔으므로 드래그 기능을 중지시킴
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					break;
				}
			case SDL_MOUSEMOTION: // 마우스가 움직인 타입일 경우
				if (event.motion.state == 1 && drag == true) {// 마우스가 움직였을때 마우스 왼쪽 버튼이 눌려져있다면 즉, 드래그 했다면
					if (event.motion.windowID == SDL_GetWindowID(Window)) {// 마우스가 움직인 곳이 첫번째 윈도우 창일경우
						if ((event.motion.x + Box.w / 2 >= Track.x&&event.motion.x + Box.w / 2 <= Track.x + Track.w) && (event.motion.y >= Box.y&&event.motion.y <= Box.y + Box.h)) {// 드래그한 점의 중심 x좌표가 트랙안에 잇고 드래그한 점의 중심 y좌표가 박스의 y좌표 범위 안에 있으면 if문 실행
							SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 색깔을 흰색으로 설정해야함 그래야 지우개 역할을 하므로
							SDL_RenderFillRect(Renderer, &Box);// 지우개같이 흰색으로 칠함
							Box.x = event.motion.x;// 박스의 x좌표를 클릭한곳의 x좌표로 바꿈 == 이동시킴
							strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// 굵기를 트랙과 스크롤 박스의 위치를 계산해서 정해줌			
							happen = true;
							break;// 이 구문을 탈출함
						}
					}
					else if (event.motion.windowID == SDL_GetWindowID(Window2)) {// 마우스가 움직인 곳이 두번째 윈도우 창일 경우
						length = sqrt(pow(Rect.x + strong / 2 - event.motion.x, 2) + pow(Rect.y + strong / 2 - event.motion.y, 2));// 두점사이의 길이를 피타고라스의 정리로 구함. 이때 두점은 전에 찍힌 점과 드래그한 곳의 점을 말함
						if (length == 0) break;
						if (clicks.pencil == true) {// 펜슬일 경우
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.motion.x, event.motion.y, strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							i = (event.motion.x - (Rect.x + Rect.w / 2)) / length;// i는 두점의 x좌표의 차이를 길이로 나눈 것임.
							j = (event.motion.y - (Rect.y + Rect.h / 2)) / length;// j는 두점의 y좌표의 차이를 길이로 나눈 것임.
							k = 0;// while문안에 쓸 변수 초기화.
							xpos = Rect.x + Rect.w / 2 - strong / 2;// 전에찍은점 x좌표를 따로 저장
							ypos = Rect.y + Rect.h / 2 - strong / 2;// 전에찍은점 y좌표를 따로 저장
							Rect.w = Rect.h = strong;// 굵기설정
							for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
								Rect.x = xpos + k*i;// 찍을 점의 왼쪽위 꼭짓점의 x좌표를 설정 
								Rect.y = ypos + k*j;// 찍을 점의 왼쪽위 꼭짓점의 y좌표를 설정
								SDL_RenderFillRect(Renderer2, &Rect);//사각형 렌더러에 저장
							}
							// 여기~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						}
						else if (clicks.eraser == true) {// 지우개 경우
							strong *= 80 / 50.0;
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.motion.x, event.motion.y, strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);// 지우개니깐 무조건 하얀색으로									
							i = (event.motion.x - Rect.x) / length;// i는 두점의 x좌표의 차이를 길이로 나눈 것임.
							j = (event.motion.y - Rect.y) / length;// j는 두점의 y좌표의 차이를 길이로 나눈 것임.
							k = 0;// while문안에 쓸 변수 초기화.
							xpos = Rect.x;// 전에찍은점 x좌표를 따로 저장
							ypos = Rect.y;// 전에찍은점 y좌표를 따로 저장
							Rect.w = Rect.h = strong;// 굵기설정
							for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
								Rect.x = xpos + k*i;// 찍을 점의 중심 점 x좌표를 설정 
								Rect.y = ypos + k*j;// 찍을 점의 중심 점 y좌표를 설정
								int x1, y1, x2, y2;
								for (l = 0; l < 180; l++) {
									x1 = sin(3.14 / 180 * l)*strong / 2;
									y1 = cos(3.14 / 180 * l)*strong / 2;
									x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
									y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
									SDL_RenderDrawLine(Renderer2, x1 + Rect.x, y1 + Rect.y, x2 + Rect.x, y2 + Rect.y);
								}
							}
							strong *= 50 / 80.0;
						}
						happen = true;
				//		send(connect_sock, "", 45, 0);
						//여기~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == 1) {
					if (event.button.windowID == SDL_GetWindowID(Window)) {
						if ((event.button.x >= RgbCode.x&&event.button.x <= RgbCode.x + RgbCode.w) && (event.button.y >= RgbCode.y&&event.button.y <= RgbCode.y + RgbCode.h)) {// RgbCode 이미지 안이면 if문 실행
							alpha = (event.button.y - RgbCode.y) / (RgbCode.h / 9);// RgbCode 안에서의 y축 계산 == 명도채도계산
							switch ((event.button.x - RgbCode.x) / (RgbCode.w / 13)) {// RgbCode안에서의 x축 계산
							case 0:// 색 설정 코드
								r = 255; g = 0; b = 0;
								break;
							case 1:
								r = 255; g = 128; b = 0;
								break;
							case 2:
								r = 255; g = 255; b = 0;
								break;
							case 3:
								r = 128; g = 255; b = 0;
								break;
							case 4:
								r = 0; g = 255; b = 0;
								break;
							case 5:
								r = 0; g = 255; b = 128;
								break;
							case 6:
								r = 0; g = 255; b = 255;
								break;
							case 7:
								r = 0; g = 128; b = 255;
								break;
							case 8:
								r = 0; g = 0; b = 255;
								break;
							case 9:
								r = 127; g = 0; b = 255;
								break;
							case 10:
								r = 255; g = 0; b = 255;
								break;
							case 11:
								r = 255; g = 0; b = 127;
								break;
							case 12:// case 12는 회색계열이라서 특수한 알고리즘임 그래서 따로 코드를 써줌
								r = 128 + (255 / 8.0)*(alpha - 4); g = 128 + (255 / 8.0) * (alpha - 4); b = 128 + (255 / 8.0) * (alpha - 4);
								alpha = 4;
								break;
							}
							// 수식으로 rgb값 설정
							if (alpha <= 4) {
								r = r + r / 5 * (alpha - 4);
								g = g + g / 5 * (alpha - 4);
								b = b + b / 5 * (alpha - 4);
								happen = true;
								break;
							}
							else {
								r = r + (255 - r) / 5 * (alpha - 4);
								g = g + (255 - g) / 5 * (alpha - 4);
								b = b + (255 - b) / 5 * (alpha - 4);
								happen = true;
								break;

							}
						}
						else if ((event.button.x >= Track.x&&event.button.x <= Track.x + Track.w) && (event.button.y >= Box.y&&event.button.y <= Box.y + Box.h)) {//스크롤 트랙을 클릭 했을 경우
							SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 흰색으로 설정(지우개 역할)
							SDL_RenderFillRect(Renderer, &Box);// 렌더러에 사각형을 그려줌. 근데 흰색이므로 지워주는 역할을 함
							Box.x = event.button.x;//스크롤 박스를 이동시킴
							drag = true; //드래그로 조정이 가능하게 설정
							strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// 굵기를 트랙과 스크롤 박스의 위치를 계산해서 정해줌
							happen = true;
							break;
						}
						else if ((event.button.x >= Eraser.x - 10 && event.button.x <= Eraser.x + Eraser.w + 10) && (event.button.y - 10 >= Eraser.y&&event.button.y <= Eraser.y + Eraser.h + 10)) {
							SDL_RenderFillRect(Renderer, &Font);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
							clicks.eraser = true;
							clicks.pencil = false;
							happen = true;
						}
						else if ((event.button.x >= New.x - 10 && event.button.x <= New.x + New.w + 10) && (event.button.y >= New.y - 10 && event.button.y <= New.y + New.h + 10)) {		//New 이미지를 클릭했을때
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
							SDL_RenderClear(Renderer2);
							Font.w += 2;// 완벽한 원이 아니라서 쪼끔 삐져나옴
							Font.h += 2;
							newclick = 1;
							//여기~~~~~~~~~~~~~~~~~~
							if (connect_sock != 0) {
								send(connect_sock, "SDLCLEAR", 45, 0);
							}
							SDL_RenderFillRect(Renderer, &Font);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
							clicks.eraser = false;
							clicks.pencil = false;
							happen = true;
						}

						else if ((event.button.x >= Pencil.x - 10 && event.button.x <= Pencil.x + Pencil.w + 10) && (event.button.y >= Pencil.y - 10 && event.button.y <= Pencil.y + Pencil.h + 10)) {
							Font.w += 2;// 완벽한 원이 아니라서 쪼끔 삐져나옴
							Font.h += 2;
							SDL_RenderFillRect(Renderer, &Font);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
							clicks.eraser = false;
							clicks.pencil = true;
							happen = true;
						}
					}
					else if (event.button.windowID == SDL_GetWindowID(Window2)) {
						if (clicks.pencil == true) {
							Rect.x = event.button.x - strong / 2;
							Rect.y = event.button.y - strong / 2;// 굵기만큼의 사각형을 만듬
							Rect.w = Rect.h = strong;// 굵기 설정
							SDL_RenderFillRect(Renderer2, &Rect);// 렌더러에 그림
																 // 여기~~~~~~~~~
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil,drag,event.button. x, event.button.y,strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							drag = true; //드래그로 그릴수 있게 설정
							happen = true;
					
							
							break;
						}
						else if (clicks.eraser == true) {
							strong *= 80 / 50.0;
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
							int x1, y1, x2, y2;
							Rect.x = event.button.x;
							Rect.y = event.button.y;// 굵기만큼의 사각형을 만듬
							for (l = 0; l < 180; l++) {
								x1 = sin(3.14 / 180 * l)*strong / 2;
								y1 = cos(3.14 / 180 * l)*strong / 2;
								x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
								y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
								SDL_RenderDrawLine(Renderer2, x1 + Rect.x, y1 + Rect.y, x2 + Rect.x, y2 + Rect.y);
							}
							// 여기~~~~~~~~~~~~~~
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil,drag, event.button.x, event.button.y,strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							strong *= 50.0 / 80;
							drag = true;
							happen = true;
							break;
						}
					}
				}
			case SDL_MOUSEBUTTONUP: // 마우스 버튼이 떼졌을때
				if (event.button.button == SDL_BUTTON_LEFT) // 떼진 버튼이 왼쪽버튼이라면
					drag = false;// 드래그로 하는 모든 것을 불가능하게 만듦
			}
		}

		SDL_GetMouseState(&x, &y);
		if ((x >= Eraser.x - 10 && x <= Eraser.x + Eraser.w + 10) && (y >= Eraser.y - 10 && y <= Eraser.y + Eraser.h + 10)) {// eraser안에 마우스가 있을때
			if (on.eraser == false && clicks.eraser == false) // 그전까지는 마우스가 올려져있지않고 지우개가 활성화되지않았을때
				happen = true;// happen이 발생
			if (clicks.eraser == false)//지우개가 클릭된 상태가 아니었다면
				on.eraser = true;// 마우스가 올려진것으로 간주함
		}
		else if (on.eraser == true) {//그전까지는 마우스가 올려져있었고 지금은 eraser안에 마우스가 없으면
			happen = true;//happen이 발생
			on.eraser = false;// 마우스가 안 올려진것으로 간주함
		}
		if ((x >= Pencil.x - 10 && x <= Pencil.x + Pencil.w + 10) && (y >= Pencil.y - 10 && y <= Pencil.y + Pencil.h + 10)) {// eraser 안에 마우스가 있을때
			if (on.pencil == false && clicks.pencil == false)// 그전까지는 마우스가 올려져있지않고 펜슬이 활성화되지 않았을때
				happen = true;// happen이 발생
			if (clicks.pencil == false)//지우개가 클릭된 상태가 아니었다면
				on.pencil = true;// 마우스가 올려진 것으로 간주함
		}
		else if (on.pencil == true) {
			happen = true;
			on.pencil = false;
		}
		if ((x >= New.x - 10 && x <= New.x + New.w + 10) && (y >= New.y - 10 && y <= New.y + New.h + 10)) {
			if (on.new == false)
				happen = true;
			on.new = true;
		}
		else if (on.new == true) {
			happen = true;
			on.new = false;
		}
		if (happen == true) {
			SDL_RenderUpdate(Renderer, Renderer2, Renderer3, TraTexture, BoxTexture, EraTexture, PenTexture, NewTexture, Track, Box, Eraser, Pencil, New, &Font, strong, r, g, b);

		}
		happen = false;

	}
	SDL_DestroyTexture(RgbTexture);// 텍스쳐 파괴하기
	SDL_DestroyTexture(ChaTexture);
	SDL_DestroyTexture(BoxTexture);
	SDL_DestroyTexture(TraTexture);
	SDL_DestroyTexture(EraTexture);
	SDL_DestroyTexture(PenTexture);
	SDL_DestroyTexture(NewTexture);
	IMG_ExceptionRoutine(Renderer, Window);//IMG예외처리루틴 실행
	IMG_ExceptionRoutine(Renderer2, Window2);
	IMG_ExceptionRoutine(Renderer3, Window3);
	return 0;// 종료
}