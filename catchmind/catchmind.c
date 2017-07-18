
/*
ĳġ ���ε� with C
������Ʈ ���� ���� : 2017-07-10
������Ʈ ���� ���� ��ǥ : 2017-07-18
����
����� -- ���콺����, ����ȭ�� ���۸�ǥ
�Ż�ȣ -- ���� ���α׷��� server, client ����
��μ� -- �׷��� ���̺귯���� ����Ͽ� ���ӳ��� ����
������ -- ��ü���� Ʋ�� ��ħ, mysql ����
��Visual Studio 2013 �̻󿡼� ����� �����մϴ�
*/
//��ó����
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// ������� ����
//�⺻ �������
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <process.h>		//process ��Ƽ�������
#include <winapifamily.h>
#include <stdbool.h>
#include <signal.h>
#include <direct.h>
#include <stdint.h>
#include <Digitalv.h>
#include <mmsystem.h>
#include <gl/GL.h>
#include <crtdbg.h>
//#include <WinSock2.h>		//�������α׷���
//Ư�� ������� (���� ��ġ) 
#include "SDL/SDL.h"			//SDL - �׷��� �������
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/han2unicode.h"
#include "mysql/mysql.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL.h"
#include "iconv.h"
#define nullptr 0
// ���̺귯�� ���� ���̺귯�������� ���� �߰����ص� �˴ϴ�.
// #pragma comment �� visual studio������ ��� ���� *�ַ�� �÷����� 64��Ʈ�� ���ּ���
#pragma comment (lib, "libmysql.lib")	//mysql���̺귯��
#pragma comment (lib, "SDL2")			//�׷��� ���̺귯�� 1
#pragma comment (lib, "SDL2main")		//�׷��� ���̺귯�� 2
#pragma comment (lib, "SDL2_image")		//�׷��� ���̺귯�� 3
#pragma comment (lib,"SDL2_ttf")
#pragma comment (lib, "SDL2_mixer.lib")	//�׷��� ���� ���̺귯�� 4
#pragma comment (lib, "ws2_32.lib")		//����(��Ʈ��ũ)���̺귯��

#pragma comment (lib, "libiconv.lib")

#pragma comment (lib, "winmm.lib")		//���� ���̺귯��

#pragma warning (disable : 4700)
#pragma warning (disable : 4244)
//#pragma warning (disable : 4101)		//������� ���� ���������Դϴ�. ��� ����

//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//������ X���� Y ���
#define RESET(X) ZeroMemory(X, sizeof(X))
#define CHOP(x) x[strlen(x) - 1] = ' '	//fgets�� ���� ���͵� �迭������. ���͸� �迭���� �������ִ°�
#define COL                   GetStdHandle(STD_OUTPUT_HANDLE)			// �ܼ�â�� �ڵ����� �ޱ�
#define BLACK                SetConsoleTextAttribute(COL, 0x0000);		// ������
#define DARK_BLUE         SetConsoleTextAttribute(COL, 0x0001);			// �Ķ���
#define GREEN                SetConsoleTextAttribute(COL, 0x0002);      // ���
#define BLUE_GREEN        SetConsoleTextAttribute(COL, 0x0003);         // û���
#define BLOOD               SetConsoleTextAttribute(COL, 0x0004);       // �˺�����
#define PURPLE               SetConsoleTextAttribute(COL, 0x0005);		// �����
#define GOLD                 SetConsoleTextAttribute(COL, 0x0006);      // �ݻ�
#define ORIGINAL            SetConsoleTextAttribute(COL, 0x0007);       // ���� ȸ�� (�⺻ �ܼ� �۾� ����)
#define GRAY                 SetConsoleTextAttribute(COL, 0x0008);      // ȸ��
#define BLUE                  SetConsoleTextAttribute(COL, 0x0009);     // �Ķ���
#define HIGH_GREEN       SetConsoleTextAttribute(COL, 0x000a);			// ���λ�
#define SKY_BLUE           SetConsoleTextAttribute(COL, 0x000b);        // �ϴû�
#define RED                   SetConsoleTextAttribute(COL, 0x000c);     // ������
#define PLUM                SetConsoleTextAttribute(COL, 0x000d);       // ���ֻ�
#define YELLOW             SetConsoleTextAttribute(COL, 0x000e);        // �����
#define WHITE                SetConsoleTextAttribute(COL, 0x000f);      // ���
#define MY_BUFSIZE 1024 // ������ �ܼ�â ������ �ִ� ���۸� ������

//����ü ����
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
//���� ������ (��� ����õ)

CRITICAL_SECTION cs;	//�̺�Ʈ
char status[4];			//���Ͽ�
char username[30];		//����� �̸�
char friendname[4][30] = { "Player 1", "Player 2", "Player 3", "Player 4" };
WSADATA wsaData;						//���� WSAStartup()�Լ��� ���̴� ����
SOCKET connect_sock, Sconnect_sock[4], listen_sock;	//���� ���Ϻ���
SOCKADDR_IN connect_addr, listen_addr;			//���� �ּ����� �����ϴ� ����
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
//�⺻ �Լ���
void gotoxy(short x, short y);
void cur(short x, short y);
void exitsignal(void);
void signalall(void);
void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
wchar_t* UTF82UNICODE(char* UTF8, int len);
char* UNICODE2UTF8(wchar_t* unicode, int len);
void usermain(void);

//--------------------- ��Ʈ��ũ �Լ��� -----------------------------------
void ErrorHandling(char *Message);				//���� ���� ��� �ϴ� �Լ�
int Connect_Server(char *ServerIP);			//���� ���� ���ִ� �Լ�
void recieve(void);								//�������� ������ �޾ƿ��� ������� �Լ�
void sendall(char *message, int c);					//�ϳ��������� ��������
int waitroom(void);							//��Ʈ��ũ ����
void Clnt_1(int v);								//���� - Ŭ���̾�Ʈ 1���
void makeroom(int *count);							//�游���(��Ʈ��ũ)
IN_ADDR GetDefaultMyIP(void);					//�� ip ���
bool exitallthread(void);
void CheckPing(void);

//--------------------- MySQL �Լ��� --------------------------------------
int sqllogin(void);						//mysql�� ����� �����͸� ���� �α����� �ϴ� �Լ�
int sqlsignup(void);						//mysql�� ���������͸� �߰��ϴ� �Լ�
void loadmysql(char mysqlip[]);	//mysql�� �����ϴ� �Լ�
char **onemysqlquery(char *query); //mysql��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
void writechating(void);					//mysql�� ä���� �Է��ϴ� �Լ�
void readchating(void);					//mysql�� ä���� �д� �Լ�
void inserttopic(void);
void sqlmakeroom(void);					//���� ����� �Լ�

