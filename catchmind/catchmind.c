
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
#include <direct.h>
#include <stdint.h>
#include <Digitalv.h>
#include <mmsystem.h>
#include <gl/GL.h>
#include <crtdbg.h>
//#include <WinSock2.h>		//소켓프로그래밍
//특수 헤더파일 (따로 설치) 
#include "SDL/SDL.h"			//SDL - 그래픽 헤더파일
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/han2unicode.h"
#include "mysql/mysql.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL.h"
#include "iconv.h"
#define nullptr 0
// 라이브러리 선언문 라이브러리파일은 따로 추가안해도 됩니다.
// #pragma comment 는 visual studio에서만 사용 가능 *솔루션 플렛폼을 64비트로 해주세요
#pragma comment (lib, "libmysql.lib")	//mysql라이브러리
#pragma comment (lib, "SDL2")			//그래픽 라이브러리 1
#pragma comment (lib, "SDL2main")		//그래픽 라이브러리 2
#pragma comment (lib, "SDL2_image")		//그래픽 라이브러리 3
#pragma comment (lib,"SDL2_ttf")
#pragma comment (lib, "SDL2_mixer.lib")	//그래픽 사운드 라이브러리 4
#pragma comment (lib, "ws2_32.lib")		//소켓(네트워크)라이브러리

#pragma comment (lib, "libiconv.lib")

#pragma comment (lib, "winmm.lib")		//사운드 라이브러리

#pragma warning (disable : 4700)
#pragma warning (disable : 4244)
//#pragma warning (disable : 4101)		//사용하지 않은 지역변수입니다. 경고 무시

//#define 정의문
#define CLS system("cls")		//화면 지우기
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//색깔설정 X글자 Y 배경
#define RESET(X) ZeroMemory(X, sizeof(X))
#define CHOP(x) x[strlen(x) - 1] = ' '	//fgets를 쓰면 엔터도 배열에남음. 엔터를 배열에서 삭제해주는것
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
#define MY_BUFSIZE 1024 // 윈도우 콘솔창 제목의 최대 버퍼를 지정함

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
	int people;
	int time;
	int question;
	int mode;
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
struct {
	int time;
	int question;
	int mode;
}roommode = { 90, 15, 1 };
//전역 변수들 (사용 비추천)

CRITICAL_SECTION cs;	//이벤트
char status[4];			//소켓용
char username[30];		//사용자 이름
char friendname[4][30] = { "Player 1", "Player 2", "Player 3", "Player 4" };
WSADATA wsaData;						//소켓 WSAStartup()함수에 쓰이는 변수
SOCKET connect_sock, Sconnect_sock[4], listen_sock;	//서버 소켓변수
SOCKADDR_IN connect_addr, listen_addr;			//서버 주소정보 저장하는 변수
int sockaddr_in_size;
ROOM connectroom[6];
uintptr_t threads[10] = { 0, };
char turn = 0;
int score[4][2] = { 0, };
char signalmode;
char querys[10][100];
long Ping = 0;
bool lead = false;
char SOCKETCOUNT = 0;
char clientcatchmind[50];
char topics[4][30];
char myownnumber;
bool timeout = false;
MYSQL *cons;
char CHOOSEROOM = 0;
bool SDL_Clear = false;
bool CurrectHappen = true;
short Userping[4] = { -1, -1, -1, -1 };
int Gametopic = 0;
SDL_Rect ReceiveRect = { 0, };
int SDLCLOCK = 0;
char pasttopic[20];
bool CHATHAPPEN = false;
bool ExitHappen = false;
char chatquery[15][50];
Mix_Music *music, *mainmusic;
//기본 함수들
void gotoxy(short x, short y);
void cur(short x, short y);
void exitsignal(void);
void signalall(void);
void ConsoleL(int x, int y);					//콘솔창의 크기를 설정하는 함수 x y의 너비가 같음
POINT MouseClick(void);							//마우스를 클릭하면 그 값을 바로 반환해주는 함수 반환값은 POINT이다 (x, y)
void disablecursor(bool a);						//커서 보이기, 숨기기  0 = 보이기 1 = 숨기기
wchar_t* UTF82UNICODE(char* UTF8, int len);
char* UNICODE2UTF8(wchar_t* unicode, int len);
void usermain(void);

//--------------------- 네트워크 함수들 -----------------------------------
void ErrorHandling(char *Message);				//소켓 에러 출력 하는 함수
int Connect_Server(char *ServerIP);			//서버 연결 해주는 함수
void recieve(void);								//서버에서 데이터 받아오는 쓰레드용 함수
void sendall(char *message, int c);					//하나를받으면 전부전송
int waitroom(void);							//네트워크 대기방
void Clnt_1(int v);								//서버 - 클라이언트 1통신
void makeroom(int *count);							//방만들기(네트워크)
IN_ADDR GetDefaultMyIP(void);					//내 ip 얻기
bool exitallthread(void);
void CheckPing(void);

//--------------------- MySQL 함수들 --------------------------------------
int sqllogin(void);						//mysql에 저장된 데이터를 비교해 로그인을 하는 함수
int sqlsignup(void);						//mysql에 유저데이터를 추가하는 함수
void loadmysql(char mysqlip[]);	//mysql에 연결하는 함수
char **onemysqlquery(char *query); //mysql명령어의 결과하나를 바로 반환해주는 함수
void writechating(void);					//mysql에 채팅을 입력하는 함수
void readchating(void);					//mysql의 채팅을 읽는 함수
void inserttopic(void);
void sqlmakeroom(void);					//방을 만드는 함수

// -------------------- SDL 그래픽 함수들 ---------------------------------
void SDL_ErrorLog(const char * msg);//에러코드 출력 함수			//그래픽에러코드 출력 함수
void IMG_ErrorLog(const char * msg);			//이미지에러코드 출력 함수
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // 그래픽예외 처리 함수
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // 텍스쳐에 이미지파일 로드하는 함수 선언
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // 텍스쳐에 이미지파일 다양하게 로드하는 함수 선언
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect);	//텍스쳐를 출력하는 함수 선언
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Texture* ChaTexture, SDL_Texture* InpTexture, SDL_Rect *Track, SDL_Rect *Box, SDL_Rect *Eraser, SDL_Rect *Pencil, SDL_Rect *New, SDL_Rect *Font, SDL_Rect* Chat, SDL_Rect *InputT, TTF_Font* Fonts, wchar_t* inputText, float *strong, int r, int g, int b);
void SDL_FontUpdate(SDL_Renderer * Renderer, SDL_Rect* Font, SDL_Rect Track, float strong, int r, int g, int b);
void SDL_RenderRemoveEdge(SDL_Renderer* Renderer, SDL_Rect * Rect);
void SDL_RenderDrawEdge(SDL_Renderer* Renderer, SDL_Rect * Rect, bool clicks);
int SDL_MAINS(void);
void Quit(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Window* Window, SDL_Window* Window2, SDL_Window* Window3, TTF_Font * Font, int step);
void TTF_DrawText(SDL_Renderer *Renderer, TTF_Font* Font, wchar_t* sentence, int x, int y);
Uint32 get_pixel32(SDL_Surface *surface, int x, int y);
void makebmp(const char *filename, SDL_Renderer * Renderer2);
void ReceiveRender(SDL_Window * Window4, SDL_Renderer* Renderer4, bool eraser, bool pencil, bool drag, int x, int y, float strong, float r, float g, float b);
void contest(SDL_Window* Window,SDL_Renderer* Renderer,int i) {
	char inputfile[50]="";
	char str[100] = "";
	sprintf(inputfile,".\\text\\user%d.txt", i);
	FILE *in = fopen(inputfile,"r+");
	if (in == NULL) {
		return;
	}
	int j=0,max = 0,line =0;
	while (!feof(in)) {
		line++;
		fscanf(in, "%[^\n]s",str);
		fgetc(in);
		if (strcmp(str, "SDLCLEAR") == 0) {
			max = line;
		}
	}
	fclose(in);
	in = fopen(inputfile, "r+");
	for (j = 0; j < max; j++) {
		fscanf(in, "%[^\n]s", str);
		fgetc(in);
	}
	int eraser=0, pencil=0, drag=0;
		int x=0, y=0;
	float strong=0, r=0, g=0, b=0;
	while(!feof(in)){
		fscanf(in,"%d %d %d %d %d %f %f %f %f\n", &eraser, &pencil,&drag, &x, &y, &strong, &r, &g, &b);
		if (i == 1) 
			ReceiveRender(Window, Renderer, (bool)eraser, (bool)pencil, (bool)drag, x/2, y/2, strong, r, g, b);
		else if (i == 2) 
			ReceiveRender(Window, Renderer, (bool)eraser, (bool)pencil, (bool)drag, x / 2+(1920-1310/4-10)/2, y / 2, strong, r, g, b);
		else if (i == 3) 
			ReceiveRender(Window, Renderer, (bool)eraser, (bool)pencil, (bool)drag, x / 2, y / 2+(1080-900/4-10)/2, strong, r, g, b);
		else if (i == 4) 
			ReceiveRender(Window, Renderer, (bool)eraser, (bool)pencil, (bool)drag, x / 2+(1920 - 1310 / 4 - 10) / 2, y / 2 + (1080 - 900 / 4 - 10) / 2, strong, r, g, b);
	}
	fclose(in);
	return;
}
// -------------------- 게임 내부 함수들 ----------------------------------
void mainatitleimage(void);						//게임 메인타이틀 출력
int maintitle(void);							//게임 메인타이틀 출력및 선택
void banglist(int j);						//게임 방 출력
int bangchose(void);						//게임 방 출력및 선택
int chooseroom(int roomnum);
void waitroomtema();
void logintema(void);							//로그인 디자인
void jointema(void);							//회원가입 디자인
LOG login(int m);								//기본적인 로그인 입력
void zeroprint(int xx, int yy, int lr, int m);  //디자인
char checkkeyborad(char n, int togl);
void credit();									//크레딧
void Auto_Update(void);
void makeroomtema();
void roomprintwhite(int xx, int yy, int lr, int bibun, int gamo, int time, int ex);
void scannum(char *arr,int *tmpi);
//-------------------------콘솔 함수들------------------------------------
void checkword(char*nowword, char*scanword);	//단어를 확인함
wchar_t* UTF82UNICODE(char* UTF8, int len);
int UTF8toEUCKR(char *outBuf, int outLength, char *inBuf, int inLength);
char* getDesktopFolderName();
char* UNICODE2UTF8(wchar_t* unicode, int len);
void click(int *xx, int *yy, int *lr);					//클릭함수 두번째, xx값과 yy값을 변환함
HWND GetConsoleHwnd(void);



wchar_t* UTF82UNICODE(char* UTF8, int len) {
	wchar_t wstr[128] = L"";
	//	int i, sum;
	int i;
	for (i = 0; i < len; i += 3) {
		wstr[i / 3] = (UTF8[i] + 22) * 64 * 64 + (UTF8[i + 1] + 128) * 64 + UTF8[i + 2] + 41088;
	}
	wcscat(wstr, L"\0");
	return wstr;
}
char* UNICODE2UTF8(wchar_t* unicode, int len) {
	char str[128] = "";
	int i = 0, j = 0;
	for (i = 0; j < len; j++) {
		if (unicode[j] == 92 || unicode[j] == 39) {// 유니코드 92번(역슬래시)나 39번(작은따운표는) mysql에서 각각 \\, \'로 입력해야하므로 예외 처리를 해준다
			str[i] = 92;
			str[i + 1] = unicode[j];
			i += 2;
		}
		else if (unicode[j] >= 0xac00 && unicode[j] <= 0xD7A0) {// 완성형 한글일경우
			str[i] = (unicode[j] - 40960) / (64 * 64) - 22;
			str[i + 1] = (unicode[j] - 40960) % (4096) / 64 - 128;
			str[i + 2] = (unicode[j] - 40960) % 64 - 128;
			i += 3;
		}
		else if (unicode[j] >= 0x3131 && unicode[j] <= 0x3163) {// 초중종성일 경우
			str[i] = (unicode[j] - 12544) / (64 * 64) - 29;
			str[i + 1] = (unicode[j] - 12544) % (4096) / 64 - 124;
			str[i + 2] = (unicode[j] - 12544) % 64 - 128;
			i += 3;
		}
		else {
			str[i] = unicode[j];
			i++;
		}
	}
	strcat(str, "\0");
	return str;
}
int main(int argc, char **argv) //main함수 SDL에서는 인수와 리턴을 꼭 해줘야함 
{
	//SDL_MAIN();
	//변수 선언
	//int i, j, k, v, result;

	InitializeCriticalSection(&cs);
	unsigned int timeout = 15;
	char mainchoose = 0;
	char bangchoose;
	char chooseroomcount;
	//POINT pos;							//x, y좌표 표현 )pos.x, pos.y
	cons = mysql_init(NULL);			//mysql 초기화
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	char query[400];						//mysql 명령어를 저장함
	char mysqlip[30] = "10.80.161.182";		//mysql ip 상희ip입니다	지금 윈도우7버전 : 10.80.162.92 윈도우10버전 : 10.80.161.182
	char data[1000][30] = { 0, };           //단어데이터
	char nowword[30] = { 0, };              //랜덤선택 단어
	char scanword[30] = { 0, };             //내가 친 단어
	int bangnum = 0;						//고른 방의 번호
	char serverreturn = 0;					//Connect_Server가 반환한 값을 저장하는곳
	mysql_options(cons, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
	loadmysql(mysqlip);
	memset(&wsaData, 0, sizeof(wsaData));
	memset(&connect_sock, 0, sizeof(connect_sock));
	memset(&connect_addr, 0, sizeof(connect_addr));
	srand((unsigned int)time(NULL));
	//SDL_MAINS();
	// 초기화 끝
	signalall();

	if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("초기화 실패");
		Sleep(5000);
	}

	sprintf(query, "music\\5.mp3");
	music = Mix_LoadMUS(query);
	if (!music) {
		printf("Mix_LoadMUS(\"titlemusic.mp3\"): %s\n", Mix_GetError());
		Sleep(5000);
		// this might be a critical error...
	}
	mainmusic = Mix_LoadMUS("music\\main2.mp3");
	if (!music) {
		printf("Mix_LoadMUS(\"main.mp3\"): %s\n", Mix_GetError());
		Sleep(5000);
		// this might be a critical error...
	}
	Mix_FadeInMusic(mainmusic, -1, 5000);
	disablecursor(1);
	while (1) {								//로그인 반복문
		mainchoose = maintitle();				//main 화면
		CLS;
		if (mainchoose == 1) {				//main에서 첫번째를 고르면
			ConsoleL(25, 15);
			if (sqllogin() != 1)		//로그인에 성공하지 못하면 처음으로
				continue;

			ConsoleL(50, 20);
			while (1) {						//방 반복문
				gotoxy(0, 0);
				bangchoose = bangchose();	//방을 고름	
				if (bangchoose == 0) {			//방만들기를 클릭하면 방만들기로 이동
					sqlmakeroom();
					break;
				}
				else                            //방 선택 접속
				{
					if (connectroom[bangchoose - 2].people == 4)  //4명일떄
						chooseroomcount = -1;

					if (connectroom[bangchoose - 2].password[0] == 0 && connectroom[bangchoose - 2].people > 0) //비밀번호 없을경우 건너띔
						chooseroomcount = 1;
					else
						chooseroomcount = chooseroom(bangchoose);

					if (chooseroomcount == -1)		//return -1은 해당 방이없을때
					{
						continue;
					}
					else if (chooseroomcount == 0)		//return 0은 비밀번호가 틀릴때
					{
						cur(14, 1);
						printf("(비밀번호가 틀렸습니다)");
						_getch();
						CLS;
						fflush(stdin);
						continue;
					}
					else if (chooseroomcount == 1)		//return 1 은 비밀번호까지 맞을때
					{
						CHOOSEROOM = bangchoose - 2;
						serverreturn = Connect_Server(connectroom[bangchoose - 2].ip);		//서버 대기방 접속
						if (serverreturn == -1)
							continue;
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
						if (serverreturn == 3)
							continue;
						break;
					}
				}
			}
			break;
		}
		if (mainchoose == 2)
		{
			inserttopic();
			continue;
		}
		if (mainchoose == 3)
		{
			credit();
		}
	}return 0;
}