// -------------------- SDL �׷��� �Լ��� ---------------------------------
void SDL_ErrorLog(const char * msg);//�����ڵ� ��� �Լ�			//�׷��ȿ����ڵ� ��� �Լ�
void IMG_ErrorLog(const char * msg);			//�̹��������ڵ� ��� �Լ�
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // �׷��ȿ��� ó�� �Լ�
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // �ؽ��Ŀ� �̹������� �پ��ϰ� �ε��ϴ� �Լ� ����
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect);	//�ؽ��ĸ� ����ϴ� �Լ� ����
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
// -------------------- ���� ���� �Լ��� ----------------------------------
void mainatitleimage(void);						//���� ����Ÿ��Ʋ ���
int maintitle(void);							//���� ����Ÿ��Ʋ ��¹� ����
void banglist(int j);						//���� �� ���
int bangchose(void);						//���� �� ��¹� ����
int chooseroom(int roomnum);
void waitroomtema();
void logintema(void);							//�α��� ������
void jointema(void);							//ȸ������ ������
LOG login(int m);								//�⺻���� �α��� �Է�
void zeroprint(int xx, int yy, int lr, int m);  //������
char checkkeyborad(char n, int togl);
void credit();									//ũ����
void Auto_Update(void);
void makeroomtema();
void roomprintwhite(int xx, int yy, int lr, int bibun, int gamo, int time, int ex);
void scannum(char *arr,int *tmpi);
//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);	//�ܾ Ȯ����
wchar_t* UTF82UNICODE(char* UTF8, int len);
int UTF8toEUCKR(char *outBuf, int outLength, char *inBuf, int inLength);
char* getDesktopFolderName();
char* UNICODE2UTF8(wchar_t* unicode, int len);
void click(int *xx, int *yy, int *lr);					//Ŭ���Լ� �ι�°, xx���� yy���� ��ȯ��
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
		if (unicode[j] == 92 || unicode[j] == 39) {// �����ڵ� 92��(��������)�� 39��(��������ǥ��) mysql���� ���� \\, \'�� �Է��ؾ��ϹǷ� ���� ó���� ���ش�
			str[i] = 92;
			str[i + 1] = unicode[j];
			i += 2;
		}
		else if (unicode[j] >= 0xac00 && unicode[j] <= 0xD7A0) {// �ϼ��� �ѱ��ϰ��
			str[i] = (unicode[j] - 40960) / (64 * 64) - 22;
			str[i + 1] = (unicode[j] - 40960) % (4096) / 64 - 128;
			str[i + 2] = (unicode[j] - 40960) % 64 - 128;
			i += 3;
		}
		else if (unicode[j] >= 0x3131 && unicode[j] <= 0x3163) {// ���������� ���
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
int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� ������� 
{
	//SDL_MAIN();
	//���� ����
	//int i, j, k, v, result;

	InitializeCriticalSection(&cs);
	unsigned int timeout = 15;
	char mainchoose = 0;
	char bangchoose;
	char chooseroomcount;
	//POINT pos;							//x, y��ǥ ǥ�� )pos.x, pos.y
	cons = mysql_init(NULL);			//mysql �ʱ�ȭ
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	char query[400];						//mysql ��ɾ ������
	char mysqlip[30] = "10.80.161.182";		//mysql ip ����ip�Դϴ�	���� ������7���� : 10.80.162.92 ������10���� : 10.80.161.182
	char data[1000][30] = { 0, };           //�ܾ����
	char nowword[30] = { 0, };              //�������� �ܾ�
	char scanword[30] = { 0, };             //���� ģ �ܾ�
	int bangnum = 0;						//�� ���� ��ȣ
	char serverreturn = 0;					//Connect_Server�� ��ȯ�� ���� �����ϴ°�
	mysql_options(cons, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
	loadmysql(mysqlip);
	memset(&wsaData, 0, sizeof(wsaData));
	memset(&connect_sock, 0, sizeof(connect_sock));
	memset(&connect_addr, 0, sizeof(connect_addr));
	srand((unsigned int)time(NULL));
	//SDL_MAINS();
	// �ʱ�ȭ ��
	signalall();

	if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("�ʱ�ȭ ����");
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
	while (1) {								//�α��� �ݺ���
		mainchoose = maintitle();				//main ȭ��
		CLS;
		if (mainchoose == 1) {				//main���� ù��°�� ����
			ConsoleL(25, 15);
			if (sqllogin() != 1)		//�α��ο� �������� ���ϸ� ó������
				continue;

			ConsoleL(50, 20);
			while (1) {						//�� �ݺ���
				gotoxy(0, 0);
				bangchoose = bangchose();	//���� ��	
				if (bangchoose == 0) {			//�游��⸦ Ŭ���ϸ� �游���� �̵�
					sqlmakeroom();
					break;
				}
				else                            //�� ���� ����
				{
					if (connectroom[bangchoose - 2].people == 4)  //4���ϋ�
						chooseroomcount = -1;

					if (connectroom[bangchoose - 2].password[0] == 0 && connectroom[bangchoose - 2].people > 0) //��й�ȣ ������� �ǳʶ�
						chooseroomcount = 1;
					else
						chooseroomcount = chooseroom(bangchoose);

					if (chooseroomcount == -1)		//return -1�� �ش� ���̾�����
					{
						continue;
					}
					else if (chooseroomcount == 0)		//return 0�� ��й�ȣ�� Ʋ����
					{
						cur(14, 1);
						printf("(��й�ȣ�� Ʋ�Ƚ��ϴ�)");
						_getch();
						CLS;
						fflush(stdin);
						continue;
					}
					else if (chooseroomcount == 1)		//return 1 �� ��й�ȣ���� ������
					{
						CHOOSEROOM = bangchoose - 2;
						serverreturn = Connect_Server(connectroom[bangchoose - 2].ip);		//���� ���� ����
						if (serverreturn == -1)
							continue;
						if (serverreturn == 3) {											//return�� 3�̸� ���� ��ư
							exitallthread();
							if (lead == true)
								closesocket(listen_sock);
							else
								closesocket(connect_sock);
							continue;
						}

						if (serverreturn == 1)												//���ϰ� 1�̸� start
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

//�Լ� �����		���� �ּ�ȭ Ctrl + M + O  ���� ���̱� Ctrl + M + L
void credit() {
	//�б��̸�
	//������
	//���߱Ⱓ
	//��Ʈ���ε� with c

	(GetAsyncKeyState(VK_LBUTTON) & 0x0001);

	gotoxy(50, 2);
	printf("��Ʈ���ε� with C project");

	gotoxy(50, 4);
	printf("���߱Ⱓ : 10��");

	gotoxy(50, 6);
	printf("����� : c���, SDL, ����, MySQL");

	gotoxy(50, 8);
	printf("�Ҽ��б� : �뱸����Ʈ�������б�");

	gotoxy(50, 10);
	printf("������ : ������, ��μ�, �����, �Ż�ȣ");

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
	printf("�� â�۹��� ũ������Ƽ�� Ŀ���� ������ǥ��-�񿵸�-�������Ǻ������ 4.0 ���� ���̼����� ���� �̿��� �� �ֽ��ϴ�.");
	gotoxy(49, 26);
	printf("Copyright(c)2017 by �Ż�ȣ, �����, ��μ�, ������");

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
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	while (1) {
		CLS;
		mysql_query(cons, "select * from catchmind.topic order by num");
		sql_result = mysql_store_result(cons);
		printf("������Դϴ�. ���͸� �Է����ּ���\n");
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			printf("%s : %s\n", sql_row[0], sql_row[1]);
			num = atoi(sql_row[0]);

		}
		mysql_free_result(sql_result);
		num++;
		getchar();
		CLS;
		printf("\n������ �Է��� �ּ���. �������� p�� �Է����ּ���\n-> %d : ", num);
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
	printf("��������������������������\n");
	printf("��  ��й�ȣ  ��  �۹̻��    �ۻ��	        ��\n"); //9~12 , 7    15~17, 7
	printf("��            ��                                ��\n");
	printf("��������������������������\n");
	printf("��  ���Ӹ��  ��  ���Ϲ�  �����׽�Ʈ  ��fps     ��\n"); //9~11 , 10   13~17, 10   19~21, 10
	printf("��������������������������\n");
	printf("���ѹ�����ð���  ��60  ��90  ��120 ���Է�:     ��\n"); //9~10 , 12~13, 15~17, 18~20
	printf("��������������������������\n");
	printf("��   ������   ��  ��5   ��10  ��15  ���Է�:     ��\n"); //9~10 , 12~13, 15~16, 18~20
	printf("��������������������������\n");
}
void roomprintwhite(int xx, int yy, int lr, int bibun, int gamo, int time, int ex) {
	WHITE if (!(yy == 7 && 9 <= xx && xx <= 12) && bibun != 1) {               //��й�ȣ
		gotoxy(18, 7);
		printf("�۹̻��");
	}
	if (!(yy == 7 && 15 <= xx && xx <= 17) && bibun != 2) {
		gotoxy(30, 7);
		printf("�ۻ��");
	}
	if (!(yy == 10 && 9 <= xx && xx <= 11) && gamo != 1) {         //���Ӹ��
		gotoxy(18, 10);
		printf("���Ϲ�");
	}
	if (!(yy == 10 && 13 <= xx && xx <= 17) && gamo != 2) {
		gotoxy(26, 10);
		printf("�����׽�Ʈ");
	}
	if (!(yy == 10 && 19 <= xx && xx <= 21) && gamo != 3) {
		gotoxy(38, 10);
		printf("��fps");
	}
	if (!(yy == 12 && 9 <= xx && xx <= 10) && time != 1) {         //������ð�
		gotoxy(18, 12);
		printf("��60");
	}
	if (!(yy == 12 && 12 <= xx && xx <= 13) && time != 2) {
		gotoxy(24, 12);
		printf("��90");
	}
	if (!(yy == 12 && 15 <= xx && xx <= 17) && time != 3) {
		gotoxy(30, 12);
		printf("��120");
	}
	if (!(yy == 12 && 18 <= xx && xx <= 20) && time != 4) {
		gotoxy(36, 12);
		printf("���Է�:");
	}
	if (!(yy == 14 && 9 <= xx && xx <= 10) && ex != 1) {         //������
		gotoxy(18, 14);
		printf("��5");
	}
	if (!(yy == 14 && 12 <= xx && xx <= 13) && ex != 2) {
		gotoxy(24, 14);
		printf("��10");
	}
	if (!(yy == 14 && 15 <= xx && xx <= 17) && ex != 3) {
		gotoxy(30, 14);
		printf("��15");
	}
	if (!(yy == 14 && 18 <= xx && xx <= 20) && ex != 4) {
		gotoxy(36, 14);
		printf("���Է�:");
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

	addr = GetDefaultMyIP();	//����Ʈ IPv4 �ּ� ������
	char * myip = inet_ntoa(addr);
	ROOM myroom = { 0, 0, 0 };

	while (1) {
		ConsoleL(28, 23);
		WHITE
		printf("��������������������������\n");
		printf("��                                              ��\n");
		printf("��            ��Ʈ���ε� �� �����              ��\n");
		printf("��          �� ip :  %s                     \n", myip);
		printf("��������������������������\n");
		printf("��    ����    ��                                ��\n");
		printf("��������������������������\n");


		gotoxy(48, 3);
		printf("��");
		disablecursor(0);
		//�����Է�
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
			printf("����, ���� �ȵ˴ϴ�");
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


			roomprintwhite(xx, yy, lr, bibun, gamo, time, ex); //���
			
			//������
			HIGH_GREEN if (yy == 7 && 9 <= xx && xx <= 12 && bibun != 1) {               //��й�ȣ
				gotoxy(18, 7);
				printf("�۹̻��");
				if (lr == 1)
					bibun = 1;
			}
			else if (yy == 7 && 15 <= xx && xx <= 17 && bibun != 2) {
				gotoxy(30, 7);
				printf("�ۻ��");
				if (lr == 1)
					bibun = 2;
			}
			else if (yy == 10 && 9 <= xx && xx <= 11 && gamo != 1) {         //���Ӹ��
				gotoxy(18, 10);
				printf("���Ϲ�");
				if (lr == 1)
					gamo = 1;
			}
			else if (yy == 10 && 13 <= xx && xx <= 17 && gamo != 2) {
				gotoxy(26, 10);
				printf("�����׽�Ʈ");
				if (lr == 1)
					gamo = 2;
			}
			else if (yy == 10 && 19 <= xx && xx <= 21 && gamo != 3) {
				gotoxy(38, 10);
				printf("��fps");
				if (lr == 1)
					gamo = 3;
			}
			else if (yy == 12 && 9 <= xx && xx <= 10 && time != 1) {         //������ð�
				gotoxy(18, 12);
				printf("��60");
				if (lr == 1)
					time = 1;
			}
			else if (yy == 12 && 12 <= xx && xx <= 13 && time != 2) {
				gotoxy(24, 12);
				printf("��90");
				if (lr == 1)
					time = 2;
			}
			else if (yy == 12 && 15 <= xx && xx <= 17 && time != 3) {
				gotoxy(30, 12);
				printf("��120");
				if (lr == 1)
					time = 3;
			}
			else if (yy == 12 && 18 <= xx && xx <= 20 && time != 4) {
				gotoxy(36, 12);
				printf("���Է�");
				if (lr == 1)
					time = 4;
			}
			else if (yy == 14 && 9 <= xx && xx <= 10 && ex != 1) {         //������
				gotoxy(18, 14);
				printf("��5");
				if (lr == 1)
					ex = 1;
			}
			else if (yy == 14 && 12 <= xx && xx <= 13 && ex != 2) {
				gotoxy(24, 14);
				printf("��10");
				if (lr == 1)
					ex = 2;
			}
			else if (yy == 14 && 15 <= xx && xx <= 17 && ex != 3) {
				gotoxy(30, 14);
				printf("��15");
				if (lr == 1)
					ex = 3;
			}
			else if (yy == 14 && 18 <= xx && xx <= 20 && ex != 4) {
				gotoxy(36, 14);
				printf("���Է�");
				if (lr == 1)
					ex = 4;
			}

			//���ֻ� ���
			YELLOW if (bibun == 1) {               //��й�ȣ
				gotoxy(18, 7);
				printf("�ܹ̻��");
			}
			else if (bibun == 2) {
				gotoxy(30, 7);
				printf("�ܻ��");
				gotoxy(6, 8);
				WHITE printf("�Է�    ��");
				gotoxy(17 + ni, 8);
				scanning(myroom.roomname, &ni);
				YELLOW
			}
			if (gamo == 1) {         //���Ӹ��
				gotoxy(18, 10);
				printf("���Ϲ�");
			}
			else if (gamo == 2) {
				gotoxy(26, 10);
				printf("�����׽�Ʈ");
			}
			else if (gamo == 3) {
				gotoxy(38, 10);
				printf("��fps");
			}
			if (time == 1) {         //������ð�
				gotoxy(18, 12);
				printf("��60");
			}
			else if (time == 2) {
				gotoxy(24, 12);
				printf("��90");
			}
			else if (time == 3) {
				gotoxy(30, 12);
				printf("��120");
			}
			else if (time == 4) {
				gotoxy(36, 12);
				printf("���Է�:");
				gotoxy(43+ti, 12);
				scannum(timedata,&ti);
				disablecursor(1);
			}
			if (ex == 1) {         //������
				gotoxy(18, 14);
				printf("��5");
			}
			else if (ex == 2) {
				gotoxy(24, 14);
				printf("��10");
			}
			else if (ex == 3) {
				gotoxy(30, 14);
				printf("��15");
			}
			else if (ex == 4) {
				gotoxy(36, 14);
				printf("���Է�:");
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
						HIGH_GREEN printf("�ۺ�й�ȣ �̻��");
					}
					else if (lr == 1) {
						WHITE
							ZeroMemory(myroom.password, sizeof(myroom.password));
						break;
					}
				}
				else {
					WHITE printf("�ۺ�й�ȣ �̻��");
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
			printf("(�˼� ���� ����. �ߺ��� �ȵǰ� ����� �ּ���)");
		}
		disablecursor(1);
		CLS;
		ConsoleL(54, 50);
		strcpy(connectroom[0].roomname, myroom.roomname);
		strcpy(connectroom[0].password, myroom.password);
		strcpy(connectroom[0].ip, myip);
		connectroom[0].people = 0;
		CHOOSEROOM = 0;
		printf("�� ������....");
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
	printf("      �������������������������������������������������\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��   [ STATUS ] :                                                                             ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      �������������������������������������������������\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��   [ STATUS ] :                                                                             ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      �������������������������������������������������\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��   [ STATUS ] :                                                                             ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      �������������������������������������������������\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n"); //92
	printf("      ��   [ STATUS ] :                                                                             ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      ��                                                                                            ��\n");
	printf("      �������������������������������������������������\n");
	printf("      ��                ��                                                        ��                ��\n");		// 4, 40		//11, 40		//42, 40		49, 40
	printf("      ��     ready      ��                                                        ��     exit       ��\n");
	printf("      ��                ��                                                        ��                ��\n");		// 4, 42		//11, 42		//42, 42		49, 42
	printf("      �������������������������������������������������\n");
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
	while (1) { //�޾ƿ� ������ ó��


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



		if (status[0] == 10)									//���� ����
		{
			status[0] = 2;
			CLS;
			turn = 0;
			printf("������ �����մϴ�.");
			if (lead == true)
			{
				sprintf(query, "delete from catchmind.chating where room = '%s'", inet_ntoa(GetDefaultMyIP()));
				mysql_query(cons, query);
				sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[������]', '%s�� ������ �����մϴ�.', '%s')", connectroom[CHOOSEROOM].roomname, connectroom[CHOOSEROOM].ip);
				mysql_query(cons, query);
				sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[������]', '/?�� ���� ��ɾ ���� �ֽ��ϴ�.', '%s')",connectroom[CHOOSEROOM].ip);
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
			printf("������ �������ϴ�.");
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
			HIGH_GREEN printf("�����ڻ���Ʈ");
			WHITE

				if (m == 2) {
					gotoxy(22, 10);
					printf("������");
				}
				else {
					gotoxy(21, 10);
					printf("ȸ������");
				}
				gotoxy(38, 10);
				printf("�ʱ�ȭ");
		}
		else if (9 <= xx && xx <= 15) {

			HIGH_GREEN

				if (m == 2) {
					gotoxy(22, 10);
					printf("������");
				}
				else {
					gotoxy(21, 10);
					printf("ȸ������");
				}
				WHITE
					gotoxy(3, 10);
				printf("�����ڻ���Ʈ");
				gotoxy(38, 10);
				printf("�ʱ�ȭ");
		}
		else if (17 <= xx && xx <= 23) {
			gotoxy(38, 10);
			HIGH_GREEN printf("�ʱ�ȭ");
			WHITE
				gotoxy(3, 10);
			printf("�����ڻ���Ʈ");
			if (m == 2) {
				gotoxy(22, 10);
				printf("������");
			}
			else {
				gotoxy(21, 10);
				printf("ȸ������");
			}
		}
	}
	else {
		WHITE
			gotoxy(3, 10);
		printf("�����ڻ���Ʈ");
		if (m == 2) {
			gotoxy(22, 10);
			printf("������");
		}
		else {
			gotoxy(21, 10);
			printf("ȸ������");
		}
		gotoxy(38, 10);
		printf("�ʱ�ȭ");
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
LOG login(int m) { // 1�̸� �α��� 2�̸� ȸ������ �ʼ�!!
				   //���� ���� �ڵ�ϱ� ȸ�������̶� �α��ο� �� �����ñ�
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
	/*�г��� ����*/
	disablecursor(0);
	if (m == 2) {
		ConsoleL(25, 15);
		jointema();
		gotoxy(9, 13);
		printf("�г��� �������� ���콺 ��� �Ұ���");
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
			//printf("%3d %3d", xx, yy); //login 19~23 5~7      ������ ����Ʈ 1~7 9~11    ȸ������ 9~15      �ʱ�ȭ 17~23
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
						return user; //�Լ��� ������ ���ΰ��� �����ϸ� �˾Ƽ� �ɷ�����
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
	/*��й�ȣ ��ȣȭ ����*/
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
						break;  //�Լ��� ������ ���ΰ��� �����ϸ� �˾Ƽ� �ɷ�����
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
					printf("�����ڻ���Ʈ");

					gotoxy(21, 10);
					printf("ȸ������");

					gotoxy(38, 10);
					printf("�ʱ�ȭ");
				}
				else if (m == 2 && 19 <= xx && xx <= 23 && 3 <= yy && yy <= 7) {
					disablecursor(1);
					HIGH_GREEN
						gotoxy(39, 5);
					printf("ȸ������");
					WHITE
						gotoxy(3, 10);
					printf("�����ڻ���Ʈ");
					gotoxy(22, 10);
					printf("������");
					gotoxy(38, 10);
					printf("�ʱ�ȭ");
				}
				else {
					disablecursor(1);
					WHITE
						gotoxy(3, 10);
					printf("�����ڻ���Ʈ");
					if (m == 2) {
						gotoxy(22, 10);
						printf("������");
						gotoxy(39, 5);
						printf("ȸ������");
					}
					else {
						gotoxy(21, 10);
						printf("ȸ������");
						gotoxy(41, 6);
						printf("login");
					}
					gotoxy(38, 10);
					printf("�ʱ�ȭ");

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
	while (cnt != i - 1) { //�ܾ ������ ���� ����

		cnt = 0;

		for (i = 0; scanword[i] != 0; i++) //�ʱ�ȭ
			scanword[i] = 0;

		fgets(scanword, sizeof(scanword), stdin); // ���� ä�ü����� ����

		for (i = 0; nowword[i] != 0; i++) //��
			if (nowword[i] == scanword[i])
				cnt++;

		if (scanword[i] != 0) //��(��������)
			cnt = 0;

	}
	printf("��");
} //�˾Ƽ� ������
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
		sprintf(query, "insert into catchmind.chating (name, mean) values ('������', '%S')", buffer);
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
void loadmysql(char mysqlip[])	//MYSQL ���� �ҷ�����
{
	CLS;
	printf("�����ͺ��̽� �ҷ����� �õ���...");
	if (cons == NULL) //cons�� �ʱ�ȭ�� ��������
	{
		fprintf(stderr, "%s\n", mysql_error(cons));		//���� ����Ʈ, printf("%s")�� ����
		printf("ä�� ���� �ҷ����� ���� (�ʱ�ȭ ����)\n");
		Sleep(1000);
		exit(1);
	}
	else
		printf("1");
	if (mysql_real_connect(cons, mysqlip, "root", "tbvjaos15", NULL, 0, NULL, 0) == NULL) //mysql������ ����
	{
		printf("\b����... \n������ �������� �ʽ��ϴ�.\n");
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("���ο� ip�� ������ �ּ���.\n������ 10�� �ȵǸ� ������7 ip�� 10.80.162.92�Դϴ�.\n->");
		scanf("%s", mysqlip);
		getchar();
		loadmysql(mysqlip);					//����Լ� ȣ��
	}
	else {
		printf("\b2");
		printf("\n����");
		mysql_set_character_set(cons, "euckr");
		CLS;
		mysql_select_db(cons, "catchmind");
	}

	return;
}
char **onemysqlquery(char *query) {		//mysql ��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
	mysql_query(cons, query);
	return mysql_fetch_row(mysql_store_result(cons));

}
POINT MouseClick(void)			//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ�
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
void ConsoleL(int x, int y) {			//�ܼ�â�� ũ�⸦ �������ִ� �Լ�
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
	Style = GetWindowLong(GetConsoleWindow(), GWL_STYLE); //�θ��������� ������Ÿ�� ���� Style�� ����.
	Style &= ~WS_THICKFRAME;							  //������ â���� ����
	Style &= ~WS_MAXIMIZEBOX;							  //������ �ִ�ȭ ����
	SetWindowLong(GetConsoleWindow(), GWL_STYLE, Style); //���� �ٲ� ������ ��Ÿ���� �θ� �������� �����콺Ÿ�Ͽ� ������ ����.

}
void disablecursor(bool a) {

	CONSOLE_CURSOR_INFO ConsoleCursor;			// �ܼ�Ŀ�� ���� ����ü
	if (a == true) {
		ConsoleCursor.bVisible = false;				// true ���� , false �Ⱥ���
		ConsoleCursor.dwSize = 1;
	}// Ŀ�� ������
	else
	{
		ConsoleCursor.bVisible = true;				// true ���� , false �Ⱥ���
		ConsoleCursor.dwSize = 10;
	}
	SetConsoleCursorInfo(COL, &ConsoleCursor);	// ����
}
void ErrorHandling(char *Message) {
	system("cls");
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);
}
int Connect_Server(char *ServerIP) { //���� ���� ���ִ� �Լ�
	char query[100];
	printf("%s�� ȣ����\n", ServerIP);
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock������ ���� �Ҵ�
	connect_addr.sin_family = AF_INET;				//������ ������ �ּ� ����
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //���� IP
	connect_addr.sin_port = htons(5555);					 //���� ��Ʈ
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) { //������ ����
		ErrorHandling("connect() error");
		return -1;
	}
	threads[0] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, NULL, 0, NULL); //�������� �����͸� �޾ƿ��� ������ ����
	CLS;

	sprintf(query, "update catchmind.room set people = people + 1 where ip = '%s'", ServerIP);
	mysql_query(cons, query);
	sprintf(query, "player   connect %s", username);
	send(connect_sock, query, 45, 0);
	Sleep(200);

	return waitroom();
}
void recieve(void) { //�������� ������ �޾ƿ��� ������� �Լ�
	char message[50] = { 0, };
	int i = 0;
	while (1) {

		if (recv(connect_sock, message, 45, 0) > 0) { //�������� �����͸� �޾ƿ� message������ ����
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
				//		printf("1�� ��� ���� : %s", topics[0]);
			}
			else if (strncmp(message, "topic 2", 7) == 0)
			{
				sscanf(message, "topic 2 %s", topics[1]);
				cur(0, 9);
				//		printf("2�� ��� ���� : %s", topics[1]);
			}
			else if (strncmp(message, "topic 3", 7) == 0)
			{
				sscanf(message, "topic 3 %s", topics[2]);
				cur(0, 10);
				//		printf("3�� ��� ���� : %s", topics[2]);
			}
			else if (strncmp(message, "topic 4", 7) == 4)
			{
				sscanf(message, "topic 4 %s", topics[3]);
				cur(0, 11);
				//		printf("4�� ��� ���� : %s", topics[3]);
			}

		}
		//	Sleep(100);
	}

}
int sqllogin(void) {
	char query[100];
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	LOG user;								//����� ���� ����ü
	while (1) {

		int check = 0;

		user = login(1);							//login �Լ��� ���
		if (user.id[0] == 0)
		{
			fflush(stdin);
			check = sqlsignup();
			gotoxy(2, 3);
			if (check == 1)
				printf("               (ȸ������ ����)           ");
			else if (check == -1)
				printf("             (���̵� �ߺ��˴ϴ�)              ");
			else if (check == -2)
				printf("             (�г����� �ߺ��˴ϴ�)              ");
			else
				printf("	      (�ߺ� ���� �ֽ��ϴ�)           ");
		}
		else {
			sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (mysql_fetch_row(sql_result) == NULL)									//�؈� id�� ������ 
			{
				gotoxy(2, 3);
				printf("           ���̵� �������� �ʽ��ϴ�     ");
				//		mysql_free_result(sql_result);

			}
			else {
				mysql_free_result(sql_result);
				sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass);
				mysql_query(cons, query);	//password�� DB�� ��ȣȭ�Ǿ� �־ ���� ���Ҷ��� ���� ��ȣȭ�ؼ� �񱳸���
				sql_result = mysql_store_result(cons);


				if ((sql_row = mysql_fetch_row(sql_result)) == NULL)
				{
					gotoxy(2, 3);
					printf("              ��й�ȣ�� Ʋ�Ƚ��ϴ�        ");
					mysql_free_result(sql_result);

				}
				else {
					mysql_free_result(sql_result);
					strcpy(username, sql_row[1]);
					sprintf(query, "title %s�� ��Ʈ���ε忡 ���Ű��� ȯ���մϴ�!", username);
					system(query);
					return 1; //�α��� ����
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
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����


	if (user.name[0] == 0)
		return 0;
	/*
	sprintf(query2, "select * from catchmind.login where name = '%s'", user.name);	//�̸��� DB���� ã��
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -2;
	}*/
	ZeroMemory(query2, sizeof(query2));
	sprintf(query2, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -1;
	}
	mysql_free_result(sql_result);
	sprintf(query3, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);

	if (!(mysql_query(cons, query3)))											//		 password�� mysql���� �����ϴ� ��ȣȭ ������.
		return 1; //����
	else
		return 0; //����

}
void mainatitleimage(void) {
	SetConsoleTitle("��Ʈ���ε� with C      Powered by  C Meister TEAM");
	WHITE
		gotoxy(136, 1);
	printf("������� ON   ");
	gotoxy(6, 3);
	printf("        ��              ������      ������  ��        ����    ��      ������                                           ����"); gotoxy(6, 4);
	printf("    ������  ��      ��              ��      ��  ��      ��      ��  ��      ��                                                 ��      ��"); gotoxy(6, 5);
	printf("                ��      ������      ��      ��  ��      ��      ��  ��      ��              ��      ��   ��    ��    ��      ��"); gotoxy(6, 6);
	printf("      ����    ��      ��              ��      ��  ���      ����    ��      ������      ��      ��       ����  ��      ��"); gotoxy(6, 7);
	printf("    ��      ��  ��      ������      ��      ��  ��                                          ��  ��  ��   ��    ��    ���    ��"); gotoxy(6, 8);
	printf("    ��      ��  ��                      ��      ��  ��        ��                                ��  ��  ��   ��    ��    ��  ��    ��      ��"); gotoxy(6, 9);
	printf("      ����    ��    ��������    ������  ��        �������    ��������      ��  ��     ��    ��    ��  ��      ����");


	gotoxy(12, 20);
	printf("����������                      ����������                      ����������"); gotoxy(12, 21);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 22);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 23);
	printf("��  ���� ����   ��                      ��  ���� �߰�   ��                      ��    ũ����    ��"); gotoxy(12, 24);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 25);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 26);
	printf("����������                      ����������                      ����������");

	gotoxy(90, 34);
	printf("���--��������--��������������������"); gotoxy(90, 35);
	printf("��                                                  ��");  gotoxy(90, 36);
	printf("��  �� ���� ���� ����� �ؾ��� ������ (EnterŰ �Է�)��"); gotoxy(90, 37);
	printf("��                                                  ��"); gotoxy(90, 38);
	printf("��  �� ������ ��������� �ǵ�� (Ŭ��)              ��"); gotoxy(90, 39);
	printf("��                                                  ��"); gotoxy(90, 40);
	printf("��  �� VPN�� ���� ���� �����ּ���                   ��"); gotoxy(90, 41);
	printf("��                                                  ��"); gotoxy(90, 42);
	printf("��  �� ���� ��������� ������ �߻��Ҽ� �ֽ��ϴ�     ��"); gotoxy(90, 43);
	printf("��                                                  ��"); gotoxy(90, 44);
	printf("��  �� �ɰ��� ���� �߻��� ���� 13����               ��"); gotoxy(90, 45);
	printf("��                                                  ��"); gotoxy(90, 46);
	printf("����������������������������");

}
int maintitle(void) { //���� ����Ÿ��Ʋ ���
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
						printf("������� ON   ");
					else
						printf("������� OFF");

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
			HIGH_GREEN printf("���� ����");
		}
		else if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 1)
			return 1;
		else if (27 <= xx && xx <= 33 && 21 <= yy && yy <= 25 && lr == 0)
		{
			gotoxy(56, 23);
			HIGH_GREEN printf("���� �߰�");
		}
		else if (27 <= xx && xx <= 33 && 21 <= yy && yy <= 25 && lr == 1)
		{
			return 2;
		}
		else if (47 <= xx && xx <= 53 && 21 <= yy && yy <= 25 && lr == 0)
		{
			gotoxy(98, 23);
			HIGH_GREEN printf("ũ����");
		}
		else if (47 <= xx && xx <= 53 && 21 <= yy && yy <= 25 && lr == 1)
		{
			return 3;
		}
		else if (46 <= xx && xx <= 70 && yy == 36 && lr == 0) {
			gotoxy(94, 36);
			HIGH_GREEN printf("�� ���� ���� ����� �ؾ��� ������ (EnterŰ �Է�)");
			gotoxy(94, 38);
			WHITE printf("�� ������ ��������� �ǵ�� (Ŭ��)");
		}
		else if (46 <= xx && xx <= 70 && yy == 38 && lr == 0) {
			gotoxy(94, 38);
			HIGH_GREEN printf("�� ������ ��������� �ǵ�� (Ŭ��)");
			gotoxy(94, 36);
			WHITE printf("�� ���� ���� ����� �ؾ��� ������ (EnterŰ �Է�)");
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
			printf("�� ���� ���� ����� �ؾ��� ������ (EnterŰ �Է�)");
			gotoxy(94, 38);
			printf("�� ������ ��������� �ǵ�� (Ŭ��)");
			gotoxy(16, 23);
			printf("���� ����");
			gotoxy(56, 23);
			printf("���� �߰�");
			gotoxy(98, 23);
			printf("ũ����");
			gotoxy(136, 1);
			if (bae == 1)
				printf("������� ON   ");
			else
				printf("������� OFF");
		}


	}
	CLS;
}
void click(int *xx, int *yy, int *lr) {//���콺���� 2�� �������� �޴´�

	HANDLE       hIn, hOut;
	DWORD        dwNOER;
	INPUT_RECORD rec;


	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER); // �ܼ�â �Է��� �޾Ƶ���.
	if (rec.EventType == MOUSE_EVENT) {// ���콺 �̺�Ʈ�� ���
		int mouse_x = rec.Event.MouseEvent.dwMousePosition.X; // X�� �޾ƿ�
		int mouse_y = rec.Event.MouseEvent.dwMousePosition.Y; // Y�� �޾ƿ�

		*xx = mouse_x / 2;
		*yy = mouse_y;

		if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // ���� ��ư�� Ŭ���Ǿ��� ���
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
		printf("\n"); //��ǥ�������� ��ĭ �о
	printf("                ��������������������������������\n"); //�游��� 9 ~ 22 , 2
	printf("                ��          �游���          ��          ��������          ��\n"); //�������� 24 ~ 37 , 2
	printf("                ��������������������������������\n");
	printf("                ��                          ����                          ��\n"); //���� 9 ~ 37, 4
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��1 9 ~ 22 , 6 ~ 8 
	printf("                ��                            ��                            ��\n"); //��2 24 ~ 37 , 6 ~ 8
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��3 9 ~ 22 , 10 ~ 12  
	printf("                ��                            ��                            ��\n"); //��4 24 ~ 37 ,10 ~ 12 
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��5 9 ~ 22 , 14 ~ 16
	printf("                ��                            ��                            ��\n"); //��6 24 ~ 37 , 14 ~ 16
	printf("                ��                            ��                            ��\n");
	printf("                ��������������������������������\n");

}
void banglist(int j) {
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	short i = 0;
	mysql_query(cons, "select ip, name, password, people from catchmind.room");
	sql_result = mysql_store_result(cons);

	memset(connectroom, 0, sizeof(connectroom));

	while (i < 6) { //ȭ�������

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

		if (j == i) {     //����������
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

		if (j == i) {  //������
			HIGH_GREEN printf("%-7s", sql_row[1]);
		}
		else {
			WHITE printf("%-7s", sql_row[1]);
		}
		if (i % 2 == 0)
			cur(25, 8 + (i * 2));
		else
			cur(55, 8 + ((i / 2) * 4));

		if (j == i) {   //�ο���
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
			if (9 <= xx && xx <= 22 && 2 == yy)			//�游���
				return 0;
			else if (24 <= xx && xx <= 37 && 2 == yy) {//��������
				for (c = 3; c > 0; c--)
					for (b = 0; b < 6; b++)
						if (connectroom[b].people == c && connectroom[b].password[0] == 0)
							return b + 2;
				for (c = 3; c > 0; c--)
					for (b = 0; b < 6; b++)
						if (connectroom[b].people == c)
							return b + 2;
			}
			else if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)	//�� 1
				return 2;
			else if (24 <= xx && xx <= 37 && 6 <= yy && yy <= 8)	//�� 2
				return 3;
			else if (9 <= xx && xx <= 22 && 10 <= yy && yy <= 12)	//�� 3
				return 4;
			else if (24 <= xx && xx <= 37 && 10 <= yy && yy <= 12)	//�� 4
				return 5;
			else if (9 <= xx && xx <= 22 && 14 <= yy && yy <= 16)	//�� 5
				return 6;
			else if (24 <= xx && xx <= 37 && 14 <= yy && yy <= 16)	//�� 6
				return 7;
		}
		else if (lr == 0) {
			if (9 <= xx && xx <= 22 && 2 == yy) {			//�游���
				gotoxy(28, 2);
				HIGH_GREEN printf("�游���");
				WHITE
					gotoxy(58, 2);
				printf("��������");
			}

			else if (24 <= xx && xx <= 37 && 2 == yy) {		//��������
				gotoxy(58, 2);
				HIGH_GREEN printf("��������");
				WHITE
					gotoxy(28, 2);
				printf("�游���");
			}

			else if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)	//�� 1
				j = 0;
			else if (24 <= xx && xx <= 37 && 6 <= yy && yy <= 8)	//�� 2
				j = 1;
			else if (9 <= xx && xx <= 22 && 10 <= yy && yy <= 12)	//�� 3
				j = 2;
			else if (24 <= xx && xx <= 37 && 10 <= yy && yy <= 12)	//�� 4
				j = 3;
			else if (9 <= xx && xx <= 22 && 14 <= yy && yy <= 16)	//�� 5
				j = 4;
			else if (24 <= xx && xx <= 37 && 14 <= yy && yy <= 16)	//�� 6
				j = 5;
			else {
				WHITE
					j = -1;
				gotoxy(28, 2);
				printf("�游���");
				gotoxy(58, 2);
				printf("��������");
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
		printf("��������������������������\n");
	printf("��                                              ��\n");
	printf("��              ��Ʈ���ε� �� ����              ��\n");
	printf("��          ���� ip :  %s\n", connectroom[roomnum].ip);
	printf("��������������������������\n");
	printf("��  �� ����   ��                              ���\n");			// x = 17 y = 5
	printf("��������������������������\n");
	printf("��  Password  ��                              ���\n");
	printf("��������������������������\n");
	gotoxy(48, 3);
	printf("��");
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
		printf("��������������������������\n");
	printf("��                                              ��\n");
	printf("��            ��Ʈ���ε� ������ �α���          ��\n");
	printf("��\n");
	printf("��������������������������\n");
	printf("��     ID     ��                    ��          ��\n");
	printf("��������������������   login  ��\n");
	printf("��  Password  ��                    ��          ��\n");
	printf("��������������������������\n");
	printf("��              ��              ��              ��\n");
	printf("�� �����ڻ���Ʈ ��   ȸ������   ��    �ʱ�ȭ    ��\n");
	printf("��              ��              ��              ��\n");
	printf("��������������������������\n");
	gotoxy(48, 3);
	printf("��");
}
void jointema(void) {
	WHITE
		printf("��������������������������\n");
	printf("��  ȸ������ : �г����� ���� Ư������ �ȵ˴ϴ�  ��\n");
	printf("��������������������������\n");
	printf("��    �г���  ��                    ��          ��\n");
	printf("��������������������          ��\n");
	printf("��     ID     ��                    �� ȸ������ ��\n");
	printf("��������������������          ��\n");
	printf("��  Password  ��                    ��          ��\n");
	printf("��������������������������\n");
	printf("��              ��              ��              ��\n");
	printf("�� �����ڻ���Ʈ ��    ������    ��    �ʱ�ȭ    ��\n");
	printf("��              ��              ��              ��\n");
	printf("��������������������������\n");
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
	printf("���--������Ʈ���--���������������������");
	cur(50, 34);
	printf("--���� : %s--", version);
	while ((sql_row = mysql_fetch_row(sql_result)) != 0)
	{
		cur(10, 35 + (i * 3));
		printf("��                                                        ��");
		cur(10, 36 + (i * 3));
		printf("��                                                        ��");
		cur(10, 37 + (i * 3));
		printf("��                                                        ��");
		cur(20, 36 + (i * 3));
		printf("%s :", sql_row[0]);
		cur(30, 36 + (i * 3));
		printf("%s", sql_row[1]);
		strcpy(serverversion, sql_row[0]);
		i++;
	}

	cur(10, 35 + (i * 3));
	printf("�������������������������������");
	if (strcmp(serverversion, version) != 0)
	{
		cur(0, 0);
		printf("���� ������ ���� ������ �ٸ��ϴ�. ������Ʈ �Ͻðڽ��ϱ�? �ȵǸ� �����ڱ������� �������ּ��� ���� ���� : %s �� ���� : %s\n�ȵǸ� Ȩ���������� �ٿ�޾� �������ּ���\n 1. �� 2. �ƴϿ�", serverversion, version);
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
				fprintf(ftp, "����\n");
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
	listen_sock = socket(PF_INET, SOCK_STREAM, 0); // ���� ���� �� ���Ͽ� ���� ������ listen_sock������ ����					
	if (listen_sock == INVALID_SOCKET) {
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("socket() error");
	}
	printf("���� ���� �Ϸ�!\n");
	memset(&listen_addr, 0, sizeof(listen_addr)); // ������ �ּ� �ʱ�ȭ
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ������ �ּҸ� �� �ּҷ� (�Ƹ���)
	listen_addr.sin_port = htons(5555); // ���� ��Ʈ 
	printf("�ּ� ���� �Ϸ�!\n");
	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) {// ���ݱ��� ������ �ּҸ� listen_sock�� bind()�� ����
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("bind() error");
	}
	printf("bind() �Ϸ�!\n");
	if (listen(listen_sock, 5) == SOCKET_ERROR) {	// Ŭ���̾�Ʈ�� �����Ҷ� ���� ��ٸ�
		sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
		mysql_query(cons, query);
		ErrorHandling("listen() error");
	}
	printf("listen() �Ϸ�!\n");
	sockaddr_in_size = sizeof(connect_addr);
	*count = 1;
	while (!turn) {
		if (Sconnect_sock[SOCKETCOUNT] == 0) {
			Sconnect_sock[SOCKETCOUNT] = accept(listen_sock, (SOCKADDR*)&connect_addr, &sockaddr_in_size); // �����ϸ� accept() ����
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

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//ȣ��Ʈ �̸� ������
	{
		return addr;
	}
	HOSTENT *ptr = gethostbyname(localhostname);//ȣ��Ʈ ��Ʈ�� ������
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 �ּ� Ÿ���� ��
		{
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);//�޸� ����
			break;//�ݺ��� Ż��
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
	printf("����� Ȯ�� ���Դϴ�.\n");
	char query[100];
	sprintf(query, "first   ping %ld", clock() + 1);
	send(connect_sock, query, 45, 0);
	//printf("%s\n", query);
	printf("�� üũ��....");
	while (Ping == 0)
	{

	}
	now = clock() - Ping + 1;
	printf("����. �� ���� %ldms�Դϴ�\n�� ���� ��ȣ = %d", now, myownnumber);
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
				printf("���� �ȵ�.");
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
					printf("����.");
			}
			else {
				HIGH_GREEN
					printf("�� : %hd", Userping[i]);
			}
		}
	}
	WHITE
		printf("\n���ӽ���");
	Sleep(1000);



}
void SDL_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, SDL_GetError());
	return;
}
void IMG_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, IMG_GetError());
	return;
}
void TTF_ErrorLog(const char * msg) {
	printf("%s Error: %s\n", msg, TTF_GetError());
	return;
}
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step) {// ���� ó�� �Լ�
	SDL_ErrorLog(msg);//�����ڵ� ���
	switch (step) {
	case 3:
		SDL_DestroyRenderer(Renderer);// SDL ������ �ı�
	case 2:
		SDL_DestroyWindow(Window);//SDL ������ �ı�
	case 1:
		SDL_Quit();//SDL ����
		getchar();//�����ڵ� Ȯ���ϱ� ���� �ܼ�â ���
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
		SDL_DestroyRenderer(Renderer3);// SDL ������ �ı�
	case 8:
		SDL_DestroyWindow(Window3);
	case 7:
		SDL_DestroyRenderer(Renderer2);
	case 6:
		SDL_DestroyWindow(Window2);//SDL ������ �ı�
	case 5:
		SDL_DestroyRenderer(Renderer);
	case 4:
		SDL_DestroyWindow(Window);
	case 3:
		TTF_CloseFont(topicFont);
			TTF_CloseFont(Font);
	case 2:
		TTF_Quit();//TTF ����
	case 1:
		SDL_Quit();
	}
	getchar();
	return;
}
void SDL_RenderDrawEdge(SDL_Renderer* Renderer, SDL_Rect * Rect, bool clicks) {
	if (clicks == true)
		SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 64);// Ŭ������ ���� ������ �Ķ�
	else
		SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 8);// ���콺�� �÷��� ���� ���� �� ���� �Ķ�
	SDL_Rect Edge = { 0 };
	Edge.x = Rect->x - 10;
	Edge.y = Rect->y - 10;
	Edge.w = Rect->w + 20;
	Edge.h = Rect->h + 20;
	SDL_RenderDrawRect(Renderer, &Edge);// �Ķ� �׵θ��� �׸�
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);// ���İ��� �̿��� �����ϰ� ĥ�ϴ� ���� �����ϰ� ��
	SDL_RenderFillRect(Renderer, &Edge);// ���� ������ �Ķ� �簢�� �׸�
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);// �����ϰ� ĥ�ϴ� �� �Ұ����ϰ� ��
}
void SDL_RenderRemoveEdge(SDL_Renderer* Renderer, SDL_Rect * Rect) {
	SDL_Rect Edge = { 0 };
	Edge.x = Rect->x - 10;
	Edge.y = Rect->y - 10;
	Edge.w = Rect->w + 20;
	Edge.h = Rect->h + 20;
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// �Ͼ��
	SDL_RenderFillRect(Renderer, &Edge);// ���� ������ �Ķ� �簢�� �׸�
}
void SDL_FontUpdate(SDL_Renderer * Renderer, SDL_Rect* Font, SDL_Rect Track, float strong, int r, int g, int b) {
	if (clicks.pencil == true) {
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������� ����
		SDL_RenderFillRect(Renderer, Font);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
		Font->x = Track.x + 35 - strong / 2;// ���� ������ ��ǥ�� �ٽü���
		Font->y = Track.y - 50 - strong / 2;// ������ ������ ��ǥ�� �ٽü���
		Font->h = Font->w = strong;// ���� �ٽü���
		SDL_SetRenderDrawColor(Renderer, r, g, b, 0);//������ ����
		SDL_RenderFillRect(Renderer, Font);// ��Ʈ�� �����
		if (strong > 5) {// if���� ���Ⱑ 5�ʰ��� ��츸 �̶�� �������� ���Ⱑ 5�����̸� �׵θ������� ���������� ���ϼ��� �ֱ� ����
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);// ���������� ����
			SDL_RenderDrawRect(Renderer, Font);// �׵θ��� �׷���
		}
	}
	else if (clicks.eraser == true) {
		strong *= 80 / (float)50.0;
		int l;
		int x1 = sin(3.14 / 180 * 0)*strong / 2, y1 = cos(3.14 / 180 * 0)*strong / 2, x2, y2;// ���� ����ϱ� ���� ������ ����
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������� ����
		Font->w += 2;//���� ��Ȯ�� ���� �ƴ϶� ��¦ ��������
		Font->h += 2;
		SDL_RenderFillRect(Renderer, Font);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
		Font->x = Track.x + 35 - strong / 2;// ���� ������ ��ǥ�� �ٽü���
		Font->y = Track.y - 50 - strong / 2;// ������ ������ ��ǥ�� �ٽü���
		Font->h = Font->w = strong;
		for (l = 0; l < 180; l++) {
			x1 = sin(3.14 / 180 * l)*strong / 2;
			y1 = cos(3.14 / 180 * l)*strong / 2;
			x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
			y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
			SDL_RenderDrawLine(Renderer, x1 + Font->x + strong / 2, y1 + Font->y + strong / 2, x2 + Font->x + strong / 2, y2 + Font->y + strong / 2);
		}
		if (strong > 5) {// if���� ���Ⱑ 5�ʰ��� ��츸 �̶�� �������� ���Ⱑ 5�����̸� �׵θ������� ���������� ���ϼ��� �ֱ� ����
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);// ���������� ����
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
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file) { // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG���ϰ� PNG���� �ε� ����
	if (IMG_Init(imgFlags) != imgFlags) {//IMG �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ����
		IMG_ErrorLog("IMG_Init");
		return nullptr;//�������� ��ȯ
	}
	SDL_Surface* Surface = IMG_Load(file);//���ǽ��� �̹����ε�
	if (Surface == nullptr) {//���ǽ��� �̹����ε尡 �ȵǸ� 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//���ǽ��κ��� �ؽ��� ����
	SDL_FreeSurface(Surface);// ���ǽ� �޸�����
	if (Texture == nullptr) {// �ؽ��� ���� ���н� if������
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// ���� �ڵ� ���
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	IMG_Quit();// IMG ����
	return Texture;// Texture������ ��ȯ
}
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip) { // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;// JPG���ϰ� PNG���� �ε� ����
	if (IMG_Init(imgFlags) != imgFlags) {//IMG �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ����
		IMG_ErrorLog("IMG_Init");
		return nullptr;//�������� ��ȯ
	}
	SDL_Surface* Surface = IMG_Load(file);//���ǽ��� �̹����ε�
	if (Surface == nullptr) {//���ǽ��� �̹����ε尡 �ȵǸ� 
		IMG_ErrorLog("IMG_Load");
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, r, g, b));// r,g,b���� �ش��ϴ� ������ ����� �Լ���
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);//���ǽ��κ��� �ؽ��� ����
	SDL_FreeSurface(Surface);// ���ǽ� �޸�����
	if (Texture == nullptr) {// �ؽ��� ���� ���н� if������
		SDL_ErrorLog("SDL_CreateTextureFromSurface");// ���� �ڵ� ���
		IMG_Quit();// IMG ����
		return nullptr;// �������� ��ȯ
	}
	IMG_Quit();// IMG ����
	return Texture;// Texture������ ��ȯ
}
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect) {//�ؽ��ĸ� ����ϴ� �Լ� ����
	SDL_Rect Src;// ���簢�� ����
	Src.x = 0;// ���簢���� ������ �������� x��ǥ�ʱ�ȭ
	Src.y = 0;// ���簢���� ������ �������� y��ǥ�ʱ�ȭ
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h); // Texture�� �ʺ�� ���� ������ Src.w, Src.h�� ����
	SDL_Rect Dst;
	Dst.x = Rect->x;//�Ű�����x�� ������ �������� x��ǥ�� ����
	Dst.y = Rect->y;//�Ű�����y�� ������ �������� y��ǥ�� ����
	Dst.w = Rect->w;//�Ű�����w�� ���簢���� �ʺ� ����
	Dst.h = Rect->h;//�Ű�����h�� ���簢���� ���̿� ����
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst);//Src�� ������ ������ �ִ� Texture�� Dst�� ������ ���� Texture �� ��ȯ�Ͽ� �������� ����
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
		if (pencil == true && drag == false) {//eraser ���¿��� Ŭ���� ���
			ReceiveRect.x = x - strong / 2;
			ReceiveRect.y = y - strong / 2;// ���⸸ŭ�� �簢���� ����
			ReceiveRect.w = ReceiveRect.h = strong;// ���� ����
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			SDL_RenderFillRect(Renderer4, &ReceiveRect);// �������� �׸�
	//		SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == false) {
			strong *= 80 / (float)50.0;
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);
			int x1, y1, x2, y2, l;
			ReceiveRect.x = x;
			ReceiveRect.y = y;// ���⸸ŭ�� �簢���� ����
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
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// ���������� ���̸� ��Ÿ����� ������ ����. �̶� ������ ���� ���� ���� �巡���� ���� ���� ����
			if (length == 0) return;
			i = (x - (ReceiveRect.x + ReceiveRect.w / 2)) / length;// i�� ������ x��ǥ�� ���̸� ���̷� ���� ����.
			j = (y - (ReceiveRect.y + ReceiveRect.h / 2)) / length;// j�� ������ y��ǥ�� ���̸� ���̷� ���� ����.
			k = 0;// while���ȿ� �� ���� �ʱ�ȭ.
			xpos = ReceiveRect.x + ReceiveRect.w / 2 - strong / 2;// ���������� x��ǥ�� ���� ����
			ypos = ReceiveRect.y + ReceiveRect.h / 2 - strong / 2;// ���������� y��ǥ�� ���� ����
			ReceiveRect.w = ReceiveRect.h = strong;// ���⼳��
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
				ReceiveRect.x = xpos + k*i;// ���� ���� ������ �������� x��ǥ�� ���� 
				ReceiveRect.y = ypos + k*j;// ���� ���� ������ �������� y��ǥ�� ����

				SDL_RenderFillRect(Renderer4, &ReceiveRect);//�簢�� �������� ����
			}
		//	SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == true) {

			strong *= 80 / (float)50.0;
			float i = 0, j = 0, k = 0, l = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// ���������� ���̸� ��Ÿ����� ������ ����. �̶� ������ ���� ���� ���� �巡���� ���� ���� ����
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);// ���찳�ϱ� ������ �Ͼ������	
			if (length == 0) return;
			i = (x - ReceiveRect.x) / length;// i�� ������ x��ǥ�� ���̸� ���̷� ���� ����.
			j = (y - ReceiveRect.y) / length;// j�� ������ y��ǥ�� ���̸� ���̷� ���� ����.
			k = 0;// while���ȿ� �� ���� �ʱ�ȭ.
			xpos = ReceiveRect.x;// ���������� x��ǥ�� ���� ����
			ypos = ReceiveRect.y;// ���������� y��ǥ�� ���� ����
			ReceiveRect.w = ReceiveRect.h = strong;// ���⼳��
			for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
				ReceiveRect.x = xpos + k*i;// ���� ���� �߽� �� x��ǥ�� ���� 
				ReceiveRect.y = ypos + k*j;// ���� ���� �߽� �� y��ǥ�� ����
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
	SDL_SetRenderDrawColor(Renderer2, r, g, b, 0);// ������
	RenderTexture(Renderer, TraTexture, Track);// �������� �����ϱ�
	RenderTexture(Renderer, BoxTexture, Box);// �������� �����ϱ�
	RenderTexture(Renderer, EraTexture, Eraser);// �������� �����ϱ�
	RenderTexture(Renderer, PenTexture, Pencil);// �������� �����ϱ�
	RenderTexture(Renderer, NewTexture, New);// �������� �����ϱ�
	RenderTexture(Renderer, InpTexture, InputT);// �������� �����ϱ�
	if (on.eraser == true || clicks.eraser == true) { // eraser�� Ŭ�� �Ǿ� �ְų� eraser���������� ���콺�� ������
		SDL_RenderRemoveEdge(Renderer, Eraser);
		RenderTexture(Renderer, EraTexture, Eraser);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, Eraser, clicks.eraser);
	}
	if (on.pencil == true || clicks.pencil == true) { // pencil�� Ŭ�� �Ǿ� �ְų� pencil���������� ���콺�� ������
		SDL_RenderRemoveEdge(Renderer, Pencil);
		RenderTexture(Renderer, PenTexture, Pencil);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, Pencil, clicks.pencil);
	}
	if (on.new == true) {
		SDL_RenderRemoveEdge(Renderer, New);
		RenderTexture(Renderer, NewTexture, New);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, New, 0);
	}
	if (clicks.eraser == true || clicks.pencil == true)
		SDL_FontUpdate(Renderer, Font, *Track, *strong, r, g, b);

	if (wcscmp(inputText, L"") != 0) {
		RenderTexture(Renderer, InpTexture, InputT);// �������� �����ϱ�

		TTF_DrawText(Renderer, Fonts, inputText, 10, 672);
	}
	else {
		TTF_DrawText(Renderer, Fonts, " ", 10, 672);
	}
	SDL_RenderPresent(Renderer);// ������ ���
	SDL_RenderPresent(Renderer2);
	SDL_RenderPresent(Renderer3);
	SDL_RenderRemoveEdge(Renderer, Eraser);
	SDL_RenderRemoveEdge(Renderer, Pencil);
	SDL_RenderRemoveEdge(Renderer, New);
}
void TTF_DrawText(SDL_Renderer *Renderer, TTF_Font* Font, wchar_t* sentence, int x, int y) {
	SDL_Color Color = { 0,0,0 };
	SDL_Surface * Surface = TTF_RenderUNICODE_Blended(Font, sentence, Color);// ��Ʈ�� ����,���ڿ�, ������ ������ �����ڵ�� �����Ѵ��� ���ǽ��� �����Ѵ�
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);// ���ǽ��κ��� �ؽ��ĸ� �����Ѵ�
	SDL_FreeSurface(Surface);//���ǽ� �޸𸮸� ���� ���ش�.
	SDL_Rect Src;
	Src.x = 0;
	Src.y = 0;
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h);
	SDL_Rect Dst;
	Dst.x = x;
	Dst.y = y;
	Dst.w = Src.w;
	Dst.h = Src.h;
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst); //�״�� �������� �����Ѵ�
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
int SDL_MAINS(void) {// �� ������ SDL.h�� ����� �����Լ��� �츮�� ���� ���� ������ �ƴ�, ���� �Ű������� ���������

	FILE* out[] = { NULL,NULL,NULL,NULL };
	SDL_Window * Window = nullptr;//SDL ������ ����
	SDL_Renderer * Renderer = nullptr;// SDL ������ ���� 
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
	// �ؽ��Ŀ� �簢�� ����
	SDL_Texture * RgbTexture = nullptr;// ������ �̹����� ������� �ؽ��� ����
	SDL_Texture * PenTexture = nullptr;// �� �̹����� ������� �ؽ��� ����
	SDL_Texture * EraTexture = nullptr;// ���찳 �̹����� ������� �ؽ��� ����
	SDL_Texture * NewTexture = nullptr;// ���θ���� �̹����� ������� �ؽ��� ����
	SDL_Texture * TraTexture = nullptr;// ��ũ�� Ʈ�� �̹����� ������� �ؽ��ļ���
	SDL_Texture * BoxTexture = nullptr;// ��ũ�� �ڽ� �̹����� ������� �ؽ��� ����
	SDL_Texture * ChaTexture = nullptr;// ä��â �̹����� ������� �ؽ��� ����
	SDL_Texture * StaTexture = nullptr;// ����â �̹����� ������� �ؽ��� ����
	SDL_Texture * InpTexture = nullptr;// ä�� ����� �̹����� ������� �ؽ��� ����
	SDL_Texture * UseTexture = nullptr;// ���� �̹����� ������� �ؽ��� ����
	SDL_Texture * QusTexture = nullptr;// ���� �̹����� ������� �ؽ��� ����

	SDL_Rect RgbCode = { 0 };// RgbCode �̹����� ������ ������� �簢������ ����
	SDL_Rect Pencil = { 0 }; // Pencil �̹����� ������ ������� �簢�� ���� ����
	SDL_Rect Eraser = { 0 }; // Eraser �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect New = { 0 }; // New �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Track = { 0 };// Track �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Box = { 0 };//Box �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Chat = { 0 };// Chat �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Status = { 0 };//Status �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect InputT = { 0 };//InputT �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect UserT = { 0 };//UserT �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect QuesT = { 0 };//QuesT �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Timer = { 0, 0, 1310 / 4 + 10, 180 };
	SDL_Rect Timer2 = { 0, 60, 400, 150 };
	SDL_Rect Timer3 = { 150, 150, 100, 30 };
	// �ؽ��Ŀ� �簢�� ���� ��

	char str[256] = "";//UNICODE2UTF8�� ��ȯ���� ������ �迭����
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
	// �߰�
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {// SDL_Init�Լ��� SDL �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ���� SDL_Init�� �μ��� �پ���(ex : SDL_INIT_VIDEO)
		SDL_ErrorLog("SDL_Init");
		return 0;// ����
	};
	//�߰�
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

	// ������â 3���� ������ ���� x��ǥ�� 1920 - 1310/4-10�̰�, 1080-900/4-10�� y��ǥ�� �����̴�.
	Window = SDL_CreateWindow("HIT MIND WITH C", 1920 - 1310 / 4 - 10, 0, 1310 / 4 + 10, 1080, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ErrorLog("SDL_CreateWindow");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 3);
		return 0;//����
	}
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Renderer == nullptr) {
		SDL_ErrorLog("SDL_CreateRenderer");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont, out,4);
		return 0;
	}
	Window2 = SDL_CreateWindow("HIT MIND WITH C 2", 0, 0, (1920 - 1310 / 4 - 10), (1080 - 900 / 4 - 10), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window2 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ErrorLog("CreateWindow2");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 5);
		return 0;//����
	}
	Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer �Լ��� SDL Renderer ���� ��s�� ȣ��� �Ѱ��ִ� �μ��� SDL_Window *, ����̹� ����(-1�̸� �˾Ƽ� ������), �÷���(������ �ϵ����Ӱ� ��������ȭ ����� �����)
	if (Renderer2 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ErrorLog("CreateRenderer2");
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 6);
		return 0;// ����
	}
	Window3 = SDL_CreateWindow("HIT MIND WITH C 3", 0, 1080 - 900 / 4 - 10, 1920 - 1310 / 4 - 10, 900 / 4 + 10, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window3 == nullptr) {// ������ ���� ���н� if�� ����
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont, out,7);
		return 0;//����
	}
	Renderer3 = SDL_CreateRenderer(Window3, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer �Լ��� SDL Renderer ���� �Լ� ȣ��� �Ѱ��ִ� �μ��� SDL_Window *, ����̹� ����(-1�̸� �˾Ƽ� ������), �÷���(������ �ϵ����Ӱ� ��������ȭ ����� �����)
	if (Renderer3 == nullptr) {// ������ ���� ���н� if�� ����
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 8);
		return 0;// ����
	}
	// ������� ����
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// �׸��� ���� ����
	SDL_RenderClear(Renderer);// ������ ��� ���� (�׸��� ����� ȭ���� ä����)
	SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);// �׸��� ���� ����
	SDL_RenderClear(Renderer2);// ������ ��� ���� (�׸��� ����� ȭ���� ä����)
	SDL_SetRenderDrawColor(Renderer3, 255, 255, 255, 0);// �׸��� ���� ����
	SDL_RenderClear(Renderer3);// ������ ��� ���� (�׸��� ����� ȭ���� ä����)
							   // ��
	SDL_Event event;//SDL_Event ���� ����
//	const Uint8 * keystate;// Key ���� �迭�� �ޱ� ���� ������ ����
						   // RgbCode �̹���
	RgbTexture = LoadTextureEx(Renderer, ".\\image\\RgbCode.jpg", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (RgbTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 9);
		return 0;
	}
	SDL_QueryTexture(RgbTexture, NULL, NULL, &RgbCode.w, &RgbCode.h);// RgbCode �̹����� ���μ��� �о����. ������ â�� 3���� ������ ������ �ǹǷ� ������â �������� �о��
	RgbCode.w /= 4;
	RgbCode.w *= (1);
	RgbCode.h /= 4;
	RgbCode.h *= (1);
	RgbCode.x = 5 * (1);// �̹����� x,y��ǥ�� �ʺ�� ���� ����
	RgbCode.y = 1080 * (1) - RgbCode.h - 10 * (1);
	// ��
	// Track �̹���
	TraTexture = LoadTextureEx(Renderer, ".\\image\\Track.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (TraTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
		return 0;
	}
	SDL_QueryTexture(TraTexture, NULL, NULL, &Track.w, &Track.h);//�̹��� ���� �ҷ�����
	Track.w /= 4;
	Track.w *= (1);
	Track.h /= 8;
	Track.h *= (1);
	Track.x = RgbCode.x;
	Track.y = RgbCode.y - Track.h - 25 * (1);
	// ��
	// Box �̹���
	BoxTexture = LoadTextureEx(Renderer, ".\\image\\Box.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (BoxTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
		return 0;
	}
	SDL_QueryTexture(BoxTexture, NULL, NULL, &Box.w, &Box.h);//�̹��� ���� �ҷ�����
	Box.w /= 2;
	Box.w *= (1);
	Box.h /= 2;
	Box.h *= (1);
	Box.x = Track.x + 50 * (1);
	Box.y = Track.y + Track.h / 2 - Box.h / 2;
	// ��
	// Pencil �̹���
	PenTexture = LoadTexture(Renderer, ".\\image\\Pencil.jpg"); // �̹��� �ҷ�����
	if (PenTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont,out,9);
		return 0;
	}
	SDL_QueryTexture(PenTexture, NULL, NULL, &Pencil.w, &Pencil.h);
	Pencil.w /= 15;
	Pencil.h /= 15;
	Pencil.x = Track.x + 50 + 40;
	Pencil.y = Track.y - 30 - Pencil.h;
	// ��
	// Eraser �̹���
	EraTexture = LoadTexture(Renderer, ".\\image\\Eraser.jpg"); // �̹��� �ҷ�����
	if (EraTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font,topicFont,out, 9);
		return 0;
	}
	Eraser.w = Pencil.w;
	Eraser.h = Pencil.h;
	Eraser.x = Pencil.x + 50 * (1) + 30 * (1);;
	Eraser.y = Pencil.y;
	// ��
	// New �̹���
	NewTexture = LoadTexture(Renderer, ".\\image\\New.jpg"); // �̹��� �ҷ�����
	if (NewTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	New.w = Eraser.w;
	New.h = Eraser.h;
	New.x = Eraser.x + 50 * (1) + 30 * (1);
	New.y = Eraser.y;
	ChaTexture = LoadTexture(Renderer, ".\\image\\CHAT_BODY.png");												// ä�� �̹���
	if (ChaTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	Chat.w = (1310 / 4) + 20;
	Chat.h = Eraser.y - 262;
	Chat.x = 0;
	Chat.y = 200;
	InpTexture = LoadTexture(Renderer, ".\\image\\Track.png");												// ä�� �̹���
	if (InpTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	InputT.w = (1310 / 4) + 20;
	InputT.h = 41;
	InputT.x = 0;
	InputT.y = Eraser.y - 71;
	RenderTexture(Renderer, RgbTexture, &RgbCode);
	UseTexture = LoadTexture(Renderer3, ".\\image\\user.png");
	if (UseTexture == nullptr) {// �����ڵ� ���
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
	QusTexture = LoadTexture(Renderer, ".\\image\\question_fix.png");												// ä�� �̹���
	if (QusTexture == nullptr) {// �����ڵ� ���
		Quit(Renderer, Renderer2, Renderer3, Window, Window2, Window3, Font, topicFont, out, 9);
		return 0;
	}
	QuesT.w = 400;
	QuesT.h = 100;
	QuesT.x = -40;
	QuesT.y = 60;

	// ��
	int drawcount = 0;
	bool quit = false;//�� ���� ����
	bool drag = false;// �巡�������� Ȯ���ϴ� ���� ����
	bool happen = true;
	bool hangeulinput = false;
	bool enter = false;
	bool writemode = false;
	int alpha;// ���� ä���� ������� ���� ����
	int x, y; // �����̰� �������� ���콺�� ��ǥ�� ������� ���� ����
	float r = 0, g = 0, b = 0; //rgb���� ���� ���� ���� ������ ������ �ϹǷ� �Ǽ������� ����
	float i = 0, j = 0, k = 0, l = 0, length = 0;// for������ �� ��������
	int pastturn = turn;
	int newclick = 0;
	float xpos = 0, ypos = 0;// ���콺 x��ǥ y��ǥ�� �����ϴ� �������� 
	float strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// ������ ����
	SDL_Rect Rect = { 0 }; // �׸� �簢���� ������ �ݺ��� �ۿ��� ����
	SDL_Rect Fonts = { Track.x - strong / 2 + 35 ,Track.y - strong / 2 - 50,strong,strong };// ����, ������� �����ֱ� ���� �簢�� ���� ����
	SDL_Rect Edge = { 0 };// �׵θ��� �׸��� ���� �簢�� ���� ���� 
	SDL_Rect Happen = { 0,0,1310 / 4 + 10,New.y - 10 };// Happen �� Ʈ���϶� ����� ����
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
	int buff = 0;		  // ��
						  // while������ �� ������ �ʱⰪ ����
	// �������� �����ϱ�
												  // ���� �ʱⰪ ������
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
	while (!quit) {// quit�� true�� �ƴҶ� ���� ���ѹݺ�
		if ((clock() - firstclock) / 1000 > first)
		{
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������ ������� �����ؾ��� �׷��� ���찳 ������ �ϹǷ�
			SDL_RenderFillRect(Renderer, &Timer3);// ���찳���� ������� ĥ��
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
			sprintf(query, "%d�� ����", roommode.time - first);
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
				sprintf(query, "%s ���� ���߾����ϴ�! ������ %s �Դϴ�", friendname[turn - 1], pasttopic);
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
			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������ ������� �����ؾ��� �׷��� ���찳 ������ �ϹǷ�
			SDL_RenderFillRect(Renderer, &Timer);// ���찳���� ������� ĥ��
			if (myownnumber == turn) {
				RenderTexture(Renderer, QusTexture, &QuesT);// �������� �����ϱ�
				han2unicode(topic, unicode);
				TTF_DrawText(Renderer, topicFont, unicode, 100, 90);
			}
			else
			{
				SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������ ������� �����ؾ��� �׷��� ���찳 ������ �ϹǷ�
				SDL_RenderFillRect(Renderer, &Timer2);// ���찳���� ������� ĥ��
			}
			//			han2unicode(query, unicode);
			//			TTF_DrawText(Renderer3, topicFont, unicode, ((1920 - (1310 / 4 - 10)) / 4) * (i * 0.98) + 290, 148);	
			//		SDL_RenderPresent(Renderer);
			//		SDL_Delay(3000);
			sprintf(query, "%s �����Դϴ�", friendname[turn - 1]);
			han2unicode(query, unicode);
			TTF_DrawText(Renderer, topicFont, unicode, 0, 0);
			sprintf(query, "���� %d/%d", ee, roommode.question);
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
		if (SDL_PollEvent(&event)) {//�̺�Ʈ�� ������ if�� ����
			switch (event.type) {//�̺�Ʈ Ÿ�Կ� ���� ���̽��� ����
			case SDL_TEXTINPUT:
				if (hangeul == true && (event.text.text[0] == -29 || event.text.text[0] + 256 >= 234 && event.text.text[0] + 256 <= 237))// �ѿ�Ű�� �ѱ۷� �Ǿ��ְ� �ѱ��̶�� event.text.text[0]�� ������ �ѱ��Ǵܰ�����
				{
					wstr[2] = L"";
					int sum = (event.text.text[0] + 22) * 64 * 64 + (event.text.text[1] + 128) * 64 + event.text.text[2] + 41088;
					wstr[0] = sum;
					wcscat(inputText, wstr);
				}
				else if (!((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {// �ѱ۾ƴϰ� c�� v�� �������� ��Ʈ�Ѹ�尡 �ƴ϶�� �ѱ��� ������ � ���ڸ� �Է��ߴٴ� ����
					wstr[2] = L"";
					swprintf(wstr, sizeof(wstr) / sizeof(wchar_t), L"%hs", event.text.text);// event.text.text ���ڿ� �׳� ������ѹ���
					wcscat(inputText, wstr);// ���ڿ� ����
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
							sprintf(query, "insert into catchmind.chating (name, mean, room) values ('[���]', 'ä���� �ʱ�ȭ �մϴ�.', '%s')",connectroom[CHOOSEROOM].ip);
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
							strcpy(chatquery[10], "[1] /help : ������ ǥ���մϴ�.");

							strcpy(chatquery[11], "[2] /clear : ä��â�� �ʱ�ȭ�մϴ�.");

							strcpy(chatquery[12], "[3] /capture [���ϸ�] : ���� ȭ���� ĸ���մϴ�.");

							strcpy(chatquery[13], "[4] /stopmusic : ���� ������ ����ϴ�.");

							strcpy(chatquery[14], "[5] /startmusic : ���� ������ ������մϴ�.");
							CHATHAPPEN = true;
						}
						else if (strcmp(euckr, "/?") == 0)
						{
							RESET(chatquery);
							strcpy(chatquery[10], "[1] /help : ������ ǥ���մϴ�.");

							strcpy(chatquery[11], "[2] /clear : ä��â�� �ʱ�ȭ�մϴ�.");

							strcpy(chatquery[12], "[3] /capture [���ϸ�] : ���� ȭ���� ĸ���մϴ�.");

							strcpy(chatquery[13], "[4] /stopmusic : ���� ������ ����ϴ�.");

							strcpy(chatquery[14], "[5] /startmusic : ���� ������ ������մϴ�.");
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
							sprintf(chatquery[14], "���� �ȿ� %s.png�� ������ �Ǿ����ϴ�", query);
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
				else if (event.key.keysym.sym == SDLK_BACKSPACE && wcslen(inputText) > 0)// Ű���� �齺���̽��� �迭�� ���̰� 1�̻��϶�
				{
					inputText[wcslen(inputText) - 1] = '\0';// ���������ڸ� �ι��ڷ� �ٲ�
					happen = true;
				}
				else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {// ��Ʈ�� ����̰� c�� �����ٸ�
					strcpy(str, UNICODE2UTF8(inputText, wcslen(inputText)));
					SDL_SetClipboardText(str);// Ŭ�����忡 ����
				}
				else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)// ��Ʈ�� ����̰� v�� �����ٸ�
					wcscpy(inputText, UTF82UNICODE(SDL_GetClipboardText(), strlen(SDL_GetClipboardText())));// Ŭ�����忡�� ������
				else
					hangeulinput = true;
				happen = true;
				break;
			case SDL_WINDOWEVENT://SDL���� Ÿ���� ���

				switch (event.window.event) {

				case SDL_WINDOWEVENT_CLOSE:// �ټ� â������ �ݱ��̺�Ʈ�� �߻��Ұ��
					send(connect_sock, "exit", 35, 0);
					quit = true;// quit�� true�� ����
					Sleep(100);
					break;// �극��ũ
				case SDL_WINDOWEVENT_ENTER:// ������
					SDL_RaiseWindow(SDL_GetWindowFromID(event.window.windowID));//��Ŀ�� �̵���Ŵ
					break;
				case SDL_WINDOWEVENT_LEAVE:
					drag = false;//���콺�� â���� �������Ƿ� �巡�� ����� ������Ŵ
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					break;
				}
			case SDL_MOUSEMOTION: // ���콺�� ������ Ÿ���� ���

				if (event.motion.state == 1 && drag == true) {// ���콺�� ���������� ���콺 ���� ��ư�� �������ִٸ� ��, �巡�� �ߴٸ�
					if (event.motion.windowID == SDL_GetWindowID(Window)) {// ���콺�� ������ ���� ù��° ������ â�ϰ��
						if ((event.motion.x + Box.w / 2 >= Track.x&&event.motion.x + Box.w / 2 <= Track.x + Track.w) && (event.motion.y >= Box.y&&event.motion.y <= Box.y + Box.h)) {// �巡���� ���� �߽� x��ǥ�� Ʈ���ȿ� �հ� �巡���� ���� �߽� y��ǥ�� �ڽ��� y��ǥ ���� �ȿ� ������ if�� ����
							SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������ ������� �����ؾ��� �׷��� ���찳 ������ �ϹǷ�
							SDL_RenderFillRect(Renderer, &Box);// ���찳���� ������� ĥ��
							Box.x = event.motion.x;// �ڽ��� x��ǥ�� Ŭ���Ѱ��� x��ǥ�� �ٲ� == �̵���Ŵ
							strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// ���⸦ Ʈ���� ��ũ�� �ڽ��� ��ġ�� ����ؼ� ������			
							happen = true;
							break;// �� ������ Ż����
						}
					}
					else if (event.motion.windowID == SDL_GetWindowID(Window2)) {// ���콺�� ������ ���� �ι�° ������ â�� ���
						length = sqrt(pow(Rect.x + strong / 2 - event.motion.x, 2) + pow(Rect.y + strong / 2 - event.motion.y, 2));// ���������� ���̸� ��Ÿ����� ������ ����. �̶� ������ ���� ���� ���� �巡���� ���� ���� ����
						if (length == 0) break;
						if (clicks.pencil == true) {// �潽�� ���
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.motion.x, event.motion.y, strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							i = (event.motion.x - (Rect.x + Rect.w / 2)) / length;// i�� ������ x��ǥ�� ���̸� ���̷� ���� ����.
							j = (event.motion.y - (Rect.y + Rect.h / 2)) / length;// j�� ������ y��ǥ�� ���̸� ���̷� ���� ����.
							k = 0;// while���ȿ� �� ���� �ʱ�ȭ.
							xpos = Rect.x + Rect.w / 2 - strong / 2;// ���������� x��ǥ�� ���� ����
							ypos = Rect.y + Rect.h / 2 - strong / 2;// ���������� y��ǥ�� ���� ����

							Rect.w = Rect.h = strong;// ���⼳��
							SDL_SetRenderDrawColor(Renderer, r, g, b, 0);
							for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
								Rect.x = xpos + k*i;// ���� ���� ������ �������� x��ǥ�� ���� 
								Rect.y = ypos + k*j;// ���� ���� ������ �������� y��ǥ�� ����
								SDL_RenderFillRect(Renderer2, &Rect);//�簢�� �������� ����
							}
							// ����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						}
						else if (clicks.eraser == true) {// ���찳 ���
							strong *= 80 / (float)50.0;
							if (connect_sock != 0) {
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.motion.x, event.motion.y, strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);// ���찳�ϱ� ������ �Ͼ������									
							i = (event.motion.x - Rect.x) / length;// i�� ������ x��ǥ�� ���̸� ���̷� ���� ����.
							j = (event.motion.y - Rect.y) / length;// j�� ������ y��ǥ�� ���̸� ���̷� ���� ����.
							k = 0;// while���ȿ� �� ���� �ʱ�ȭ.
							xpos = Rect.x;// ���������� x��ǥ�� ���� ����
							ypos = Rect.y;// ���������� y��ǥ�� ���� ����
							Rect.w = Rect.h = strong;// ���⼳��
							for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
								Rect.x = xpos + k*i;// ���� ���� �߽� �� x��ǥ�� ���� 
								Rect.y = ypos + k*j;// ���� ���� �߽� �� y��ǥ�� ����
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
								//����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (writemode == true) {
					if (event.button.button == 1) {
						if (event.button.windowID == SDL_GetWindowID(Window)) {
							if ((event.button.x >= RgbCode.x&&event.button.x <= RgbCode.x + RgbCode.w) && (event.button.y >= RgbCode.y&&event.button.y <= RgbCode.y + RgbCode.h)) {// RgbCode �̹��� ���̸� if�� ����
								alpha = (event.button.y - RgbCode.y) / (RgbCode.h / 9);// RgbCode �ȿ����� y�� ��� == ��ä�����
								switch ((event.button.x - RgbCode.x) / (RgbCode.w / 13)) {// RgbCode�ȿ����� x�� ���
								case 0:// �� ���� �ڵ�
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
								case 12:// case 12�� ȸ���迭�̶� Ư���� �˰����� �׷��� ���� �ڵ带 ����
									r = 128 + (255 / 8.0)*(alpha - 4); g = 128 + (255 / 8.0) * (alpha - 4); b = 128 + (255 / 8.0) * (alpha - 4);
									alpha = 4;
									break;
								}
								// �������� rgb�� ����
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
							else if ((event.button.x >= Track.x&&event.button.x <= Track.x + Track.w) && (event.button.y >= Box.y&&event.button.y <= Box.y + Box.h)) {//��ũ�� Ʈ���� Ŭ�� ���� ���
								SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������� ����(���찳 ����)
								SDL_RenderFillRect(Renderer, &Box);// �������� �簢���� �׷���. �ٵ� ����̹Ƿ� �����ִ� ������ ��
								Box.x = event.button.x;//��ũ�� �ڽ��� �̵���Ŵ
								drag = true; //�巡�׷� ������ �����ϰ� ����
								strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// ���⸦ Ʈ���� ��ũ�� �ڽ��� ��ġ�� ����ؼ� ������
								happen = true;
								break;
							}
							else if ((event.button.x >= Eraser.x - 10 && event.button.x <= Eraser.x + Eraser.w + 10) && (event.button.y - 10 >= Eraser.y&&event.button.y <= Eraser.y + Eraser.h + 10)) {
								SDL_RenderFillRect(Renderer, &Fonts);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
								clicks.eraser = true;

								clicks.pencil = false;
								happen = true;
							}
							else if ((event.button.x >= New.x - 10 && event.button.x <= New.x + New.w + 10) && (event.button.y >= New.y - 10 && event.button.y <= New.y + New.h + 10)) {		//New �̹����� Ŭ��������

							//	sprintf(query, "screenshot\\%d.png", time(NULL));
						//		makebmp(query, Renderer2);


								SDL_DestroyRenderer(Renderer2);
								Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
								SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
								SDL_RenderClear(Renderer2);
								happen = true;
								sndPlaySoundA("music\\erase.wav", SND_ASYNC);
								//����~~~~~~~~~~~~~~~~~~
								if (connect_sock != 0) {
									send(connect_sock, "SDLCLEAR", 45, 0);
								}
								//	SDL_RenderFillRect(Renderer, &Fonts);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
								clicks.eraser = false;
								clicks.pencil = true;

								happen = true;
							}

							else if ((event.button.x >= Pencil.x - 10 && event.button.x <= Pencil.x + Pencil.w + 10) && (event.button.y >= Pencil.y - 10 && event.button.y <= Pencil.y + Pencil.h + 10)) {
								Fonts.w += 2;// �Ϻ��� ���� �ƴ϶� �ɲ� ��������
								Fonts.h += 2;
								SDL_RenderFillRect(Renderer, &Fonts);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
								clicks.eraser = false;
								clicks.pencil = true;
								sndPlaySoundA("music\\pencil.wav", SND_ASYNC);
								happen = true;
							}
						}
						else if (event.button.windowID == SDL_GetWindowID(Window2)) {
							if (clicks.pencil == true) {

								Rect.x = event.button.x - strong / 2;
								Rect.y = event.button.y - strong / 2;// ���⸸ŭ�� �簢���� ����
								Rect.w = Rect.h = strong;// ���� ����
								SDL_RenderFillRect(Renderer2, &Rect);// �������� �׸�
																	 // ����~~~~~~~~~
								if (connect_sock != 0) {
									sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil, drag, event.button.x, event.button.y, strong, r, g, b);
									send(connect_sock, query, 45, 0);
								}
								drag = true; //�巡�׷� �׸��� �ְ� ����
								happen = true;


								break;
							}
							else if (clicks.eraser == true) {
								strong *= 80 / (float)50.0;
								SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
								int x1, y1, x2, y2;
								Rect.x = event.button.x;// ���̶� �������� ��ǥ���ƴ� �߽���ǥ�� ��������
								Rect.y = event.button.y;
								for (l = 0; l < 180; l++) {
									x1 = sin(3.14 / 180 * l)*strong / 2;
									y1 = cos(3.14 / 180 * l)*strong / 2;
									x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
									y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
									SDL_RenderDrawLine(Renderer2, x1 + Rect.x, y1 + Rect.y, x2 + Rect.x, y2 + Rect.y);
								}
								// ����~~~~~~~~~~~~~~
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
			case SDL_MOUSEBUTTONUP: // ���콺 ��ư�� ��������
				if (event.button.button == SDL_BUTTON_LEFT) // ���� ��ư�� ���ʹ�ư�̶��
					drag = false;// �巡�׷� �ϴ� ��� ���� �Ұ����ϰ� ����
			}
		}

		SDL_GetMouseState(&x, &y);
		if ((x >= Eraser.x - 10 && x <= Eraser.x + Eraser.w + 10) && (y >= Eraser.y - 10 && y <= Eraser.y + Eraser.h + 10)) {// eraser�ȿ� ���콺�� ������
			if (on.eraser == false && clicks.eraser == false) // ���������� ���콺�� �÷��������ʰ� ���찳�� Ȱ��ȭ�����ʾ�����
				happen = true;// happen�� �߻�
			if (clicks.eraser == false)//���찳�� Ŭ���� ���°� �ƴϾ��ٸ�
				on.eraser = true;// ���콺�� �÷��������� ������
		}
		else if (on.eraser == true) {//���������� ���콺�� �÷����־��� ������ eraser�ȿ� ���콺�� ������
			happen = true;//happen�� �߻�
			on.eraser = false;// ���콺�� �� �÷��������� ������
		}
		if ((x >= Pencil.x - 10 && x <= Pencil.x + Pencil.w + 10) && (y >= Pencil.y - 10 && y <= Pencil.y + Pencil.h + 10)) {// eraser �ȿ� ���콺�� ������
			if (on.pencil == false && clicks.pencil == false)// ���������� ���콺�� �÷��������ʰ� �潽�� Ȱ��ȭ���� �ʾ�����
				happen = true;// happen�� �߻�
			if (clicks.pencil == false)//���찳�� Ŭ���� ���°� �ƴϾ��ٸ�
				on.pencil = true;// ���콺�� �÷��� ������ ������
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
			RenderTexture(Renderer, ChaTexture, &Chat);// �������� �����ϱ�
			for (l = 0; l < 15; l++) {
				if (chatquery[(int)l][0] != 0) {
					han2unicode(chatquery[(int)l], unicode);
					TTF_DrawText(Renderer, Font, unicode, 10, 250 + 25 * l);		//�ֱ� 15���� ä���� �ҷ���
					ZeroMemory(unicode, sizeof(unicode));// �߰�
				}
			}
			CHATHAPPEN = false;
			happen = true;
		}

		//	han2unicode(query, unicode);
		//	TTF_DrawText(Renderer, Font, unicode, 0, 50);

		if (happen == true) {
			//		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);// ������ ������� �����ؾ��� �׷��� ���찳 ������ �ϹǷ�
			//		SDL_RenderFillRect(Renderer, &Timer);// ���찳���� ������� ĥ��

			SDL_RenderUpdate(Renderer, Renderer2, Renderer3, TraTexture, BoxTexture, EraTexture, PenTexture, NewTexture, ChaTexture, InpTexture, &Track, &Box, &Eraser, &Pencil, &New, &Fonts, &Chat, &InputT, Font, inputText, &strong, r, g, b);
			happen = false;

		}

	}

	SDL_DestroyTexture(InpTexture);
	SDL_DestroyTexture(UseTexture);
	SDL_DestroyTexture(RgbTexture);// �ؽ��� �ı��ϱ�
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
	printf("���ڴ� %s�Դϴ�. ���᰹�� %d��\n", friendname[good], score[good][1]);
	for (int k = 0; k < 4; k++)
	{
		printf("%s : %d��\n", friendname[k], score[k][1]);
	}
	printf("\n����..");
	getchar();
	return 0;// ����
}
HWND GetConsoleHwnd(void)
{

	HWND hwndFound;         // ������ �ڵ� ����
	WCHAR pszNewWindowTitle[MY_BUFSIZE]; // �� ������ Ÿ��Ʋ
	WCHAR pszOldWindowTitle[MY_BUFSIZE]; // ���� ������ Ÿ��Ʋ
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);		//���� ������ Ÿ��Ʋ�� �����Ŵ
	wsprintf(pszNewWindowTitle, "%d/%d", GetTickCount(), GetCurrentProcessId());	//�� ������ Ÿ��Ʋ�� Ư���ϰ� �����Ŵ
	SetConsoleTitle(pszNewWindowTitle);				//�� ������ Ÿ��Ʋ�� �����Ŵ
	Sleep(10);			//��� ���
	hwndFound = FindWindow(NULL, pszNewWindowTitle);		//�� ������ Ÿ��Ʋ�� �����Ų �����츦 ã�� �� �ڵ鰪�� ������
	SetConsoleTitle(pszOldWindowTitle);					//������Ÿ��Ʋ�� ���� Ÿ��Ʋ�� ������
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
char* getDesktopFolderName()     //c:\Users\UserName\Desktop\ ��ȯ

{
	ULONG ulDataType;
	HKEY hKey;
	DWORD dwToRead = 100;
	static char strPath[100];
	char strKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	//cannot use in unicode, use multi-byte code  http://blog.naver.com/stormor/70171786787
	RegOpenKeyEx(HKEY_CURRENT_USER,
		strKey,   //���� unicode �ȵ�		

		0, KEY_READ, &hKey);

	RegQueryValueEx(hKey, "Desktop", NULL,

		&ulDataType, (LPBYTE)strPath, &dwToRead);

	strPath[dwToRead] = '\0';

	RegCloseKey(hKey);



	return strPath;




}
void strintrude(char *s, char *t, int i)



{
	/*���ڿ� s�� i ��° ��ġ�� ���ڿ� t�� ����*/



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