//함수 내용들		전부 최소화 Ctrl + M + O  전부 보이기 Ctrl + M + L
void credit() {
	//학교이름
	//제작자
	//개발기간
	//히트마인드 with c

	(GetAsyncKeyState(VK_LBUTTON) & 0x0001);

	gotoxy(50, 2);
	printf("히트마인드 with C project");

	gotoxy(50, 4);
	printf("개발기간 : 10일");

	gotoxy(50, 6);
	printf("사용기술 : c언어, SDL, 소켓, MySQL");

	gotoxy(50, 8);
	printf("소속학교 : 대구소프트웨어고등학교");

	gotoxy(50, 10);
	printf("제작자 : 서상희, 장민석, 배수한, 신상호");

	gotoxy(35, 14);
	printf("                                       ##                ##                    "); gotoxy(35, 15);
	printf("   ####            ##   ##             ##                ##    "); gotoxy(35, 16);
	printf("  ##  ##           ##   ##                               ## "); gotoxy(35, 17);
	printf(" ##                ### ###   #####   ####      #####   ######    #####   ## ### "); gotoxy(35, 18);
	printf(" ##                ## # ##  ##   ##    ##     ##         ##     ##   ##  ###   "); gotoxy(35, 19);
	printf(" ##                ## # ##  #######    ##      ####      ##     #######  ##   "); gotoxy(35, 20);
	printf("  ##  ##           ##   ##  ##         ##         ##     ##     ##       ##   "); gotoxy(35, 21);
	printf("   ####            ##   ##   #####   ######   #####       ###    #####   ##    "); gotoxy(35, 23);
	printf("                          Made By  C Meister TEAM");
	gotoxy(23, 28);
	printf("본 창작물은 크리에이티브 커먼즈 저작자표시-비영리-동일조건변경허락 4.0 국제 라이선스에 따라 이용할 수 있습니다.");
	gotoxy(49, 26);
	printf("Copyright(c)2017 by 신상호, 배수한, 장민석, 서상희");

	while (1) {
		if (kbhit())
			break;
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
			break;
	}
}
void inserttopic(void)
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
		mysql_free_result(sql_result);
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
void makeroomtema() {
	printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
	printf("■  비밀번호  □  ○미사용    ○사용	        ■\n"); //9~12 , 7    15~17, 7
	printf("■            □                                ■\n");
	printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
	printf("■  게임모드  □  ○일반  ○콘테스트  ○fps     ■\n"); //9~11 , 10   13~17, 10   19~21, 10
	printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
	printf("■한문제당시간□  ○60  ○90  ○120 ○입력:     ■\n"); //9~10 , 12~13, 15~17, 18~20
	printf("■□□□□□□□□□□□□□□□□□□□□□□□■\n");
	printf("■   문제수   □  ○5   ○10  ○15  ○입력:     ■\n"); //9~10 , 12~13, 15~16, 18~20
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
}
void roomprintwhite(int xx, int yy, int lr, int bibun, int gamo, int time, int ex) {
	WHITE if (!(yy == 7 && 9 <= xx && xx <= 12) && bibun != 1) {               //비밀번호
		gotoxy(18, 7);
		printf("○미사용");
	}
	if (!(yy == 7 && 15 <= xx && xx <= 17) && bibun != 2) {
		gotoxy(30, 7);
		printf("○사용");
	}
	if (!(yy == 10 && 9 <= xx && xx <= 11) && gamo != 1) {         //게임모드
		gotoxy(18, 10);
		printf("○일반");
	}
	if (!(yy == 10 && 13 <= xx && xx <= 17) && gamo != 2) {
		gotoxy(26, 10);
		printf("○콘테스트");
	}
	if (!(yy == 10 && 19 <= xx && xx <= 21) && gamo != 3) {
		gotoxy(38, 10);
		printf("○fps");
	}
	if (!(yy == 12 && 9 <= xx && xx <= 10) && time != 1) {         //문제당시간
		gotoxy(18, 12);
		printf("○60");
	}
	if (!(yy == 12 && 12 <= xx && xx <= 13) && time != 2) {
		gotoxy(24, 12);
		printf("○90");
	}
	if (!(yy == 12 && 15 <= xx && xx <= 17) && time != 3) {
		gotoxy(30, 12);
		printf("○120");
	}
	if (!(yy == 12 && 18 <= xx && xx <= 20) && time != 4) {
		gotoxy(36, 12);
		printf("○입력:");
	}
	if (!(yy == 14 && 9 <= xx && xx <= 10) && ex != 1) {         //문제수
		gotoxy(18, 14);
		printf("○5");
	}
	if (!(yy == 14 && 12 <= xx && xx <= 13) && ex != 2) {
		gotoxy(24, 14);
		printf("○10");
	}
	if (!(yy == 14 && 15 <= xx && xx <= 17) && ex != 3) {
		gotoxy(30, 14);
		printf("○15");
	}
	if (!(yy == 14 && 18 <= xx && xx <= 20) && ex != 4) {
		gotoxy(36, 14);
		printf("○입력:");
	}

	//--------------------------------------


}
void scannum(char *arr,int *tmpi) {
	disablecursor(0);

	int togl = -1;
	int keybit = 0;
	int i = *tmpi;

	arr[i] = checkkeyborad(arr[i], togl);


	if (arr[i])
		keybit = 1;
	else
		keybit = 0;

	if (keybit) {

		if (arr[i] == 8) {
			if (i == 0) {
				arr[0] = 0;
				return ;
			}
			printf("\b \b");
			arr[i - 1] = 0;
			arr[i--] = 0;
		}
		else if (i >= 3) {
			arr[i] = 0;
			return;
		}
		else if (!(arr[i] >= '0' && arr[i] <= '9')) {
			arr[i] = 0;
		}
		else {
			putchar(arr[*tmpi]);
			i++;
		}
	}
	*tmpi = i;
}
void scanning(char *s, int *tmpi) {
	int i = *tmpi;
	int keybit = 0;
	int togl = -1;
	disablecursor(0);

	s[i] = checkkeyborad(s[i], togl);

	if (s[i] == 1) {
		togl *= -1;
		s[i] = 0;
		return;
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		if (togl == -1 && 'a' <= s[i] && 'z' >= s[i])
			s[i] -= 32;
		else if (togl == 1 && 'A' <= s[i] && 'Z' >= s[i])
			s[i] += 32;
	}



	if (s[i])
		keybit = 1;
	else
		keybit = 0;

	if (keybit) {

		if (s[i] == 8) {
			if (i == 0) {
				s[0] = 0;
				return;
			}
			printf("\b \b");
			s[i - 1] = 0;
			s[i--] = 0;
		}
		else if (s[i] == 13) {
			s[i] = 0;
		}
		else if (i >= 10) {
			s[i] = 0;
		}
		else if (!((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'))) {
			s[i] = 0;
		}
		else {
			putchar(s[i++]);
		}
	}

	*tmpi = i;
	disablecursor(1);
}
void sqlmakeroom(void) {
	int count = 0;
	int i = 0;
	int c = 0;
	int togl = -1;
	int keybit = 0;
	POINT a;
	int xx = 0, yy = 0, lr = 0;
	int buff = 0;
	int gamo = 1, bibun = 1, time = 1, ex = 1;
	char timedata[5] = { 0, };
	char exdata[5] = { 0, };
	char tmp = 0;
	int ti = 0;
	int ei = 0;
	int ni = 0;
	IN_ADDR addr;

	addr = GetDefaultMyIP();	//디폴트 IPv4 주소 얻어오기
	char * myip = inet_ntoa(addr);
	ROOM myroom = { 0, 0, 0 };

	while (1) {
		ConsoleL(28, 23);
		WHITE
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("■                                              ■\n");
		printf("■            히트마인드 방 만들기              ■\n");
		printf("■          내 ip :  %s                     \n", myip);
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("■    제목    □                                ■\n");
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");


		gotoxy(48, 3);
		printf("■");
		disablecursor(0);
		//제목입력
		while (1) {
			c = 0;
			i = 0;
			for (i = 0; i < 30; i++)
				myroom.roomname[i] = 0;

			cur(16, 5);
			printf("                           ");
			cur(16, 5);
			scanf("%[^\n]s", myroom.roomname);
			getchar();
			for (i = 0; myroom.roomname[i] != 0; i++) {
				if (myroom.roomname[i] >= 0)
					if (!((myroom.roomname[i] >= '0' && myroom.roomname[i] <= '9') || (myroom.roomname[i] >= 'a' && myroom.roomname[i] <= 'z') || (myroom.roomname[i] >= 'A' && myroom.roomname[i] <= 'Z')))
						c++;
			}
			if (c == 0)
				break;
			gotoxy(13, 7);
			printf("공백, 숫자 안됩니다");
			fflush(stdin);
		}

		gotoxy(13, 7);
		printf("                                ");
		gotoxy(0, 6);
		makeroomtema();
		Sleep(100);
		disablecursor(1);


		while (1) {
			gotoxy(0, 0);
			GetCursorPos(&a);
			SetCursorPos(a.x, a.y);
			click(&xx, &yy, &lr);
			printf("%3d %3d %3d", xx, yy, lr);

			if (buff < 20) {
				tmp=checkkeyborad(tmp, -1);
				GetAsyncKeyState(VK_RETURN);
				buff++;
			}
			

			if (GetAsyncKeyState(VK_RETURN) & 0x0001)
				break;


			roomprintwhite(xx, yy, lr, bibun, gamo, time, ex); //흰색
			
			//녹색출력
			HIGH_GREEN if (yy == 7 && 9 <= xx && xx <= 12 && bibun != 1) {               //비밀번호
				gotoxy(18, 7);
				printf("○미사용");
				if (lr == 1)
					bibun = 1;
			}
			else if (yy == 7 && 15 <= xx && xx <= 17 && bibun != 2) {
				gotoxy(30, 7);
				printf("○사용");
				if (lr == 1)
					bibun = 2;
			}
			else if (yy == 10 && 9 <= xx && xx <= 11 && gamo != 1) {         //게임모드
				gotoxy(18, 10);
				printf("○일반");
				if (lr == 1)
					gamo = 1;
			}
			else if (yy == 10 && 13 <= xx && xx <= 17 && gamo != 2) {
				gotoxy(26, 10);
				printf("○콘테스트");
				if (lr == 1)
					gamo = 2;
			}
			else if (yy == 10 && 19 <= xx && xx <= 21 && gamo != 3) {
				gotoxy(38, 10);
				printf("○fps");
				if (lr == 1)
					gamo = 3;
			}
			else if (yy == 12 && 9 <= xx && xx <= 10 && time != 1) {         //문제당시간
				gotoxy(18, 12);
				printf("○60");
				if (lr == 1)
					time = 1;
			}
			else if (yy == 12 && 12 <= xx && xx <= 13 && time != 2) {
				gotoxy(24, 12);
				printf("○90");
				if (lr == 1)
					time = 2;
			}
			else if (yy == 12 && 15 <= xx && xx <= 17 && time != 3) {
				gotoxy(30, 12);
				printf("○120");
				if (lr == 1)
					time = 3;
			}
			else if (yy == 12 && 18 <= xx && xx <= 20 && time != 4) {
				gotoxy(36, 12);
				printf("○입력");
				if (lr == 1)
					time = 4;
			}
			else if (yy == 14 && 9 <= xx && xx <= 10 && ex != 1) {         //문제수
				gotoxy(18, 14);
				printf("○5");
				if (lr == 1)
					ex = 1;
			}
			else if (yy == 14 && 12 <= xx && xx <= 13 && ex != 2) {
				gotoxy(24, 14);
				printf("○10");
				if (lr == 1)
					ex = 2;
			}
			else if (yy == 14 && 15 <= xx && xx <= 17 && ex != 3) {
				gotoxy(30, 14);
				printf("○15");
				if (lr == 1)
					ex = 3;
			}
			else if (yy == 14 && 18 <= xx && xx <= 20 && ex != 4) {
				gotoxy(36, 14);
				printf("○입력");
				if (lr == 1)
					ex = 4;
			}

			//자주색 출력
			YELLOW if (bibun == 1) {               //비밀번호
				gotoxy(18, 7);
				printf("●미사용");
			}
			else if (bibun == 2) {
				gotoxy(30, 7);
				printf("●사용");
				gotoxy(6, 8);
				WHITE printf("입력    □");
				gotoxy(17 + ni, 8);
				scanning(myroom.roomname, &ni);
				YELLOW
			}
			if (gamo == 1) {         //게임모드
				gotoxy(18, 10);
				printf("●일반");
			}
			else if (gamo == 2) {
				gotoxy(26, 10);
				printf("●콘테스트");
			}
			else if (gamo == 3) {
				gotoxy(38, 10);
				printf("●fps");
			}
			if (time == 1) {         //문제당시간
				gotoxy(18, 12);
				printf("●60");
			}
			else if (time == 2) {
				gotoxy(24, 12);
				printf("●90");
			}
			else if (time == 3) {
				gotoxy(30, 12);
				printf("●120");
			}
			else if (time == 4) {
				gotoxy(36, 12);
				printf("●입력:");
				gotoxy(43+ti, 12);
				scannum(timedata,&ti);
				disablecursor(1);
			}
			if (ex == 1) {         //문제수
				gotoxy(18, 14);
				printf("●5");
			}
			else if (ex == 2) {
				gotoxy(24, 14);
				printf("●10");
			}
			else if (ex == 3) {
				gotoxy(30, 14);
				printf("●15");
			}
			else if (ex == 4) {
				gotoxy(36, 14);
				printf("●입력:");
				gotoxy(43 + ei, 14);
				scannum(exdata, &ei);
				disablecursor(1);
			}
		

			
		}
		cur(16, 7);
		i = 0;
		/*while (1) {

			myroom.password[i] = checkkeyborad(myroom.password[i], togl);

			if (buff < 20) {
				buff++;
				continue;
			}


			if (myroom.password[i] == 1) {
				togl *= -1;
				myroom.password[i] = 0;
				continue;
			}
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				if (togl == -1 && 'a' <= myroom.password[i] && 'z' >= myroom.password[i])
					myroom.password[i] -= 32;
				else if (togl == 1 && 'A' <= myroom.password[i] && 'Z' >= myroom.password[i])
					myroom.password[i] += 32;
			}



			if (myroom.password[i])
				keybit = 1;
			else
				keybit = 0;

			if (keybit) {

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
			else {
				disablecursor(1);
				GetCursorPos(&a);
				SetCursorPos(a.x, a.y);
				click(&xx, &yy, &lr);

				gotoxy(33, 9);
				if (xx >= 16 && xx <= 24 && yy == 9) {
					if (lr == 0) {
						HIGH_GREEN printf("○비밀번호 미사용");
					}
					else if (lr == 1) {
						WHITE
							ZeroMemory(myroom.password, sizeof(myroom.password));
						break;
					}
				}
				else {
					WHITE printf("○비밀번호 미사용");
				}
			}
			cur(16 + i, 7);
			disablecursor(0);
		}*/

		if (gamo == 1)
			roommode.mode = 1;
		else if (gamo == 2)
			roommode.mode = 2;
		else if (gamo == 3)
			roommode.mode = 3;

		if (time == 1)
			roommode.time = 60;
		else if (time == 2)
			roommode.time = 90;
		else if (time == 3)
			roommode.time = 120;

		if (ex == 1)
			roommode.question = 5;
		else if (ex == 2)
			roommode.question = 10;
		else if (ex == 3)
			roommode.question = 15;

		char query[300];
		sprintf(query, "insert into catchmind.room (ip, name, password, mode, time, question) values ('%s', '%s', '%s', '%d', '%d', '%d')", myip, myroom.roomname, myroom.password, roommode.mode, roommode.time, roommode.question);
		if (!(mysql_query(cons, query)))
		{
			cur(10, 1);
			printf("(알수 없는 오류. 중복이 안되게 만들어 주세요)");
		}
		disablecursor(1);
		CLS;
		ConsoleL(54, 50);
		strcpy(connectroom[0].roomname, myroom.roomname);
		strcpy(connectroom[0].password, myroom.password);
		strcpy(connectroom[0].ip, myip);
		connectroom[0].people = 0;
		CHOOSEROOM = 0;
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

		disablecursor(0);
	}
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
	ConsoleL(54, 50);
	disablecursor(1);
	int xx = 0, yy = 0, lr = 0;
	int togl = -1;
	CLS;
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



		if (status[0] == 10)									//게임 시작
		{
			status[0] = 2;
			CLS;
			turn = 0;
			printf("게임을 시작합니다.");
			if (lead == true)
			{
				sprintf(query, "delete from catchmind.chating where room = '%s'", inet_ntoa(GetDefaultMyIP()));
				mysql_query(cons, query);
				sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[관리자]', '%s방 게임을 시작합니다.', '%s')", connectroom[CHOOSEROOM].roomname, connectroom[CHOOSEROOM].ip);
				mysql_query(cons, query);
				sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[관리자]', '/?를 통해 명령어를 볼수 있습니다.', '%s')",connectroom[CHOOSEROOM].ip);
				mysql_query(cons, query);
				signalmode = 3;
				sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
				mysql_query(cons, query);

			}
			//	ExitThread(threads[5]);

			CheckPing();
			_beginthreadex(0, 0, (_beginthreadex_proc_type)readchating, 0, 0, 0);
			SDL_MAINS();
			return 1;
		}
		if (status[0] == -1)
		{

			CLS;
			closesocket(connect_sock);
			printf("서버가 닫혔습니다.");
			return 3;
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
				if (lead == true)
				{
					sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
					mysql_query(cons, query);
				}
				send(connect_sock, "exit", 45, 0);
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
			if (xx > 3 && xx < 12 && yy < 43 && yy > 39) {
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
char checkkeyborad(char n, int togl) {

	int k = 0;



	if (GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
		return '0';
	else if (GetAsyncKeyState(VK_NUMPAD1) & 0x0001)
		return '1';
	else if (GetAsyncKeyState(VK_NUMPAD2) & 0x0001)
		return '2';
	else if (GetAsyncKeyState(VK_NUMPAD3) & 0x0001)
		return '3';
	else if (GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
		return '4';
	else if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
		return '5';
	else if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
		return '6';
	else if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
		return '7';
	else if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
		return '8';
	else if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
		return '9';
	else if (GetAsyncKeyState(VK_BACK) & 0x0001)
		return 8;
	else if (GetAsyncKeyState(VK_TAB) & 0x0001)
		return 9;
	else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		return 13;
	else if (GetAsyncKeyState(VK_CAPITAL) & 0x0001) {
		return 1;
	}


	for (k = 65; k <= 90; k++)
		if (GetAsyncKeyState(k) & 0x0001)
			if (togl == -1)
				return k + 32;
			else
				return k;

	for (k = 48; k <= 57; k++)
		if (GetAsyncKeyState(k) & 0x0001)
			return k;

	return 0;

}
LOG login(int m) { // 1이면 로그인 2이면 회원가입 필수!!
				   //오류 없는 코드니까 회원가입이랑 로그인에 잘 적으시길
	int to = -1;
	int b = 0;
	int n = 0;
	int keybit = 0;
	int togl = -1;
	int shift = 0;
	POINT a;
restart:

	gotoxy(0, 0);
	if (m == 1)
		logintema();

	LOG user = { 0, 0, 0 };
	int i = 0, j = 0;
	int buff = 0;
	int cnt = 0;
	int xx = 0, yy = 0, lr = 0;
	/*닉네임 생성*/
	disablecursor(0);
	if (m == 2) {
		ConsoleL(25, 15);
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

	buff = 0;
	while (1) {

		user.id[i] = checkkeyborad(user.id[i], togl);

		if (buff < 20) {
			buff++;
			continue;
		}


		if (user.id[i] == 1) {
			togl *= -1;
			user.id[i] = 0;
			continue;
		}
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			if (togl == -1 && 'a' <= user.id[i] && 'z' >= user.id[i])
				user.id[i] -= 32;
			else if (togl == 1 && 'A' <= user.id[i] && 'Z' >= user.id[i])
				user.id[i] += 32;
		}



		if (user.id[i])
			keybit = 1;
		else
			keybit = 0;

		if (keybit) {

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
			else if (user.id[i] == 13 || user.id[i] == 9) {
				user.id[i] = 0;
			}
			else if (i >= 15) {
				continue;
			}
			else if (!((user.id[i] >= '0' && user.id[i] <= '9') || (user.id[i] >= 'a' && user.id[i] <= 'z') || (user.id[i] >= 'A' && user.id[i] <= 'Z'))) {
				user.id[i] = 0;
			}
			else {
				putchar(user.id[i++]);
			}
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
	buff = 0;
	/*비밀번호 암호화 구현*/
	gotoxy(16, 7);
	while (1) {

		user.pass[i] = checkkeyborad(user.pass[i], togl);

		if (buff < 20) {
			buff++;
			continue;
		}

		if (user.pass[i] == 1) {
			togl *= -1;
			user.pass[i] = 0;
			continue;
		}

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			if (togl == -1 && 'a' <= user.id[i] && 'z' >= user.id[i])
				user.id[i] -= 32;
			else if (togl == 1 && 'A' <= user.id[i] && 'Z' >= user.id[i])
				user.id[i] += 32;
		}

		if (user.pass[i])
			keybit = 1;
		else
			keybit = 0;

		if (keybit) {
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
					disablecursor(1);
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
					disablecursor(1);
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
					disablecursor(1);
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
void writechating(void)
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
			//		Sleep(10);
		}
		CLS;
		i = 0;
		sprintf(query, "insert into catchmind.chating (name, mean) values ('서상희', '%S')", buffer);
		mysql_query(cons, query);
		memset(buffer, 0, sizeof(buffer));
	}
}
void readchating(void) {
	Sleep(1000);
	int v = 0;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	int last2 = 0;
	int last = 0;
	int last3 = 0;
	char query[100];
	//	CHATHAPPEN = true;
	while (1) {
		EnterCriticalSection(&cs);
		if (CHATHAPPEN == false) {

			//	ZeroMemory(sql_result, sizeof(sql_result));
			ZeroMemory(chatquery, sizeof(chatquery));
			last3 = last2;
			sprintf(query, "select id from catchmind.chating where room = '%s' order by id desc limit 1", connectroom[CHOOSEROOM].ip);
			//	printf("\n%s", query);
			//	getchar();
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (sql_result != 0) {
				if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					last2 = atoi(sql_row[0]);

			}
			else
			{
				RESET(chatquery);
				CHATHAPPEN = true;
			}
			mysql_free_result(sql_result);
			if (last < last2)
			{
				sprintf(query, "select * from catchmind.chating where room = '%s' order by id desc limit 15", connectroom[CHOOSEROOM].ip);
				mysql_query(cons, query);
				sql_result = mysql_store_result(cons);

				v = 14;
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
				{

					sprintf(chatquery[v], "%s : %s", sql_row[2], sql_row[3]);
					v--;
				}
				mysql_free_result(sql_result);
				CHATHAPPEN = true;
				last = last2;
			}
			else
				CHATHAPPEN = false;

		}
		LeaveCriticalSection(&cs);
		Sleep(30);
	}

}
void loadmysql(char mysqlip[])	//MYSQL 서버 불러오기
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
		printf("새로운 ip를 설정해 주세요.\n윈도우 10이 안되면 윈도우7 ip는 10.80.162.92입니다.\n->");
		scanf("%s", mysqlip);
		getchar();
		loadmysql(mysqlip);					//재귀함수 호출
	}
	else {
		printf("\b2");
		printf("\n성공");
		mysql_set_character_set(cons, "euckr");
		CLS;
		mysql_select_db(cons, "catchmind");
	}

	return;
}
char **onemysqlquery(char *query) {		//mysql 명령어의 결과하나를 바로 반환해주는 함수
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
	/*SDL_SetWindowMaximumSize(GetConsoleWindow(), x, y);
	SDL_SetWindowMinimumSize(GetConsoleWindow(), x, y);
	MINMAXINFO *lpMMI;
	lpMMI->ptMinTrackSize.x = x;
	lpMMI->ptMinTrackSize.y = y;
	lpMMI->ptMaxTrackSize.x = x;
	lpMMI->ptMaxTrackSize.y = y;*/
	//SetWindowPos(GetConsoleWindow(), HWND_TOP, 0, 0, x * 20, y * 20, SWP_);
	DWORD Style;
	Style = GetWindowLong(GetConsoleWindow(), GWL_STYLE); //부모윈도우의 윈도스타일 값을 Style에 저장.
	Style &= ~WS_THICKFRAME;							  //윈도우 창조절 방지
	Style &= ~WS_MAXIMIZEBOX;							  //윈도우 최대화 방지
	SetWindowLong(GetConsoleWindow(), GWL_STYLE, Style); //새로 바꾼 윈도우 스타일을 부모 윈도우의 윈도우스타일에 적용한 상태.

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
	printf("%s로 호스팅\n", ServerIP);
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock변수에 소켓 할당
	connect_addr.sin_family = AF_INET;				//연결할 서버의 주소 설정
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //서버 IP
	connect_addr.sin_port = htons(5555);					 //서버 포트
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) { //서버에 연결
		ErrorHandling("connect() error");
		return -1;
	}
	threads[0] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, NULL, 0, NULL); //서버에서 데이터를 받아오는 쓰레드 시작
	CLS;

	sprintf(query, "update catchmind.room set people = people + 1 where ip = '%s'", ServerIP);
	mysql_query(cons, query);
	sprintf(query, "player   connect %s", username);
	send(connect_sock, query, 45, 0);
	Sleep(200);

	return waitroom();
}
void recieve(void) { //서버에서 데이터 받아오는 쓰레드용 함수
	char message[50] = { 0, };
	int i = 0;
	while (1) {

		if (recv(connect_sock, message, 45, 0) > 0) { //서버에서 데이터를 받아와 message변수에 저장
			if (strncmp(message, "0 ", 2) == 0 || strncmp(message, "1 ", 2) == 0)
			{

				strcpy(clientcatchmind, message);
				SDLCLOCK++;
				ZeroMemory(message, sizeof(message));
				continue;
			}
			else if (strncmp("player 1 connect", message, 15) == 0) {
				sscanf(message, "player 1 connect %s", friendname[0]);
				status[0] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 connect", message, 15) == 0) {
				sscanf(message, "player 2 connect %s", friendname[1]);
				status[1] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 connect", message, 15) == 0) {
				sscanf(message, "player 3 connect %s", friendname[2]);
				status[2] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 connect", message, 15) == 0) {
				sscanf(message, "player 4 connect %s", friendname[3]);
				status[3] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 1 ready", message, 13) == 0) {
				sscanf(message, "player 1 ready %s", friendname[0]);
				status[0] = 2;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 ready", message, 13) == 0) {
				sscanf(message, "player 2 ready %s", friendname[1]);
				status[1] = 2;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 ready", message, 13) == 0) {
				sscanf(message, "player 3 ready %s", friendname[2]);
				status[2] = 2;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 ready", message, 13) == 0) {
				sscanf(message, "player 4 ready %s", friendname[3]);
				status[3] = 2;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 1 not ready", message, 16) == 0) {
				sscanf(message, "player 1 not ready %s", friendname[0]);
				status[0] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 not ready", message, 16) == 0) {
				sscanf(message, "player 2 not ready %s", friendname[1]);
				status[1] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 not ready", message, 16) == 0) {
				sscanf(message, "player 3 not ready %s", friendname[2]);
				status[2] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 not ready", message, 16) == 0) {
				sscanf(message, "player 4 not ready %s", friendname[3]);
				status[3] = 1;
				ShowWindow(GetConsoleWindow(), 1);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 1 exit", message, 12) == 0) {
				status[0] = 0;
				ShowWindow(GetConsoleWindow(), 1);
				ExitHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 2 exit", message, 12) == 0) {
				status[1] = 0;
				ShowWindow(GetConsoleWindow(), 1);
				ExitHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 3 exit", message, 12) == 0) {
				status[2] = 0;
				ShowWindow(GetConsoleWindow(), 1);
				ExitHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("player 4 exit", message, 12) == 0) {
				status[3] = 0;
				ShowWindow(GetConsoleWindow(), 1);
				ExitHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp("game start", message) == 0) {
				status[0] = 10;
				ShowWindow(GetConsoleWindow(), 1);
				CurrectHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp("server close", message) == 0) {
				status[0] = -1;
				CurrectHappen = true;
				ZeroMemory(message, sizeof(message));
			}
			else if (strcmp(message, "time out 1") == 0)
			{
				if (myownnumber == 1)
					Gametopic = 0;
				turn = 1;
				timeout = true;
			}
			else if (strcmp(message, "time out 2") == 0)
			{
				if (myownnumber == 2)
					Gametopic = 0;
				turn = 2;
				timeout = true;
			}
			else if (strcmp(message, "time out 3") == 0)
			{
				if (myownnumber == 3)
					Gametopic = 0;
				turn = 3;
				timeout = true;
			}
			else if (strcmp(message, "time out 4") == 0)
			{
				if (myownnumber == 4)
					Gametopic = 0;
				turn = 4;
				timeout = true;
			}
			else if (strcmp(message, "SDLCLEAR") == 0)
			{
				SDL_Clear = true;
				SDLCLOCK++;
				continue;
			}
			else if (strcmp(message, "right 1 answer") == 0)
			{
				sndPlaySoundA("music\\kill.wav", SND_ASYNC);
				strcpy(pasttopic, topics[turn - 1]);
				score[0][1] += 1;
				turn = 1;
				RESET(message);
				if (myownnumber == 1)
					Gametopic = 0;
				CurrectHappen = true;
			}
			else if (strcmp(message, "right 2 answer") == 0)
			{
				sndPlaySoundA("music\\kill.wav", SND_ASYNC);
				strcpy(pasttopic, topics[turn - 1]);
				score[1][1] += 1;
				turn = 2;
				RESET(message);
				if (myownnumber == 2)
					Gametopic = 0;
				CurrectHappen = true;
			}
			else if (strcmp(message, "right 3 answer") == 0)
			{
				sndPlaySoundA("music\\kill.wav", SND_ASYNC);
				strcpy(pasttopic, topics[turn - 1]);
				score[2][1] += 1;
				turn = 3;
				RESET(message);
				if (myownnumber == 3)
					Gametopic = 0;
				CurrectHappen = true;
			}
			else if (strcmp(message, "right 4 answer") == 0)
			{
				sndPlaySoundA("music\\kill.wav", SND_ASYNC);
				strcpy(pasttopic, topics[turn - 1]);
				score[3][1] += 1;
				turn = 4;
				RESET(message);
				if (myownnumber == 4)
					Gametopic = 0;
				CurrectHappen = true;
			}

			else if (strncmp("first ", message, 6) == 0)
			{
				sscanf(message, "first %hhd ping %ld", &myownnumber, &Ping);
				ZeroMemory(message, sizeof(message));

			}
			else if (strncmp("user 1 ping", message, 10) == 0)
			{
				sscanf(message, "user 1 ping %hd", &Userping[0]);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("user 2 ping", message, 10) == 0)
			{
				sscanf(message, "user 2 ping %hd", &Userping[1]);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("user 3 ping", message, 10) == 0)
			{
				sscanf(message, "user 3 ping %hd", &Userping[2]);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp("user 4 ping", message, 10) == 0)
			{
				sscanf(message, "user 4 ping %hd", &Userping[3]);
				ZeroMemory(message, sizeof(message));
			}
			else if (strncmp(message, "topic 1", 7) == 0)
			{
				sscanf(message, "topic 1 %s", topics[0]);
				cur(0, 8);
				//		printf("1번 사람 주제 : %s", topics[0]);
			}
			else if (strncmp(message, "topic 2", 7) == 0)
			{
				sscanf(message, "topic 2 %s", topics[1]);
				cur(0, 9);
				//		printf("2번 사람 주제 : %s", topics[1]);
			}
			else if (strncmp(message, "topic 3", 7) == 0)
			{
				sscanf(message, "topic 3 %s", topics[2]);
				cur(0, 10);
				//		printf("3번 사람 주제 : %s", topics[2]);
			}
			else if (strncmp(message, "topic 4", 7) == 4)
			{
				sscanf(message, "topic 4 %s", topics[3]);
				cur(0, 11);
				//		printf("4번 사람 주제 : %s", topics[3]);
			}

		}
		//	Sleep(100);
	}

}
int sqllogin(void) {
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
			check = sqlsignup();
			gotoxy(2, 3);
			if (check == 1)
				printf("               (회원가입 성공)           ");
			else if (check == -1)
				printf("             (아이디가 중복됩니다)              ");
			else if (check == -2)
				printf("             (닉네임이 중복됩니다)              ");
			else
				printf("	      (중복 값이 있습니다)           ");
		}
		else {
			sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id를 DB에서 찾음
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (mysql_fetch_row(sql_result) == NULL)									//해댱 id가 없으면 
			{
				gotoxy(2, 3);
				printf("           아이디가 존재하지 않습니다     ");
				//		mysql_free_result(sql_result);

			}
			else {
				mysql_free_result(sql_result);
				sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass);
				mysql_query(cons, query);	//password는 DB에 암호화되어 있어서 값을 비교할때도 서로 암호화해서 비교를함
				sql_result = mysql_store_result(cons);


				if ((sql_row = mysql_fetch_row(sql_result)) == NULL)
				{
					gotoxy(2, 3);
					printf("              비밀번호가 틀렸습니다        ");
					mysql_free_result(sql_result);

				}
				else {
					mysql_free_result(sql_result);
					strcpy(username, sql_row[1]);
					sprintf(query, "title %s님 히트마인드에 오신것을 환영합니다!", username);
					system(query);
					return 1; //로그인 성공
				}
			}
		}


	}


}
int sqlsignup(void) {
	LOG user;
	char query3[200];
	char query2[100];
	user = login(2);
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수


	if (user.name[0] == 0)
		return 0;
	/*
	sprintf(query2, "select * from catchmind.login where name = '%s'", user.name);	//이름을 DB에서 찾음
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -2;
	}*/
	ZeroMemory(query2, sizeof(query2));
	sprintf(query2, "select * from catchmind.login where id = '%s'", user.id);	//id를 DB에서 찾음
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -1;
	}
	mysql_free_result(sql_result);
	sprintf(query3, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);

	if (!(mysql_query(cons, query3)))											//		 password는 mysql에서 지원하는 암호화 형식임.
		return 1; //성공
	else
		return 0; //실패

}
void mainatitleimage(void) {
	SetConsoleTitle("히트마인드 with C      Powered by  C Meister TEAM");
	WHITE
		gotoxy(136, 1);
	printf("배경음악 ON   ");
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
	printf("■  게임 시작   ■                      ■  주제 추가   ■                      ■    크레딧    ■"); gotoxy(12, 24);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 25);
	printf("■              ■                      ■              ■                      ■              ■"); gotoxy(12, 26);
	printf("■■■■■■■■■                      ■■■■■■■■■                      ■■■■■■■■■");

	gotoxy(90, 34);
	printf("■■--공지사항--■■■■■■■■■■■■■■■■■■■"); gotoxy(90, 35);
	printf("■                                                  ■");  gotoxy(90, 36);
	printf("■  ● 게임 최초 실행시 해야할 설정들 (Enter키 입력)■"); gotoxy(90, 37);
	printf("■                                                  ■"); gotoxy(90, 38);
	printf("■  ● 간단한 설문조사와 피드백 (클릭)              ■"); gotoxy(90, 39);
	printf("■                                                  ■"); gotoxy(90, 40);
	printf("■  ● VPN을 끄고 실행 시켜주세요                   ■"); gotoxy(90, 41);
	printf("■                                                  ■"); gotoxy(90, 42);
	printf("■  ● 게임 강제종료시 문제가 발생할수 있습니다     ■"); gotoxy(90, 43);
	printf("■                                                  ■"); gotoxy(90, 44);
	printf("■  ● 심각한 문제 발생시 랩실 13으로               ■"); gotoxy(90, 45);
	printf("■                                                  ■"); gotoxy(90, 46);
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■");

}
int maintitle(void) { //게임 메인타이틀 출력
	ConsoleL(80, 50);
	Auto_Update();
	disablecursor(true);
	int xx = 0, yy = 0, lr = 0, bae = 1;

	mainatitleimage();
	while (1) {
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
			system("start https://blog.naver.com/soohan530/221053393169");
		WHITE
			/*	gotoxy(0, 0);
				printf("%3d %3d\n", xx, yy);*/

			click(&xx, &yy, &lr);
		cur(6, 1);
		printf("MySQL Ping : %dms", mysql_ping(cons));
		mysql_select_db(cons, "catchmind");
		//	MoveWindow(GetConsoleWindow(), xx * 20, yy * 20, 1000,1000, false);



		if (68 <= xx && xx <= 73 && yy == 1) {
			gotoxy(136, 1);

			if (lr == 0) {
				HIGH_GREEN
					if (bae == 1)
						printf("배경음악 ON   ");
					else
						printf("배경음악 OFF");

			}
			else if (lr == 1) {
				if (bae == 1) {
					Mix_PauseMusic();
					bae *= -1;
				}
				else {
					Mix_ResumeMusic();
					bae *= -1;
				}
			}
		}
		else if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 0) {
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
		else if (47 <= xx && xx <= 53 && 21 <= yy && yy <= 25 && lr == 0)
		{
			gotoxy(98, 23);
			HIGH_GREEN printf("크레딧");
		}
		else if (47 <= xx && xx <= 53 && 21 <= yy && yy <= 25 && lr == 1)
		{
			return 3;
		}
		else if (46 <= xx && xx <= 70 && yy == 36 && lr == 0) {
			gotoxy(94, 36);
			HIGH_GREEN printf("● 게임 최초 실행시 해야할 설정들 (Enter키 입력)");
			gotoxy(94, 38);
			WHITE printf("● 간단한 설문조사와 피드백 (클릭)");
		}
		else if (46 <= xx && xx <= 70 && yy == 38 && lr == 0) {
			gotoxy(94, 38);
			HIGH_GREEN printf("● 간단한 설문조사와 피드백 (클릭)");
			gotoxy(94, 36);
			WHITE printf("● 게임 최초 실행시 해야할 설정들 (Enter키 입력)");
		}
		else if (46 <= xx && xx <= 70 && yy == 36 && lr == 1) {
			Sleep(100);
			system("start https://blog.naver.com/soohan530/221053393169");
		}
		else if (46 <= xx && xx <= 70 && yy == 38 && lr == 1) {
			Sleep(100);
			system("start https://goo.gl/forms/0x4Vbro3dWqcO4213");
		}
		else {
			WHITE
				gotoxy(94, 36);
			printf("● 게임 최초 실행시 해야할 설정들 (Enter키 입력)");
			gotoxy(94, 38);
			printf("● 간단한 설문조사와 피드백 (클릭)");
			gotoxy(16, 23);
			printf("게임 시작");
			gotoxy(56, 23);
			printf("주제 추가");
			gotoxy(98, 23);
			printf("크레딧");
			gotoxy(136, 1);
			if (bae == 1)
				printf("배경음악 ON   ");
			else
				printf("배경음악 OFF");
		}


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
	ConsoleL(50, 20);
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
void banglist(int j) {
	MYSQL_RES *sql_result;					//mysql 결과의 한줄을 저장하는 변수
	MYSQL_ROW sql_row;						//mysql 결과의 데이터 하나를 저장하는 변수
	short i = 0;
	mysql_query(cons, "select ip, name, password, people from catchmind.room");
	sql_result = mysql_store_result(cons);

	memset(connectroom, 0, sizeof(connectroom));

	while (i < 6) { //화면지우기

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

		if (i % 2 == 0)
			cur(25, 8 + (i * 2));
		else
			cur(55, 8 + ((i / 2) * 4));

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

		if (j == i) {     //유저아이피
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

		if (j == i) {  //방제목
			HIGH_GREEN printf("%-7s", sql_row[1]);
		}
		else {
			WHITE printf("%-7s", sql_row[1]);
		}
		if (i % 2 == 0)
			cur(25, 8 + (i * 2));
		else
			cur(55, 8 + ((i / 2) * 4));

		if (j == i) {   //인원수
			HIGH_GREEN printf("%5s/4", sql_row[3]);
		}
		else {
			WHITE printf("%5s/4", sql_row[3]);
		}

		strcpy(connectroom[i].ip, sql_row[0]);
		strcpy(connectroom[i].roomname, sql_row[1]);
		strcpy(connectroom[i].password, sql_row[2]);
		connectroom[i].people = atoi(sql_row[3]);
		i++;

	}
	//mysql_free_result(sql_result);
	i = 0;

}
int bangchose(void) {

	int xx = 0, yy = 0, lr = 0;
	int j = 0;
	int b = 0, c = 0;

	POINT a;
	bangtema();
	j = -1;
	while (1) {
		banglist(j);
		/*	gotoxy(0, 0);
			printf("%3d %3d\n", xx, yy);*/

		GetCursorPos(&a);
		SetCursorPos(a.x, a.y);
		click(&xx, &yy, &lr);

		if (lr == 1) {
			if (9 <= xx && xx <= 22 && 2 == yy)			//방만들기
				return 0;
			else if (24 <= xx && xx <= 37 && 2 == yy) {//빠른시작
				for (c = 3; c > 0; c--)
					for (b = 0; b < 6; b++)
						if (connectroom[b].people == c && connectroom[b].password[0] == 0)
							return b + 2;
				for (c = 3; c > 0; c--)
					for (b = 0; b < 6; b++)
						if (connectroom[b].people == c)
							return b + 2;
			}
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
	char query[100];
	MYSQL_ROW sql_row;

	if (connectroom[roomnum].ip[0] == 0)
		return -1;

	ConsoleL(28, 11);
	WHITE
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■                                              ■\n");
	printf("■              히트마인드 방 접속              ■\n");
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

	sprintf(query, "select people from catchmind.room where ip = '%s'", connectroom[roomnum].ip);
	mysql_query(cons, query);
	sql_row = mysql_fetch_row(mysql_store_result(cons));
	connectroom[roomnum].people = atoi(sql_row[0]);
	//	mysql_free_result(sql_result);
	if (connectroom[roomnum].people == 4 || connectroom[roomnum].people == 0)
		return -1;

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
void Auto_Update(void)
{
	MYSQL_ROW sql_row;
	MYSQL_RES *sql_result;
	int i = 0;
	char serverversion[10];
	char choose;
	char version[] = "0.0.4";
	mysql_query(cons, "select * from catchmind.autoupdate order by version");
	sql_result = mysql_store_result(cons);
	cur(10, 34);
	printf("■■--업데이트기록--■■■■■■■■■■■■■■■■■■■■");
	cur(50, 34);
	printf("--현재 : %s--", version);
	while ((sql_row = mysql_fetch_row(sql_result)) != 0)
	{
		cur(10, 35 + (i * 3));
		printf("■                                                        ■");
		cur(10, 36 + (i * 3));
		printf("■                                                        ■");
		cur(10, 37 + (i * 3));
		printf("■                                                        ■");
		cur(20, 36 + (i * 3));
		printf("%s :", sql_row[0]);
		cur(30, 36 + (i * 3));
		printf("%s", sql_row[1]);
		strcpy(serverversion, sql_row[0]);
		i++;
	}

	cur(10, 35 + (i * 3));
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
	if (strcmp(serverversion, version) != 0)
	{
		cur(0, 0);
		printf("서버 버전과 현재 버전이 다릅니다. 업데이트 하시겠습니까? 안되면 관리자권한으로 실행해주세요 서버 버전 : %s 내 버전 : %s\n안되면 홈페이지에서 다운받아 실행해주세요\n 1. 예 2. 아니요", serverversion, version);
		choose = getch();
		if (choose != '1')
		{
			cur(0, 0);
			printf("                                                                                                                                    \n ");
			printf("                                                                                                                                    \n ");
			printf("                                                                                                                                     \n ");
			printf("                                                                                                                                     \n ");
			return;
		}
		else
		{
			FILE *ftp, *fftp;
			if ((ftp = fopen("./ftpscript.bat", "w+")) != NULL)
			{
				fprintf(ftp, "open %s\n", "10.80.161.182");
				fprintf(ftp, "상희\n");
				fprintf(ftp, "Desgayle10!\n");
				fprintf(ftp, "get catchmind.exe	Hitmind.exe\n");
				fprintf(ftp, "bye");
			}
			fclose(ftp);

			if ((fftp = fopen("./autoupdater.bat", "w+")) != NULL)
			{
				fprintf(ftp, "timeout /t 1 /NOBREAK\n");
				fprintf(ftp, "del Hitmind.exe\n");
				fprintf(ftp, "ftp -s:ftpscript.bat\n");
				fprintf(ftp, "del ftpscript.bat\n");
				fprintf(ftp, "start Hitmind.exe\n");
				fprintf(ftp, "exit");
			}
			//		fclose(fftp);
			system("start autoupdater.bat && del auto_updater.bat");
			exit(1);

		}

	}
}
void sendall(char *message, int c) {
	for (int i = 0; i < 4; i++)
	{
		if (i == c)
			continue;
		if (Sconnect_sock[i] != 0)
			send(Sconnect_sock[i], message, 45, 0);
	}


}
void makeroom(int *count) {
	int i = 0;
	char query[100];
	//	char message[100];
	//	IN_ADDR serverip = GetDefaultMyIP();
	listen_sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성 후 소켓에 대한 정보를 listen_sock변수에 대입					
	if (listen_sock == INVALID_SOCKET) {
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("socket() error");
	}
	printf("소켓 생성 완료!\n");
	memset(&listen_addr, 0, sizeof(listen_addr)); // 서버의 주소 초기화
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 서버의 주소를 내 주소로 (아마도)
	listen_addr.sin_port = htons(5555); // 서버 포트 
	printf("주소 지정 완료!\n");
	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) {// 지금까지 설정한 주소를 listen_sock에 bind()로 지정
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("bind() error");
	}
	printf("bind() 완료!\n");
	if (listen(listen_sock, 5) == SOCKET_ERROR) {	// 클라이언트가 접속할때 까지 기다림
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("listen() error");
	}
	printf("listen() 완료!\n");
	sockaddr_in_size = sizeof(connect_addr);
	*count = 1;
	while (!turn) {
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
	CLS;
	printf("ThreadExit");

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
	CLS;
	char query[100];
	if (signalmode == 1)
	{
		send(connect_sock, "exit", 40, 0);
		sprintf(query, "update catchmind.room set people = people - 1 where ip = '%s'", connectroom[CHOOSEROOM].ip);
		mysql_query(cons, query);
	}
	if (signalmode == 2)
	{
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		sprintf(query, "delete from catchmind.chating where room = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);

	}
	mysql_close(cons);


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
	i = 0;
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
			if (strncmp(message, "0 ", 2) == 0 || strncmp(message, "1 ", 2) == 0)
			{
				sendall(message, v);
			}
			else if (strcmp("right   answer", message) == 0)
			{
				message[6] = v + '0' + 1;
				sendall(message, 5);
				RESET(message);
			}
			else if (strncmp("setting", message, 7) == 0)
			{
				sendall(message, 5);
			}
			else if (strncmp("first   ping", message, 12) == 0)
			{
				message[6] = v + '0' + 1;
				send(Sconnect_sock[v], message, 45, 0);
				ZeroMemory(message, sizeof(message));
				continue;
			}
			else if (strncmp("user   ping", message, 10) == 0)
			{
				message[5] = v + '0' + 1;
				sendall(message, 5);
			}
			else if (strncmp(message, "player   connect", 16) == 0) {
				message[7] = v + '0' + 1;
				strcpy(querys[v], message);
				sendall(message, 5);

			}
			else if (strncmp(message, "time out", 8) == 0)
			{
				sendall(message, 5);
				RESET(message);
			}
			else if (strcmp(message, "player ready") == 0) {
				ZeroMemory(message, sizeof(message));
				sprintf(message, "player %d ready %s", v + 1, friendname[v]);
				strcpy(querys[v], message);
				sendall(message, 5);
			}
			else if (strcmp(message, "player not ready") == 0) {
				ZeroMemory(message, sizeof(message));
				sprintf(message, "player %d not ready %s", v + 1, friendname[v]);
				strcpy(querys[v], message);
				sendall(message, 5);
			}
			else if (strcmp(message, "exit") == 0)
			{
				ZeroMemory(message, sizeof(message));
				if (v == 0)
					sendall("server close", 5);
				else {
					sprintf(message, "player %d exit", v + 1);
					closesocket(Sconnect_sock[v]);
					SOCKETCOUNT = v;
					Sconnect_sock[v] = 0;
					strcpy(querys[v], message);
				}
				sendall(message, 5);
			}
			else if (strcmp(message, "game start") == 0)
			{
				cur(0, 0);
				printf("%s", message);
				sendall(message, 5);
			}
			else if (strncmp(message, "topic", 5) == 0)
			{
				message[6] = v + '0' + 1;
				sendall(message, 5);
			}
			else
			{
				sendall(message, v);
			}
			ZeroMemory(message, sizeof(message));
		}

	}
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
void CheckPing(void)
{
	Sleep(500);
	int i;
	for (i = 0; i < 4; i++)
	{
		Userping[i] = 0;
	}
	CLS;
	long now;
	printf("통신을 확인 중입니다.\n");
	char query[100];
	sprintf(query, "first   ping %ld", clock() + 1);
	send(connect_sock, query, 45, 0);
	//printf("%s\n", query);
	printf("핑 체크중....");
	while (Ping == 0)
	{

	}
	now = clock() - Ping + 1;
	printf("성공. 내 핑은 %ldms입니다\n내 고유 번호 = %d", now, myownnumber);
	Sleep(100);
	sprintf(query, "user   ping %ld", now);
	send(connect_sock, query, 45, 0);
	for (i = 0; i < 4; i++)
	{
		WHITE
			cur(0, 4 + i);
		printf("%s : ", friendname[i]);
		if (status[i] == 0)
		{
			GRAY
				printf("연결 안됨.");
		}
		else
		{
			now = clock();
			while (Userping[i] == -1)
			{
				if (clock() - now > 1000)
					break;
			}
			if (Userping[i] == -1)
			{
				RED
					printf("오류.");
			}
			else {
				HIGH_GREEN
					printf("핑 : %hd", Userping[i]);
			}
		}
	}
	WHITE
		printf("\n게임시작");
	Sleep(1000);



}
void SDL_ErrorLog(const char * msg) {//에러코드 출력 함수
	printf("%s Error: %s\n", msg, SDL_GetError());
	return;
}
void IMG_ErrorLog(const char * msg) {//에러코드 출력 함수
	printf("%s Error: %s\n", msg, IMG_GetError());
	return;
}
void TTF_ErrorLog(const char * msg) {
	printf("%s Error: %s\n", msg, TTF_GetError());
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
void getlevel(void)
{
	//	MYSQL_RES sql_result;
	MYSQL_ROW sql_row;
	int i;
	char query[100];
	for (i = 0; i < 4; i++) {
		if (status[i] != 0) {
			sprintf(query, "select level from catchmind.login where name = '%s'", friendname[i]);
			mysql_query(cons, query);
			sql_row = mysql_fetch_row(mysql_store_result(cons));
			score[i][0] = atoi(sql_row[0]);

		}
	}
	//	mysql_free_result(sql_result);
}
void Quit(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Window* Window, SDL_Window* Window2, SDL_Window* Window3, TTF_Font * Font,TTF_Font *topicFont,FILE* out[], int step) {
	switch (step) {
	case 10:
		if (out[0] != NULL)
		fclose(out[0]);
		if (out[1] != NULL)
		fclose(out[1]);
		if (out[2] != NULL)
		fclose(out[2]);
		if (out[3] != NULL)
		fclose(out[3]);
		SDL_StopTextInput();
	case 9:
		SDL_DestroyRenderer(Renderer3);// SDL 렌더러 파괴
	case 8:
		SDL_DestroyWindow(Window3);
	case 7:
		SDL_DestroyRenderer(Renderer2);
	case 6:
		SDL_DestroyWindow(Window2);//SDL 윈도우 파괴
	case 5:
		SDL_DestroyRenderer(Renderer);
	case 4:
		SDL_DestroyWindow(Window);
	case 3:
		TTF_CloseFont(topicFont);
			TTF_CloseFont(Font);
	case 2:
		TTF_Quit();//TTF 종료
	case 1:
		SDL_Quit();
	}
	getchar();
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
		strong *= 80 / (float)50.0;
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
void ReceiveRender(SDL_Window * Window4, SDL_Renderer* Renderer4, bool eraser, bool pencil, bool drag, int x, int y, float strong, float r, float g, float b) {
	if (SDL_Clear == true) {
		SDL_DestroyRenderer(Renderer4);
		Renderer4 = SDL_CreateRenderer(Window4, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);
		SDL_RenderClear(Renderer4);
		SDL_Clear = false;
		return;
	}
	else {
		if (pencil == true && drag == false) {//eraser 상태에서 클릭한 경우
			ReceiveRect.x = x - strong / 2;
			ReceiveRect.y = y - strong / 2;// 굵기만큼의 사각형을 만듬
			ReceiveRect.w = ReceiveRect.h = strong;// 굵기 설정
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			SDL_RenderFillRect(Renderer4, &ReceiveRect);// 렌더러에 그림
	//		SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == false) {
			strong *= 80 / (float)50.0;
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
			strong *= 50.0 / 80;
		//	SDL_RenderPresent(Renderer4);
			return;
		}
		else if (pencil == true && drag == true) {
			float i = 0, j = 0, k = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// 두점사이의 길이를 피타고라스의 정리로 구함. 이때 두점은 전에 찍힌 점과 드래그한 곳의 점을 말함
			if (length == 0) return;
			i = (x - (ReceiveRect.x + ReceiveRect.w / 2)) / length;// i는 두점의 x좌표의 차이를 길이로 나눈 것임.
			j = (y - (ReceiveRect.y + ReceiveRect.h / 2)) / length;// j는 두점의 y좌표의 차이를 길이로 나눈 것임.
			k = 0;// while문안에 쓸 변수 초기화.
			xpos = ReceiveRect.x + ReceiveRect.w / 2 - strong / 2;// 전에찍은점 x좌표를 따로 저장
			ypos = ReceiveRect.y + ReceiveRect.h / 2 - strong / 2;// 전에찍은점 y좌표를 따로 저장
			ReceiveRect.w = ReceiveRect.h = strong;// 굵기설정
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
				ReceiveRect.x = xpos + k*i;// 찍을 점의 왼쪽위 꼭짓점의 x좌표를 설정 
				ReceiveRect.y = ypos + k*j;// 찍을 점의 왼쪽위 꼭짓점의 y좌표를 설정

				SDL_RenderFillRect(Renderer4, &ReceiveRect);//사각형 렌더러에 저장
			}
		//	SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == true) {

			strong *= 80 / (float)50.0;
			float i = 0, j = 0, k = 0, l = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// 두점사이의 길이를 피타고라스의 정리로 구함. 이때 두점은 전에 찍힌 점과 드래그한 곳의 점을 말함
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);// 지우개니깐 무조건 하얀색으로	
			if (length == 0) return;
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
		//	SDL_RenderPresent(Renderer4);
			return;
		}
	}
}
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Texture* ChaTexture, SDL_Texture* InpTexture, SDL_Rect *Track, SDL_Rect *Box, SDL_Rect *Eraser, SDL_Rect *Pencil, SDL_Rect *New, SDL_Rect *Font, SDL_Rect* Chat, SDL_Rect *InputT, TTF_Font* Fonts, wchar_t* inputText, float *strong, int r, int g, int b) {
	SDL_SetRenderDrawColor(Renderer2, r, g, b, 0);// 색깔설정
	RenderTexture(Renderer, TraTexture, Track);// 렌더러에 저장하기
	RenderTexture(Renderer, BoxTexture, Box);// 렌더러에 저장하기
	RenderTexture(Renderer, EraTexture, Eraser);// 렌더러에 저장하기
	RenderTexture(Renderer, PenTexture, Pencil);// 렌더러에 저장하기
	RenderTexture(Renderer, NewTexture, New);// 렌더러에 저장하기
	RenderTexture(Renderer, InpTexture, InputT);// 렌더러에 저장하기
	if (on.eraser == true || clicks.eraser == true) { // eraser가 클릭 되어 있거나 eraser아이콘위에 마우스가 있으면
		SDL_RenderRemoveEdge(Renderer, Eraser);
		RenderTexture(Renderer, EraTexture, Eraser);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, Eraser, clicks.eraser);
	}
	if (on.pencil == true || clicks.pencil == true) { // pencil이 클릭 되어 있거나 pencil아이콘위에 마우스가 있으면
		SDL_RenderRemoveEdge(Renderer, Pencil);
		RenderTexture(Renderer, PenTexture, Pencil);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, Pencil, clicks.pencil);
	}
	if (on.new == true) {
		SDL_RenderRemoveEdge(Renderer, New);
		RenderTexture(Renderer, NewTexture, New);// 렌더러에 저장하기
		SDL_RenderDrawEdge(Renderer, New, 0);
	}
	if (clicks.eraser == true || clicks.pencil == true)
		SDL_FontUpdate(Renderer, Font, *Track, *strong, r, g, b);

	if (wcscmp(inputText, L"") != 0) {
		RenderTexture(Renderer, InpTexture, InputT);// 렌더러에 저장하기

		TTF_DrawText(Renderer, Fonts, inputText, 10, 672);
	}
	else {
		TTF_DrawText(Renderer, Fonts, " ", 10, 672);
	}
	SDL_RenderPresent(Renderer);// 렌더러 출력
	SDL_RenderPresent(Renderer2);
	SDL_RenderPresent(Renderer3);
	SDL_RenderRemoveEdge(Renderer, Eraser);
	SDL_RenderRemoveEdge(Renderer, Pencil);
	SDL_RenderRemoveEdge(Renderer, New);
}
void TTF_DrawText(SDL_Renderer *Renderer, TTF_Font* Font, wchar_t* sentence, int x, int y) {
	SDL_Color Color = { 0,0,0 };
	SDL_Surface * Surface = TTF_RenderUNICODE_Blended(Font, sentence, Color);// 폰트의 종류,문자열, 색깔을 보내서 유니코드로 렌더한다음 서피스에 저장한다
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);// 서피스로부터 텍스쳐를 생성한다
	SDL_FreeSurface(Surface);//서피스 메모리를 해제 해준다.
	SDL_Rect Src;
	Src.x = 0;
	Src.y = 0;
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h);
	SDL_Rect Dst;
	Dst.x = x;
	Dst.y = y;
	Dst.w = Src.w;
	Dst.h = Src.h;
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst); //그대로 렌더러에 저장한다
	return;
}
int UTF8toEUCKR(char *outBuf, int outLength, char *inBuf, int inLength)
{
	iconv_t cd = iconv_open("EUC-KR", "UTF-8");

	int ires = (int)iconv(cd, &inBuf, (size_t*)&inLength, &outBuf, (size_t*)&outLength);

	iconv_close(cd);

	return ires;
}
int unicodehan(wchar_t unicode[], int len) {
	int i, cnt = 0;
	for (i = 0; i < len; i++) {
		if ((unicode[i] >= 0xac00 && unicode[i] <= 0xd7a0) || (unicode[i] >= 0x3131 && unicode[i] <= 0x3163))
			cnt++;
	}
	return cnt;
}
int SDL_MAINS(void) {// 이 메인은 SDL.h에 선언된 메인함수로 우리가 흔히 쓰는 메인이 아님, 따라서 매개변수도 맞춰줘야함

	FILE* out[] = { NULL,NULL,NULL,NULL };
	SDL_Window * Window = nullptr;//SDL 윈도우 선언
	SDL_Renderer * Renderer = nullptr;// SDL 렌더러 선언 
	SDL_Window * Window2 = nullptr;
	SDL_Renderer * Renderer2 = nullptr;
	SDL_Window * Window3 = nullptr;
	SDL_Renderer * Renderer3 = nullptr;
	SDL_Rect center = { 0 };
	char query[256];
	Mix_PauseMusic();
	int senddata = 0;
	Mix_VolumeMusic(120);
	Mix_PlayMusic(music, -1);
	// 텍스쳐와 사각형 선언
	SDL_Texture * RgbTexture = nullptr;// 알지비 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * PenTexture = nullptr;// 펜 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * EraTexture = nullptr;// 지우개 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * NewTexture = nullptr;// 새로만들기 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * TraTexture = nullptr;// 스크롤 트랙 이미지를 담기위한 텍스쳐선언
	SDL_Texture * BoxTexture = nullptr;// 스크롤 박스 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * ChaTexture = nullptr;// 채팅창 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * StaTexture = nullptr;// 상태창 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * InpTexture = nullptr;// 채팅 보재는 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * UseTexture = nullptr;// 유저 이미지를 담기위한 텍스쳐 선언
	SDL_Texture * QusTexture = nullptr;// 주제 이미지를 담기위한 텍스쳐 선언

	SDL_Rect RgbCode = { 0 };// RgbCode 이미지의 정보를 담기위한 사각형변수 선언
	SDL_Rect Pencil = { 0 }; // Pencil 이미지의 정보를 담기위한 사각형 변수 선언
	SDL_Rect Eraser = { 0 }; // Eraser 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect New = { 0 }; // New 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Track = { 0 };// Track 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Box = { 0 };//Box 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Chat = { 0 };// Chat 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Status = { 0 };//Status 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect InputT = { 0 };//InputT 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect UserT = { 0 };//UserT 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect QuesT = { 0 };//QuesT 이미지의 정보를 담기 위한 사각형 변수 선언
	SDL_Rect Timer = { 0, 0, 1310 / 4 + 10, 180 };
	SDL_Rect Timer2 = { 0, 60, 400, 150 };
	SDL_Rect Timer3 = { 150, 150, 100, 30 };
	// 텍스쳐와 사각형 선언 끝

	char str[256] = "";//UNICODE2UTF8의 반환값을 복사할 배열선언
	int chaty = 0;
	float fontsize = 17.0;
	float fontsize2 = 35.0;
	TTF_Font * Font;
	TTF_Font * topicFont;
	//	SDL_Surface *Text;
	SDL_Rect  Word = { 0 };
	unsigned short unicode[128] = L"";

	//
	getlevel();
	// 추가
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {// SDL_Init함수로 SDL 초기화하고 초기화 안되면 if문 실행 SDL_Init의 인수는 다양함(ex : SDL_INIT_VIDEO)
		SDL_ErrorLog("SDL_Init");
		return 0;// 종료
	};
	//추가
	if (TTF_Init() != 0) {
		TTF_ErrorLog("TTF_Init");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 1);
		return 0;
	}
	Font = TTF_OpenFont(".\\font\\NanumGothic.ttf", fontsize);
	if (Font == nullptr) {
		TTF_ErrorLog("TTF_OpenFont");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 2);
		return 0;
	}
	topicFont = TTF_OpenFont(".\\font\\NanumGothic.ttf", fontsize2);
	if (topicFont == nullptr) {
		TTF_ErrorLog("TTF_OpenFont");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont, out,2);
		return 0;
	}

	// 윈도우창 3개로 나누는 기준 x좌표는 1920 - 1310/4-10이고, 1080-900/4-10은 y좌표의 기준이다.
	Window = SDL_CreateWindow("HIT MIND WITH C", 1920 - 1310 / 4 - 10, 0, 1310 / 4 + 10, 1080, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window == nullptr) {// 윈도우 생성 실패시 if문 실행
		SDL_ErrorLog("SDL_CreateWindow");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 3);
		return 0;//종료
	}
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Renderer == nullptr) {
		SDL_ErrorLog("SDL_CreateRenderer");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont, out,4);
		return 0;
	}
	Window2 = SDL_CreateWindow("HIT MIND WITH C 2", 0, 0, (1920 - 1310 / 4 - 10), (1080 - 900 / 4 - 10), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window2 == nullptr) {// 윈도우 생성 실패시 if문 실행
		SDL_ErrorLog("CreateWindow2");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 5);
		return 0;//종료
	}
	Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer 함수로 SDL Renderer 생성 함s수 호출시 넘겨주는 인수는 SDL_Window *, 드라이버 설정(-1이면 알아서 맞춰줌), 플래그(지금은 하드웨어가속과 수직동기화 사용을 허용함)
	if (Renderer2 == nullptr) {// 렌더러 생성 실패시 if문 실행
		SDL_ErrorLog("CreateRenderer2");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 6);
		return 0;// 종료
	}
	Window3 = SDL_CreateWindow("HIT MIND WITH C 3", 0, 1080 - 900 / 4 - 10, 1920 - 1310 / 4 - 10, 900 / 4 + 10, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow 함수로 SDL 윈도우 생성 함수호출시 넘겨주는 인수는 차례대로 창이름, 창의 x축위치, 창의 y축위치, 창의 너비, 창의 높이, 플래그임
	if (Window3 == nullptr) {// 윈도우 생성 실패시 if문 실행
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont, out,7);
		return 0;//종료
	}
	Renderer3 = SDL_CreateRenderer(Window3, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer 함수로 SDL Renderer 생성 함수 호출시 넘겨주는 인수는 SDL_Window *, 드라이버 설정(-1이면 알아서 맞춰줌), 플래그(지금은 하드웨어가속과 수직동기화 사용을 허용함)
	if (Renderer3 == nullptr) {// 렌더러 생성 실패시 if문 실행
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 8);
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
//	const Uint8 * keystate;// Key 상태 배열을 받기 위한 포인터 선언
						   // RgbCode 이미지
	RgbTexture = LoadTextureEx(Renderer, ".\\image\\RgbCode.jpg", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (RgbTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 9);
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
	TraTexture = LoadTextureEx(Renderer, ".\\image\\Track.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (TraTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
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
	BoxTexture = LoadTextureEx(Renderer, ".\\image\\Box.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// 이미지 불러오기
	if (BoxTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
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
	PenTexture = LoadTexture(Renderer, ".\\image\\Pencil.jpg"); // 이미지 불러오기
	if (PenTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
		return 0;
	}
	SDL_QueryTexture(PenTexture, NULL, NULL, &Pencil.w, &Pencil.h);
	Pencil.w /= 15;
	Pencil.h /= 15;
	Pencil.x = Track.x + 50 + 40;
	Pencil.y = Track.y - 30 - Pencil.h;
	// 끝
	// Eraser 이미지
	EraTexture = LoadTexture(Renderer, ".\\image\\Eraser.jpg"); // 이미지 불러오기
	if (EraTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 9);
		return 0;
	}
	Eraser.w = Pencil.w;
	Eraser.h = Pencil.h;
	Eraser.x = Pencil.x + 50 * (1) + 30 * (1);;
	Eraser.y = Pencil.y;
	// 끝
	// New 이미지
	NewTexture = LoadTexture(Renderer, ".\\image\\New.jpg"); // 이미지 불러오기
	if (NewTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	New.w = Eraser.w;
	New.h = Eraser.h;
	New.x = Eraser.x + 50 * (1) + 30 * (1);
	New.y = Eraser.y;
	ChaTexture = LoadTexture(Renderer, ".\\image\\CHAT_BODY.png");												// 채팅 이미지
	if (ChaTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	Chat.w = (1310 / 4) + 20;
	Chat.h = Eraser.y - 262;
	Chat.x = 0;
	Chat.y = 200;
	InpTexture = LoadTexture(Renderer, ".\\image\\Track.png");												// 채팅 이미지
	if (InpTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	InputT.w = (1310 / 4) + 20;
	InputT.h = 41;
	InputT.x = 0;
	InputT.y = Eraser.y - 71;
	RenderTexture(Renderer, RgbTexture, &RgbCode);
	UseTexture = LoadTexture(Renderer3, ".\\image\\user.png");
	if (UseTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	UserT.w = (1920 - (1310 / 4 - 10)) / 4;
	UserT.h = (900 / 4 - 10);
	UserT.y = 0;/*
	for (int i = 0; i < 4; i++) {
		UserT.x = ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98);
		RenderTexture(Renderer3, UseTexture, &UserT);
		TTF_DrawText(Renderer3, topicFont, friendname[i], (392.6125*i + 196.30625) - (strlen(friendname[i]) * 7), 5);
	}*/
	QusTexture = LoadTexture(Renderer, ".\\image\\question_fix.png");												// 채팅 이미지
	if (QusTexture == nullptr) {// 에러코드 잡기
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	QuesT.w = 400;
	QuesT.h = 100;
	QuesT.x = -40;
	QuesT.y = 60;

	// 끝
	int drawcount = 0;
	bool quit = false;//불 변수 선언
	bool drag = false;// 드래그중인지 확인하는 변수 선언
	bool happen = true;
	bool hangeulinput = false;
	bool enter = false;
	bool writemode = false;
	int alpha;// 명도와 채도를 담기위한 변수 선언
	int x, y; // 움직이고 있지않은 마우스의 좌표를 담기위한 변수 선언
	float r = 0, g = 0, b = 0; //rgb값을 가질 변수 선언 나누기 연산을 하므로 실수형으로 선언
	float i = 0, j = 0, k = 0, l = 0, length = 0;// for문에서 쓸 변수선언
	int pastturn = turn;
	int newclick = 0;
	float xpos = 0, ypos = 0;// 마우스 x좌표 y좌표를 저장하는 변수선언 
	float strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// 굵기의 선언
	SDL_Rect Rect = { 0 }; // 그릴 사각형의 변수를 반복문 밖에서 선언
	SDL_Rect Fonts = { Track.x - strong / 2 + 35 ,Track.y - strong / 2 - 50,strong,strong };// 색깔, 굵기등을 보여주기 위한 사각형 변수 선언
	SDL_Rect Edge = { 0 };// 테두리를 그리기 위한 사각형 변수 선언 
	SDL_Rect Happen = { 0,0,1310 / 4 + 10,New.y - 10 };// Happen 이 트루일때 사용할 변수
	char click_eraser, click_pencil;
	char dragging;
	int len = 0;
	MYSQL_ROW sql_row;
	int ee = 0;
	char euckr[256];
	char query2[50];
	Gametopic = 0;
	RESET(euckr);
	int xxx, yyy;
	float sstrong;
	float rr, gg, bb;
	int buff = 0;		  // 끝
						  // while문에서 쓸 변수의 초기값 설정
	// 렌더러에 저장하기
												  // 변수 초기값 설정끝
//	_beginthreadex(0, 0, (_beginthreadex_proc_type)rooprender, Renderer2, 0, 0);
	RenderTexture(Renderer3, UseTexture, &UserT);
	SDL_StartTextInput();
	wchar_t inputText[128] = L"";
	char topic[30];
	bool hangeul = false;
	wchar_t wstr[2] = L"";
	long firstclock = clock();
	int first = 0;
	turn++;
	/*
	contest(Window2, Renderer2, 1);
	contest(Window2, Renderer2, 2);
	contest(Window2, Renderer2, 3);
	contest(Window2, Renderer2, 4);
	*/;
	out[0] = fopen(".\\text\\user1.txt", "w");
	out[1] = fopen(".\\text\\user2.txt", "w");
	out[2] = fopen(".\\text\\user3.txt", "w");
	out[3] = fopen(".\\text\\user4.txt", "w");
	while (!quit) {// quit가 true가 아닐때 동안 무한반복
		if ((clock() - firstclock) / 1000 > first)
		{
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 색깔을 흰색으로 설정해야함 그래야 지우개 역할을 하므로
			SDL_RenderFillRect(Renderer, &Timer3);// 지우개같이 흰색으로 칠함
			first++;
			if (first == roommode.time + 1)
			{
				firstclock = clock();
				first = 0;
				if (turn == myownnumber)
				{
					first = 0;
					while (1)
					{
						turn++;
						if (turn == 5)
							turn = 1;
						if (status[turn - 1] != 0)
							break;

					}
					sprintf(query, "time out %d", turn);
					send(connect_sock, query, 45, 0);

				}
				while (!timeout);
				//		printf("turn : %d", turn);
				firstclock = clock();
				first = 0;
				timeout = false;
				SDL_DestroyRenderer(Renderer2);
				Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
				SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
				SDL_RenderClear(Renderer2);
				ee++;
			}
			sprintf(query, "%d초 남음", roommode.time - first);
			han2unicode(query, unicode);
			TTF_DrawText(Renderer, Font, unicode, 150, 150);
			happen = true;
		}
		if (myownnumber == turn && Gametopic == 0)
		{
			EnterCriticalSection(&cs);
			mysql_query(cons, "select top from catchmind.topic order by rand() limit 1");
			sql_row = (mysql_fetch_row(mysql_store_result(cons)));
			strcpy(topic, sql_row[0]);
			sprintf(query, "topic   %s", sql_row[0]);
			LeaveCriticalSection(&cs);
			//			mysql_free_result(sql_result);
			send(connect_sock, query, 45, 0);
			Gametopic++;
			drag = false;
			clicks.pencil = false;
			happen = true;
		}

		if (CurrectHappen == true)
		{
			CurrectHappen = false;

			SDL_DestroyRenderer(Renderer2);
			Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
			SDL_RenderClear(Renderer2);
			if (ee != 0) {
				firstclock = clock();
				first = 0;
				sprintf(query, "%s 님이 맞추었습니다! 정답은 %s 입니다", friendname[turn - 1], pasttopic);
				han2unicode(query, unicode);
				TTF_DrawText(Renderer2, topicFont, unicode, 0, 0);
				SDL_RenderPresent(Renderer2);
			}
			ee++;
			for (int i = 0; i < 4; i++)
			{

				if (status[i] != 0)
				{

					UserT.x = ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98);
					RenderTexture(Renderer3, UseTexture, &UserT);
					han2unicode(friendname[i], unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, (392.6125*i + 196.30625) - (strlen(friendname[i]) * 7), 5);
					sprintf(query, "%d", score[i][0]);
					han2unicode(query, unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 143);

					sprintf(query, "%d", score[i][1]);
					han2unicode(query, unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 75);
				}
			}



			happen = true;


		}
		if (pastturn != turn)
		{
			firstclock = clock();
			first = 0;
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 색깔을 흰색으로 설정해야함 그래야 지우개 역할을 하므로
			SDL_RenderFillRect(Renderer, &Timer);// 지우개같이 흰색으로 칠함
			if (myownnumber == turn) {
				RenderTexture(Renderer, QusTexture, &QuesT);// 렌더러에 저장하기
				han2unicode(topic, unicode);
				TTF_DrawText(Renderer, topicFont, unicode, 100, 90);
			}
			else
			{
				SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 색깔을 흰색으로 설정해야함 그래야 지우개 역할을 하므로
				SDL_RenderFillRect(Renderer, &Timer2);// 지우개같이 흰색으로 칠함
			}
			//			han2unicode(query, unicode);
			//			TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 148);	
			//		SDL_RenderPresent(Renderer);
			//		SDL_Delay(3000);
			sprintf(query, "%s 차례입니다", friendname[turn - 1]);
			han2unicode(query, unicode);
			TTF_DrawText(Renderer, topicFont, unicode, 0, 0);
			sprintf(query, "문제 %d/%d", ee, roommode.question);
			if (ee > roommode.question)
			{
				quit = true;
			}
			han2unicode(query, unicode);
			TTF_DrawText(Renderer, Font, unicode, 0, 150);
			pastturn = turn;
			happen = true;
		}
		if (ExitHappen == true)
		{
			ExitHappen = false;
			SDL_RenderClear(Renderer3);
			for (int i = 0; i < 4; i++)
			{

				if (status[i] != 0)
				{

					UserT.x = ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98);
					RenderTexture(Renderer3, UseTexture, &UserT);
					han2unicode(friendname[i], unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, (392.6125*i + 196.30625) - (strlen(friendname[i]) * 7), 5);
					sprintf(query, "%d", score[i][0]);
					han2unicode(query, unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 143);

					sprintf(query, "%d", score[i][1]);
					han2unicode(query, unicode);
					TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 75);
				}
			}

		}
		if (myownnumber == turn)
		{
			writemode = true;
		}
		else {
			drag = false;
			writemode = false;		//X
	//	CLS;
		}

		if (buff < SDLCLOCK) {
			buff++;
			sscanf(clientcatchmind, "%hhd %hhd %hhd %d %d %f %f %f %f", &click_eraser, &click_pencil, &dragging, &xxx, &yyy, &sstrong, &rr, &gg, &bb);
			cur(0, 30);
			printf("sscanf : %d", buff);
			ZeroMemory(clientcatchmind, sizeof(clientcatchmind));
			ReceiveRender(Window2, Renderer2, (bool)click_eraser, (bool)click_pencil, (bool)dragging, xxx, yyy, sstrong, (float)rr, (float)gg, (float)bb);
		}
		if (SDL_PollEvent(&event)) {//이벤트가 있으면 if문 실행
			switch (event.type) {//이벤트 타입에 따라 케이스문 실행
			case SDL_TEXTINPUT:
				if (hangeul == true && (event.text.text[0] == -29 || event.text.text[0] + 256 >= 234 && event.text.text[0] + 256 <= 237))// 한영키가 한글로 되어있고 한글이라면 event.text.text[0]의 값으로 한글판단가능함
				{
					wstr[2] = L"";
					int sum = (event.text.text[0] + 22) * 64 * 64 + (event.text.text[1] + 128) * 64 + event.text.text[2] + 41088;
					wstr[0] = sum;
					wcscat(inputText, wstr);
				}
				else if (!((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {// 한글아니고 c나 v를 눌렀을때 컨트롤모드가 아니라면 한글을 제외한 어떤 문자를 입력했다는 것임
					wstr[2] = L"";
					swprintf(wstr, sizeof(wstr) / sizeof(wchar_t), L"%hs", event.text.text);// event.text.text 문자열 그냥 연결시켜버림
					wcscat(inputText, wstr);// 문자열 연결
					hangeulinput = false;
				}
				happen = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN) {
					if (hangeulinput == true && enter == false)
						enter = true;
					else {
						strcpy(str, UNICODE2UTF8(inputText, wcslen(inputText)));
						UTF8toEUCKR(euckr, 256, str, 256);
						euckr[strlen(euckr)] = '\0';
						han2unicode(euckr, unicode);
						if (unicodehan(unicode, wcslen(unicode)) != unicodehan(inputText, wcslen(inputText)))
							strcpy(euckr, "[Error] invalid conversion");
						else if (strcmp(euckr, topics[turn - 1]) == 0)
						{
							if (myownnumber != turn)
								send(connect_sock, "right   answer", 35, 0);
						}
						else if (strcmp(euckr, "/clear") == 0)
						{
							EnterCriticalSection(&cs);
							sprintf(query, "delete from catchmind.chating where room = '%s'", connectroom[CHOOSEROOM].ip);
							mysql_query(cons, query);
							sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[명령]', '채팅을 초기화 합니다.', '%s')",connectroom[CHOOSEROOM].ip);
							mysql_query(cons, query);
							LeaveCriticalSection(&cs);
						}
						else if (strcmp(euckr, "/myturn") == 0)
						{
							send(connect_sock, "right   answer", 35, 0);
							Gametopic = 0;
						}
						else if (strcmp(euckr, "/help") == 0)
						{
							RESET(chatquery);
							strcpy(chatquery[10], "[1] /help : 도움말을 표시합니다.");

							strcpy(chatquery[11], "[2] /clear : 채팅창을 초기화합니다.");

							strcpy(chatquery[12], "[3] /capture [파일명] : 현재 화면을 캡쳐합니다.");

							strcpy(chatquery[13], "[4] /stopmusic : 현재 음악을 멈춥니다.");

							strcpy(chatquery[14], "[5] /startmusic : 현재 음악을 재실행합니다.");
							CHATHAPPEN = true;
						}
						else if (strcmp(euckr, "/?") == 0)
						{
							RESET(chatquery);
							strcpy(chatquery[10], "[1] /help : 도움말을 표시합니다.");

							strcpy(chatquery[11], "[2] /clear : 채팅창을 초기화합니다.");

							strcpy(chatquery[12], "[3] /capture [파일명] : 현재 화면을 캡쳐합니다.");

							strcpy(chatquery[13], "[4] /stopmusic : 현재 음악을 멈춥니다.");

							strcpy(chatquery[14], "[5] /startmusic : 현재 음악을 재실행합니다.");
							CHATHAPPEN = true;
						}
						else if (strncmp(euckr, "/capture ", 9) == 0)
						{			
							RESET(chatquery);
							
							printf("%s", chatquery[12]);
							sscanf(euckr, "/capture %s", query);
							
							sprintf(query2, "screenshot//%s.png", query);
							makebmp(query2, Renderer2);
							sprintf(chatquery[13], "%s\\screenshot", _getcwd(NULL, 0));
							sprintf(chatquery[14], "폴더 안에 %s.png가 저장이 되었습니다", query);
							RESET(chatquery[12]);
							RESET(euckr);
							CHATHAPPEN = true;
							
						}
						else if (strcmp(euckr, "/stopmusic") == 0)
						{

							Mix_PauseMusic();
						}
						else if (strcmp(euckr, "/startmusic") == 0)
						{
							Mix_ResumeMusic();
						}
						else {
							EnterCriticalSection(&cs);
							sprintf(query, "insert into catchmind.chating (name, mean, room) values ('%s', '%s', '%s')", username, euckr, connectroom[CHOOSEROOM].ip);
							mysql_query(cons, query);
							LeaveCriticalSection(&cs);
						}
						ZeroMemory(unicode, sizeof(unicode));
						ZeroMemory(str, sizeof(str));
						ZeroMemory(euckr, sizeof(euckr));
						ZeroMemory(inputText, sizeof(inputText));
						enter = false;
						happen = true;
					}

				}
				else if (event.key.keysym.sym == SDLK_RALT)
					hangeul = !(hangeul);
				else if (event.key.keysym.sym == SDLK_BACKSPACE && wcslen(inputText) > 0)// 키보드 백스페이스고 배열의 길이가 1이상일때
				{
					inputText[wcslen(inputText) - 1] = '\0';// 마지막문자를 널문자로 바꿈
					happen = true;
				}
				else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {// 컨트롤 모드이고 c를 눌렀다면
					strcpy(str, UNICODE2UTF8(inputText, wcslen(inputText)));
					SDL_SetClipboardText(str);// 클립보드에 넣음
				}
				else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)// 컨트롤 모드이고 v를 눌렀다면
					wcscpy(inputText, UTF82UNICODE(SDL_GetClipboardText(), strlen(SDL_GetClipboardText())));// 클립보드에서 가져옴
				else
					hangeulinput = true;
				happen = true;
				break;
			case SDL_WINDOWEVENT://SDL종료 타입일 경우

				switch (event.window.event) {

				case SDL_WINDOWEVENT_CLOSE:// 다수 창에서의 닫기이벤트가 발생할경우
					send(connect_sock, "exit", 35, 0);
					quit = true;// quit를 true로 변경
					Sleep(100);
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
							SDL_SetRenderDrawColor(Renderer, r, g, b, 0);
							for (k = 0; k < length; k++) {// 두 점사이의 공백을 전부 사각형으로 채우는 반복문임
								Rect.x = xpos + k*i;// 찍을 점의 왼쪽위 꼭짓점의 x좌표를 설정 
								Rect.y = ypos + k*j;// 찍을 점의 왼쪽위 꼭짓점의 y좌표를 설정
								SDL_RenderFillRect(Renderer2, &Rect);//사각형 렌더러에 저장
							}
							// 여기~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						}
						else if (clicks.eraser == true) {// 지우개 경우
							strong *= 80 / (float)50.0;
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
				if (writemode == true) {
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
								SDL_RenderFillRect(Renderer, &Fonts);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
								clicks.eraser = true;

								clicks.pencil = false;
								happen = true;
							}
							else if ((event.button.x >= New.x - 10 && event.button.x <= New.x + New.w + 10) && (event.button.y >= New.y - 10 && event.button.y <= New.y + New.h + 10)) {		//New 이미지를 클릭했을때

							//	sprintf(query, "screenshot\\%d.png", time(NULL));
						//		makebmp(query, Renderer2);


								SDL_DestroyRenderer(Renderer2);
								Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
								SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
								SDL_RenderClear(Renderer2);
								happen = true;
								sndPlaySoundA("music\\erase.wav", SND_ASYNC);
								//여기~~~~~~~~~~~~~~~~~~
								if (connect_sock != 0) {
									send(connect_sock, "SDLCLEAR", 45, 0);
								}
								//	SDL_RenderFillRect(Renderer, &Fonts);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
								clicks.eraser = false;
								clicks.pencil = true;

								happen = true;
							}

							else if ((event.button.x >= Pencil.x - 10 && event.button.x <= Pencil.x + Pencil.w + 10) && (event.button.y >= Pencil.y - 10 && event.button.y <= Pencil.y + Pencil.h + 10)) {
								Fonts.w += 2;// 완벽한 원이 아니라서 쪼끔 삐져나옴
								Fonts.h += 2;
								SDL_RenderFillRect(Renderer, &Fonts);// 폰트를 출력함. 근데 흰색이므로 지워주는 역할을 하게됨
								clicks.eraser = false;
								clicks.pencil = true;
								sndPlaySoundA("music\\pencil.wav", SND_ASYNC);
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
									sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.button.x, event.button.y, strong, r, g, b);
									send(connect_sock, query, 45, 0);
								}
								drag = true; //드래그로 그릴수 있게 설정
								happen = true;


								break;
							}
							else if (clicks.eraser == true) {
								strong *= 80 / (float)50.0;
								SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
								int x1, y1, x2, y2;
								Rect.x = event.button.x;// 원이라서 꼭짓점의 좌표가아닌 중심좌표를 찍어줘야함
								Rect.y = event.button.y;
								for (l = 0; l < 180; l++) {
									x1 = sin(3.14 / 180 * l)*strong / 2;
									y1 = cos(3.14 / 180 * l)*strong / 2;
									x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
									y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
									SDL_RenderDrawLine(Renderer2, x1 + Rect.x, y1 + Rect.y, x2 + Rect.x, y2 + Rect.y);
								}
								// 여기~~~~~~~~~~~~~~
								if (connect_sock != 0) {
									sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.button.x, event.button.y, strong, r, g, b);
									send(connect_sock, query, 45, 0);

								}
								strong *= 50.0 / 80;
								drag = true;
								happen = true;
								break;
							}
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

		if (CHATHAPPEN == true) {
			RenderTexture(Renderer, ChaTexture, &Chat);// 렌더러에 저장하기
			for (l = 0; l < 15; l++) {
				if (chatquery[(int)l][0] != 0) {
					han2unicode(chatquery[(int)l], unicode);
					TTF_DrawText(Renderer, Font, unicode, 10, 250 + 25 * l);		//최근 15개의 채팅을 불러옴
					ZeroMemory(unicode, sizeof(unicode));// 추가
				}
			}
			CHATHAPPEN = false;
			happen = true;
		}

		//	han2unicode(query, unicode);
		//	TTF_DrawText(Renderer, Font, unicode, 0, 50);

		if (happen == true) {
			//		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// 색깔을 흰색으로 설정해야함 그래야 지우개 역할을 하므로
			//		SDL_RenderFillRect(Renderer, &Timer);// 지우개같이 흰색으로 칠함

			SDL_RenderUpdate(Renderer, Renderer2, Renderer3, TraTexture, BoxTexture, EraTexture, PenTexture, NewTexture, ChaTexture, InpTexture, &Track, &Box, &Eraser, &Pencil, &New, &Fonts, &Chat, &InputT, Font, inputText, &strong, r, g, b);
			happen = false;

		}

	}

	SDL_DestroyTexture(InpTexture);
	SDL_DestroyTexture(UseTexture);
	SDL_DestroyTexture(RgbTexture);// 텍스쳐 파괴하기
	SDL_DestroyTexture(ChaTexture);
	SDL_DestroyTexture(BoxTexture);
	SDL_DestroyTexture(TraTexture);
	SDL_DestroyTexture(EraTexture);
	SDL_DestroyTexture(PenTexture);
	SDL_DestroyTexture(NewTexture);
	SDL_DestroyTexture(QusTexture);
	Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,10);

	int good;
	int max = 0;
	for (int j = 0; j < 4; j++)
	{
		if (score[j][1] > max)
		{
			max = score[j][1];
			good = j;
		}
	}
	if (myownnumber == j)
	{
		sprintf(query, "update catchmind.login set level = level + 1 where name = '%s'", username);
		mysql_query(cons, query);
	}
	CLS;
	printf("승자는 %s입니다. 맞춘갯수 %d개\n", friendname[good], score[good][1]);
	for (int k = 0; k < 4; k++)
	{
		printf("%s : %d개\n", friendname[k], score[k][1]);
	}
	printf("\n엔터..");
	getchar();
	return 0;// 종료
}
HWND GetConsoleHwnd(void)
{

	HWND hwndFound;         // 리턴할 핸들 값임
	WCHAR pszNewWindowTitle[MY_BUFSIZE]; // 새 윈도우 타이틀
	WCHAR pszOldWindowTitle[MY_BUFSIZE]; // 원래 윈도우 타이틀
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);		//원래 윈도우 타이틀을 저장시킴
	wsprintf(pszNewWindowTitle, "%d/%d", GetTickCount(), GetCurrentProcessId());	//새 윈도우 타이틀을 특별하게 저장시킴
	SetConsoleTitle(pszNewWindowTitle);				//새 윈도우 타이틀을 적용시킴
	Sleep(10);			//잠시 대기
	hwndFound = FindWindow(NULL, pszNewWindowTitle);		//새 윈도우 타이틀을 적용시킨 윈도우를 찾아 그 핸들값을 적용함
	SetConsoleTitle(pszOldWindowTitle);					//윈도우타이틀을 원래 타이틀로 저장함
	return(hwndFound);
}
Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;

	//Get the requested pixel
	return pixels[(y * surface->w) + x];
}
void makebmp(const char *filename, SDL_Renderer * Renderer2) {
	SDL_Surface *sshot = SDL_CreateRGBSurface(0, (1920 - 1310 / 4 - 10), (1080 - 900 / 4 - 10), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(Renderer2, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	IMG_SavePNG(sshot, filename);
	SDL_FreeSurface(sshot);
}
char* getDesktopFolderName()     //c:\Users\UserName\Desktop\ 반환

{
	ULONG ulDataType;
	HKEY hKey;
	DWORD dwToRead = 100;
	static char strPath[100];
	char strKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	//cannot use in unicode, use multi-byte code  http://blog.naver.com/stormor/70171786787
	RegOpenKeyEx(HKEY_CURRENT_USER,
		strKey,   //여기 unicode 안됨		

		0, KEY_READ, &hKey);

	RegQueryValueEx(hKey, "Desktop", NULL,

		&ulDataType, (LPBYTE)strPath, &dwToRead);

	strPath[dwToRead] = '\0';

	RegCloseKey(hKey);



	return strPath;




}
void strintrude(char *s, char *t, int i)



{
	/*문자열 s의 i 번째 위치에 문자열 t를 삽입*/



	int cnt;

	char string[300], *temp = string;



	for (cnt = 0; cnt < 300; cnt++) // init 

		string[cnt] = '\0';



	if (i < 0 && i >(int)strlen(s))

	{
		fprintf(stderr, "position is out of bounds \n");

		exit(1);

	}



	if (!strlen(s))

	{
		strcpy(s, t);

	}



	else if (strlen(t))

	{
		strcat(t, (s + i));
//		if ((s+i) != 0)
			strcpy((s + i), t);

	}

}



//4012