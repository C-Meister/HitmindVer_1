
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
#include <stdint.h>
//#include <WinSock2.h>		//�������α׷���

//Ư�� ������� (���� ��ġ) 
#include "SDL/SDL.h"			//SDL - �׷��� �������
#include "SDL/SDL_image.h"
#include "SDL/sdl_ttf.h"
//#include "SDL/render/SDL_sysrender.h"
#include "mysql/mysql.h"
#define nullptr 0

// ���̺귯�� ���� ���̺귯�������� ���� �߰����ص� �˴ϴ�.
// #pragma comment �� visual studio������ ��� ���� *�ַ�� �÷����� 64��Ʈ�� ���ּ���
#pragma comment (lib, "libmysql.lib")	//mysql���̺귯��
#pragma comment (lib, "SDL2")			//�׷��� ���̺귯�� 1
#pragma comment (lib, "SDL2main")		//�׷��� ���̺귯�� 2
#pragma comment (lib, "SDL2_image")		//�׷��� ���̺귯�� 3

#pragma comment (lib, "ws2_32.lib")		//����(��Ʈ��ũ)���̺귯��


#pragma warning (disable : 4700)
#pragma warning (disable : 4244)
#pragma warning (disable : 4101)		//������� ���� ���������Դϴ�. ��� ����
//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
//#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�
//#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�(����)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//������ X���� Y ���
#define CHOP(x) x[strlen(x) - 1] = ' '	//fgets�� ���� ���͵� �迭������. ���͸� �迭���� �������ִ°�
//������� ������� 
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
//���� ������ (��� ����õ)

CRITICAL_SECTION cs;	//�̺�Ʈ
//char message[100];		//���� ���α׷��� ���ڿ�
char status[4];			//���Ͽ�
char username[30];		//����� �̸�
char friendname[4][30] = { "Player 1", "Player 2", "Player 3", "Player 4" };
WSADATA wsaData;						//���� WSAStartup()�Լ��� ���̴� ����
SOCKET connect_sock, Sconnect_sock[4], listen_sock;	//���� ���Ϻ���
SOCKADDR_IN connect_addr, listen_addr;			//���� �ּ����� �����ϴ� ����
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



//�⺻ �Լ���
void gotoxy(short x, short y);
void cur(short x, short y);
void exitsignal(void);
void signalall(void);
void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
void usermain(void);
//--------------------- ��Ʈ��ũ �Լ��� -----------------------------------
void ErrorHandling(char *Message);				//���� ���� ��� �ϴ� �Լ�
int Connect_Server(char *ServerIP);			//���� ���� ���ִ� �Լ�
void recieve(void);								//�������� ������ �޾ƿ��� ������� �Լ�
void sendall(char *message);					//�ϳ��������� ��������
int waitroom(void);							//��Ʈ��ũ ����
void Clnt_1(int v);								//���� - Ŭ���̾�Ʈ 1���
void makeroom(int *count);							//�游���(��Ʈ��ũ)
IN_ADDR GetDefaultMyIP(void);					//�� ip ���
bool exitallthread(void);

//--------------------- MySQL �Լ��� --------------------------------------
int sqllogin(MYSQL *cons);						//mysql�� ����� �����͸� ���� �α����� �ϴ� �Լ�
int sqlsignup(MYSQL *cons);						//mysql�� ���������͸� �߰��ϴ� �Լ�
void loadmysql(MYSQL *cons, char mysqlip[]);	//mysql�� �����ϴ� �Լ�
char **onemysqlquery(MYSQL *cons, char *query); //mysql��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
void writechating(MYSQL *cons);					//mysql�� ä���� �Է��ϴ� �Լ�
void readchating(MYSQL *cons);					//mysql�� ä���� �д� �Լ�
void inserttopic(MYSQL *cons);
void sqlmakeroom(MYSQL *cons);					//���� ����� �Լ�

// -------------------- SDL �׷��� �Լ��� ---------------------------------
void SDL_ErrorLog(const char * msg);//�����ڵ� ��� �Լ�			//�׷��ȿ����ڵ� ��� �Լ�
void IMG_ErrorLog(const char * msg);			//�̹��������ڵ� ��� �Լ�
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // �׷��ȿ��� ó�� �Լ�
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // �̹������� ó�� �Լ�
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // �ؽ��Ŀ� �̹������� �پ��ϰ� �ε��ϴ� �Լ� ����
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, SDL_Rect * Rect);	//�ؽ��ĸ� ����ϴ� �Լ� ����
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Rect Track, SDL_Rect Box, SDL_Rect Eraser, SDL_Rect Pencil, SDL_Rect New, SDL_Rect Font, float strong, int r, int g, int b);	//�������� �ø��� �Լ�
void SDL_FontUpdate(SDL_Renderer * Renderer, SDL_Rect Font, SDL_Rect Track, float strong, int r, int g, int b);
void SDL_RenderRemoveEdge(SDL_Renderer* Renderer, SDL_Rect * Rect);
void SDL_RenderDrawEdge(SDL_Renderer* Renderer, SDL_Rect * Rect, bool clicks);
int SDL_MAINS(void);
// -------------------- ���� ���� �Լ��� ----------------------------------
void mainatitleimage(void);						//���� ����Ÿ��Ʋ ���
int maintitle(void);							//���� ����Ÿ��Ʋ ��¹� ����
void banglist(MYSQL *cons, int j);						//���� �� ���
int bangchose(MYSQL *cons);						//���� �� ��¹� ����
int chooseroom(int roomnum);
void waitroomtema();
void logintema(void);							//�α��� ������
void jointema(void);							//ȸ������ ������
LOG login(int m);								//�⺻���� �α��� �Է�
void zeroprint(int xx, int yy, int lr, int m);  //������

//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);	//�ܾ Ȯ����
void click(int *xx, int *yy, int *lr);					//Ŭ���Լ� �ι�°, xx���� yy���� ��ȯ��

//--------------------------�̴ϰ��� ���ھ߱� �Լ���----------------------


int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� ������� 
{

	//SDL_MAIN();
	//���� ����
	int i, j, k, v, result;

	signalall();
	char mainchoose = 0;
	char bangchoose;
	char chooseroomcount;
	POINT pos;								//x, y��ǥ ǥ�� )pos.x, pos.y
	MYSQL *cons = mysql_init(NULL);			//mysql �ʱ�ȭ
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	char query[400];						//mysql ��ɾ ������
	char mysqlip[30] = "10.80.161.182";		//mysql ip ����ip�Դϴ�	���� ������7����
//	char *ServerIP = "10.80.162.41";		//���� ip ��ȣip��
	char data[1000][30] = { 0, };           //�ܾ����
	char nowword[30] = { 0, };              //�������� �ܾ�
	char scanword[30] = { 0, };             //���� ģ �ܾ�
	int bangnum = 0;						//�� ���� ��ȣ
	char serverreturn = 0;
	mysql_query(cons, "use catchmind");
	memset(&wsaData, 0, sizeof(wsaData));
	memset(&connect_sock, 0, sizeof(connect_sock));
	memset(&connect_addr, 0, sizeof(connect_addr));
	//SDL_MAINS();
	// �ʱ�ȭ ��

	loadmysql(cons, mysqlip);				//mysql ���� �ҷ�����
	con = cons;

	disablecursor(1);
	while (1) {								//�α��� �ݺ���
		mainchoose = maintitle();				//main ȭ��
		CLS;
		if (mainchoose == 1) {				//main���� ù��°�� ����
			ConsoleL(30, 30);
			if (sqllogin(cons) != 1)		//�α��ο� �������� ���ϸ� ó������
				continue;

			ConsoleL(50, 20);
			while (1) {						//�� �ݺ���
				gotoxy(0, 0);
				bangchoose = bangchose(cons);	//���� ��	
				if (bangchoose == 0) {			//�游��⸦ Ŭ���ϸ� �游���� �̵�
					sqlmakeroom(cons);
					break;
				}
				else if (bangchoose == 1)		//�� ���� ���� -�����߰�
				{

				}
				else                            //�� ���� ����
				{
					chooseroomcount = chooseroom(bangchoose);
					if (chooseroomcount == -1)		//return -1�� �ش� ���̾�����
					{
						continue;
					}
					if (chooseroomcount == 0)		//return 0�� ��й�ȣ�� Ʋ����
					{
						cur(14, 1);
						printf("(��й�ȣ�� Ʋ�Ƚ��ϴ�)");
						_getch();
						CLS;
						fflush(stdin);
						continue;
					}
					if (chooseroomcount == 1)		//return 1 �� ��й�ȣ���� ������
					{

						serverreturn = Connect_Server(connectroom[bangchoose - 2].ip);		//���� ���� ����
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


//�Լ� �����		���� �ּ�ȭ Ctrl + M + O  ���� ���̱� Ctrl + M + L

void inserttopic(MYSQL *cons)
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
void sqlmakeroom(MYSQL *cons) {
	while (1) {
		CLS;
		int count = 0;
		int i = 0;
		IN_ADDR addr;

		addr = GetDefaultMyIP();	//����Ʈ IPv4 �ּ� ������
		char * myip = inet_ntoa(addr);
		ROOM myroom = { 0, 0, 0 };
		disablecursor(0);
		WHITE
			printf("��������������������������\n");
		printf("��                                              ��\n");
		printf("��            ĳġ���ε� �� �����              ��\n");
		printf("��          �� ip :  %s \n", myip);
		printf("��������������������������\n");
		printf("��    ����    ��                              ���\n");
		printf("��������������������������\n");
		printf("��  Password  ��                              ���\n");
		printf("��������������������������\n");
		gotoxy(48, 3);
		printf("��");
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
			printf("(�˼� ���� ����. �ߺ��� �ȵǰ� ����� �ּ���)");
		}
		disablecursor(1);
		CLS;
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
	}
	disablecursor(0);
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



		if (status[0] == 10)
		{
			status[0] = 2;
			CLS;
			printf("������ �����մϴ�.");
			if (lead == true)
			{
				signalmode = 3;
				sprintf(query, "delete from catchmind.room where ip = '%s'", inet_ntoa(GetDefaultMyIP()));
				mysql_query(con, query);
				
			}
			for (int i = 0; i < 3; i++)
			{
				cur(0, 1);
				printf("%d���� ����", 3 - i);
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
LOG login(int m) { // 1�̸� �α��� 2�̸� ȸ������ �ʼ�!!
				   //���� ���� �ڵ�ϱ� ȸ�������̶� �α��ο� �� �����ñ�
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
	/*�г��� ����*/
	disablecursor(0);
	if (m == 2) {
		ConsoleL(30, 30);
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

	/*��й�ȣ ��ȣȭ ����*/
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
		sprintf(query, "insert into catchmind.chating (name, mean) values ('������', '%S')", buffer);
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
void loadmysql(MYSQL *cons, char mysqlip[])	//MYSQL ���� �ҷ�����
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
		printf("���ο� ip�� ������ �ּ���.\n->");
		scanf("%s", mysqlip);
		loadmysql(cons, mysqlip);					//����Լ� ȣ��
	}
	else {
		printf("\b2");
		printf("\n����");
		mysql_set_character_set(cons, "euckr");
		CLS;
	}

	return;
}
char **onemysqlquery(MYSQL *cons, char *query) {		//mysql ��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
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
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock������ ���� �Ҵ�
	connect_addr.sin_family = AF_INET;				//������ ������ �ּ� ����
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //���� IP
	connect_addr.sin_port = htons(5555);					 //���� ��Ʈ
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //������ ����
		ErrorHandling("connect() error");
	threads[0] = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, NULL, 0, NULL); //�������� �����͸� �޾ƿ��� ������ ����
	CLS;
	sprintf(query, "player   connect %s", username);
	send(connect_sock, query, 45, 0);
	Sleep(200);

	return waitroom();
}
void recieve(void) { //�������� ������ �޾ƿ��� ������� �Լ�
	char message[50] = { 0, };
	while (1) {

		if (recv(connect_sock, message, 45, 0) > 0) { //�������� �����͸� �޾ƿ� message������ ����
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
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	LOG user;								//����� ���� ����ü
	while (1) {

		int check = 0;

		user = login(1);							//login �Լ��� ���
		if (user.id[0] == 0)
		{
			fflush(stdin);
			check = sqlsignup(cons);
			gotoxy(2, 3);
			if (check == 1)
				printf("               (ȸ������ ����)           ");
			else if (check == -1)
				printf("             (���̵� �ߺ��˴ϴ�)              ");
			else
				printf("               (ȸ������ ����)          ");
		}
		else {
			sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
			mysql_query(cons, query);
			sql_result = mysql_store_result(cons);
			if (mysql_fetch_row(sql_result) == NULL)									//�؈� id�� ������ 
			{
				gotoxy(2, 3);
				printf("           ���̵� �������� �ʽ��ϴ�     ");

			}
			else {

				sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass);
				mysql_query(cons, query);	//password�� DB�� ��ȣȭ�Ǿ� �־ ���� ���Ҷ��� ���� ��ȣȭ�ؼ� �񱳸���
				sql_result = mysql_store_result(cons);



				if ((sql_row = mysql_fetch_row(sql_result)) == NULL)
				{
					gotoxy(2, 3);
					printf("              ��й�ȣ�� Ʋ�Ƚ��ϴ�        ");
				}
				else {
					strcpy(username, sql_row[1]);
					sprintf(query, "title %s�� ��Ʈ���ε忡 ���Ű��� ȯ���մϴ�!", username);
					system(query);
					return 1; //�α��� ����
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
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����


	if (user.name[0] == 0)
		return 0;

	sprintf(query2, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
	mysql_query(cons, query2);
	sql_result = mysql_store_result(cons);
	if ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		if (sql_row[0][1] != 0)
			return -1;
	}

	sprintf(query3, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);

	if (!(mysql_query(cons, query3)))											//		 password�� mysql���� �����ϴ� ��ȣȭ ������.
		return 1; //����
	else
		return 0; //����

}
void mainatitleimage(void) {
	WHITE
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
	printf("��  ���� ����   ��                      ��   ���� �߰�  ��                      ��              ��"); gotoxy(12, 24);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 25);
	printf("��              ��                      ��              ��                      ��              ��"); gotoxy(12, 26);
	printf("����������                      ����������                      ����������");
}
int maintitle(void) { //���� ����Ÿ��Ʋ ���
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
		else if (7 <= xx && xx <= 13 && 21 <= yy && yy <= 25 && lr == 1)
			return 1;
		else {
			gotoxy(16, 23);
			WHITE printf("���� ����");
			gotoxy(56, 23);
			WHITE printf("���� �߰�");
		}

		gotoxy(0, 0);
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
void banglist(MYSQL *cons, int j) {
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
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
			if (9 <= xx && xx <= 22 && 2 == yy)			//�游���
				return 0;
			else if (24 <= xx && xx <= 37 && 2 == yy)		//��������
				return 1;
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
	if (connectroom[roomnum].ip[0] == 0)
		return -1;
	CLS;
	WHITE
		printf("��������������������������\n");
	printf("��                                              ��\n");
	printf("��              ĳġ���ε� �� ����              ��\n");
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
	listen_sock = socket(PF_INET, SOCK_STREAM, 0); // ���� ���� �� ���Ͽ� ���� ������ listen_sock������ ����					
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling("socket() error");
	printf("���� ���� �Ϸ�!\n");
	memset(&listen_addr, 0, sizeof(listen_addr)); // ������ �ּ� �ʱ�ȭ
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ������ �ּҸ� �� �ּҷ� (�Ƹ���)
	listen_addr.sin_port = htons(5555); // ���� ��Ʈ 
	printf("�ּ� ���� �Ϸ�!\n");
	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR) // ���ݱ��� ������ �ּҸ� listen_sock�� bind()�� ����
		ErrorHandling("bind() error");
	printf("bind() �Ϸ�!\n");
	if (listen(listen_sock, 5) == SOCKET_ERROR)	// Ŭ���̾�Ʈ�� �����Ҷ� ���� ��ٸ�
		ErrorHandling("listen() error");
	printf("listen() �Ϸ�!\n");
	sockaddr_in_size = sizeof(connect_addr);
	*count = 1;
	while (1) {
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
void SDL_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, SDL_GetError());
	return;
}
void IMG_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	printf("%s Error: %s\n", msg, IMG_GetError());
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
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window) {
	SDL_DestroyRenderer(Renderer);// SDL ������ �ı�
	SDL_DestroyWindow(Window);//SDL ������ �ı�
	SDL_Quit();//SDL ����
	getchar();//�����ڵ� Ȯ���ϱ� ���� �ܼ�â ���
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
		strong *= 80 / 50.0;
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
void ReceiveRender(SDL_Renderer* Renderer4, bool eraser, bool pencil, bool drag, int x, int y, float strong, float r, float g, float b) {
	cur(0, 0);
//	printf("eraser: %d pencil: %d drag: %d x: %d y: %d strong: %f r: %f g: %f b: %f       \n", eraser, pencil, drag, x, y, strong, r, g, b);
	if (SDL_Clear == true) {
		printf("clear�� �����ҰԽ�\n");
		SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);
		SDL_RenderClear(Renderer4);
		SDL_RenderPresent(Renderer4);
		SDL_Clear = false;
	}
	else {
		if (pencil == true && drag == false) {//eraser ���¿��� Ŭ���� ���
			ReceiveRect.x = x - strong / 2;
			ReceiveRect.y = y - strong / 2;// ���⸸ŭ�� �簢���� ����
			ReceiveRect.w = ReceiveRect.h = strong;// ���� ����
			SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
			SDL_RenderFillRect(Renderer4, &ReceiveRect);// �������� �׸�
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == false) {
			strong *= 80 / 50.0;
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
			strong *= 50.0 / 80;\
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (pencil == true && drag == true) {

			printf("pencil �巡�׹� �����ҰԽ�\n");
			float i = 0, j = 0, k = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// ���������� ���̸� ��Ÿ����� ������ ����. �̶� ������ ���� ���� ���� �巡���� ���� ���� ����
		//	printf("length:%f\n", length);
			if (length == 0) return;
		//	printf("clicks.pencil %d\n", clicks.pencil);
		//	if (pencil == true) {// �潽�� ���
				SDL_SetRenderDrawColor(Renderer4, r, g, b, 0);
				printf("rgb���� %d %d %d��\n", r, g, b);
				i = (x - (ReceiveRect.x + ReceiveRect.w / 2)) / length;// i�� ������ x��ǥ�� ���̸� ���̷� ���� ����.
				j = (y - (ReceiveRect.y + ReceiveRect.h / 2)) / length;// j�� ������ y��ǥ�� ���̸� ���̷� ���� ����.
				k = 0;// while���ȿ� �� ���� �ʱ�ȭ.
				xpos = ReceiveRect.x + ReceiveRect.w / 2 - strong / 2;// ���������� x��ǥ�� ���� ����
				ypos = ReceiveRect.y + ReceiveRect.h / 2 - strong / 2;// ���������� y��ǥ�� ���� ����
				ReceiveRect.w = ReceiveRect.h = strong;// ���⼳��
				for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
					ReceiveRect.x = xpos + k*i;// ���� ���� ������ �������� x��ǥ�� ���� 
					ReceiveRect.y = ypos + k*j;// ���� ���� ������ �������� y��ǥ�� ����
					SDL_RenderFillRect(Renderer4, &ReceiveRect);//�簢�� �������� ����
				}
		//	}
			SDL_RenderPresent(Renderer4);
			return;
		}
		else if (eraser == true && drag == true) {
			strong *= 80 / 50.0;
			float i = 0, j = 0, k = 0, l = 0, xpos = 0, ypos = 0;
			float length = sqrt(pow(ReceiveRect.x + strong / 2 - x, 2) + pow(ReceiveRect.y + strong / 2 - y, 2));// ���������� ���̸� ��Ÿ����� ������ ����. �̶� ������ ���� ���� ���� �巡���� ���� ���� ����
			SDL_SetRenderDrawColor(Renderer4, 255, 255, 255, 0);// ���찳�ϱ� ������ �Ͼ������									
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
			SDL_RenderPresent(Renderer4);
			return;
		}
	}
}
void SDL_RenderUpdate(SDL_Renderer* Renderer, SDL_Renderer* Renderer2, SDL_Renderer* Renderer3, SDL_Texture* TraTexture, SDL_Texture* BoxTexture, SDL_Texture* EraTexture, SDL_Texture* PenTexture, SDL_Texture* NewTexture, SDL_Rect Track, SDL_Rect Box, SDL_Rect Eraser, SDL_Rect Pencil, SDL_Rect New, SDL_Rect *Font, float strong, int r, int g, int b) {
	SDL_SetRenderDrawColor(Renderer2, r, g, b, 0);// ������
	RenderTexture(Renderer, TraTexture, &Track);// �������� �����ϱ�
	RenderTexture(Renderer, BoxTexture, &Box);// �������� �����ϱ�
	RenderTexture(Renderer, EraTexture, &Eraser);// �������� �����ϱ�
	RenderTexture(Renderer, PenTexture, &Pencil);// �������� �����ϱ�
	RenderTexture(Renderer, NewTexture, &New);// �������� �����ϱ�
	if (on.eraser == true || clicks.eraser == true) { // eraser�� Ŭ�� �Ǿ� �ְų� eraser���������� ���콺�� ������
		SDL_RenderRemoveEdge(Renderer, &Eraser);
		RenderTexture(Renderer, EraTexture, &Eraser);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, &Eraser, clicks.eraser);
	}
	if (on.pencil == true || clicks.pencil == true) { // pencil�� Ŭ�� �Ǿ� �ְų� pencil���������� ���콺�� ������
		SDL_RenderRemoveEdge(Renderer, &Pencil);
		RenderTexture(Renderer, PenTexture, &Pencil);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, &Pencil, clicks.pencil);
	}
	if (on.new == true) {
		SDL_RenderRemoveEdge(Renderer, &New);
		RenderTexture(Renderer, NewTexture, &New);// �������� �����ϱ�
		SDL_RenderDrawEdge(Renderer, &New, 0);
	}
	if (clicks.eraser == true || clicks.pencil == true)
		SDL_FontUpdate(Renderer, Font, Track, strong, r, g, b);
	SDL_RenderPresent(Renderer);// ������ ���
	SDL_RenderPresent(Renderer2);
	SDL_RenderPresent(Renderer3);
	SDL_RenderRemoveEdge(Renderer, &Eraser);
	SDL_RenderRemoveEdge(Renderer, &Pencil);
	SDL_RenderRemoveEdge(Renderer, &New);
}
int SDL_MAINS(void) {// �� ������ SDL.h�� ����� �����Լ��� �츮�� ���� ���� ������ �ƴ�, ���� �Ű������� ���������
	SDL_Window * Window;//SDL ������ ����
	SDL_Renderer * Renderer;// SDL ������ ���� 
	SDL_Window * Window2;
	SDL_Renderer * Renderer2;
	SDL_Window * Window3;
	SDL_Renderer * Renderer3;
	SDL_Rect center = { 0 };
	char query[256];
	// �ؽ��Ŀ� �簢�� ����
	SDL_Texture * RgbTexture = nullptr;// ������ �̹����� ������� �ؽ��� ����
	SDL_Texture * PenTexture = nullptr;// �� �̹����� ������� �ؽ��� ����
	SDL_Texture * EraTexture = nullptr;// ���찳 �̹����� ������� �ؽ��� ����
	SDL_Texture * NewTexture = nullptr;// ���θ���� �̹����� ������� �ؽ��� ����
	SDL_Texture * TraTexture = nullptr;// ��ũ�� Ʈ�� �̹����� ������� �ؽ��ļ���
	SDL_Texture * BoxTexture = nullptr;// ��ũ�� �ڽ� �̹����� ������� �ؽ��� ����
	SDL_Texture * ChaTexture = nullptr;// ä��â �̹����� ������� �ؽ��� ����
	SDL_Texture * StaTexture = nullptr;// ����â �̹����� ������� �ؽ��� ����
	SDL_Rect RgbCode = { 0 };// RgbCode �̹����� ������ ������� �簢������ ����
	SDL_Rect Pencil = { 0 }; // Pencil �̹����� ������ ������� �簢�� ���� ����
	SDL_Rect Eraser = { 0 }; // Eraser �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect New = { 0 }; // New �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Track = { 0 };// Track �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Box = { 0 };//Box �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Chat = { 0 };// Chat �̹����� ������ ��� ���� �簢�� ���� ����
	SDL_Rect Status = { 0 };//Status �̹����� ������ ��� ���� �簢�� ���� ����
							// �ؽ��Ŀ� �簢�� ���� ��
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {// SDL_Init�Լ��� SDL �ʱ�ȭ�ϰ� �ʱ�ȭ �ȵǸ� if�� ���� SDL_Init�� �μ��� �پ���(ex : SDL_INIT_VIDEO)
		SDL_ExceptionRoutine(Renderer, Window, "SDL_Init", 1);//�ܰ�1�� ���� ó�� ��ƾ����
		return 0;// ����
	};
	// ������â 3���� ������ ���� x��ǥ�� 1920 - 1310/4-10�̰�, 1080-900/4-10�� y��ǥ�� �����̴�.
	Window = SDL_CreateWindow("HIT MIND WITH C", (1920 - 1310 / 4 - 10)*(1), 0, (1310 / 4 + 10)*(1), 1080 * (1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ExceptionRoutine(Renderer, Window, "SDL_CreateWindow", 2);//�ܰ�2�� ����ó����ƾ����
		return 0;//����
	}
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Renderer == nullptr) {
		SDL_ExceptionRoutine(Renderer, Window, "SDL_CreateRenderer", 3);
		return 0;
	}
	Window2 = SDL_CreateWindow("HIT MIND WITH C 2", 0, 0, (1920 - 1310 / 4 - 10)*(1), (1080 - 900 / 4 - 10)*(1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window2 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ExceptionRoutine(Renderer2, Window2, "SDL_CreateWindow2", 2);//�ܰ�2�� ����ó����ƾ����
		return 0;//����
	}
	Renderer2 = SDL_CreateRenderer(Window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer �Լ��� SDL Renderer ���� �Լ� ȣ��� �Ѱ��ִ� �μ��� SDL_Window *, ����̹� ����(-1�̸� �˾Ƽ� ������), �÷���(������ �ϵ����Ӱ� ��������ȭ ����� �����)
	if (Renderer2 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ExceptionRoutine(Renderer2, Window2, "SDL_CreateRenderer2", 3);//�ܰ�3�� ���� ó�� ��ƾ ����
		return 0;// ����
	}
	Window3 = SDL_CreateWindow("HIT MIND WITH C 3", 0, (1080 - 900 / 4 - 10)*(1), (1920 - 1310 / 4 - 10)*(1), (900 / 4 + 10)*(1), SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);// SDL_CreateWindow �Լ��� SDL ������ ���� �Լ�ȣ��� �Ѱ��ִ� �μ��� ���ʴ�� â�̸�, â�� x����ġ, â�� y����ġ, â�� �ʺ�, â�� ����, �÷�����
	if (Window3 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ExceptionRoutine(Renderer3, Window3, "SDL_CreateWindow3", 2);//�ܰ�2�� ����ó����ƾ����
		return 0;//����
	}
	Renderer3 = SDL_CreateRenderer(Window3, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);// SDL_CreateRenderer �Լ��� SDL Renderer ���� �Լ� ȣ��� �Ѱ��ִ� �μ��� SDL_Window *, ����̹� ����(-1�̸� �˾Ƽ� ������), �÷���(������ �ϵ����Ӱ� ��������ȭ ����� �����)
	if (Renderer3 == nullptr) {// ������ ���� ���н� if�� ����
		SDL_ExceptionRoutine(Renderer3, Window3, "SDL_CreateRenderer3", 3);//�ܰ�3�� ���� ó�� ��ƾ ����
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
	const Uint8 * keystate;// Key ���� �迭�� �ޱ� ���� ������ ����
						   // RgbCode �̹���
	RgbTexture = LoadTextureEx(Renderer, "RgbCode.jpg", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (RgbTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
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
	TraTexture = LoadTextureEx(Renderer, "Track.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (TraTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
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
	BoxTexture = LoadTextureEx(Renderer, "Box.png", 255, 255, 255, 0, &center, SDL_FLIP_NONE);// �̹��� �ҷ�����
	if (BoxTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
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
	PenTexture = LoadTexture(Renderer, "Pencil.jpg"); // �̹��� �ҷ�����
	if (PenTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	SDL_QueryTexture(PenTexture, NULL, NULL, &Pencil.w, &Pencil.h);
	Pencil.w /= 15;
	Pencil.h /= 15;
	Pencil.x = Track.x + 50 + 40;
	Pencil.y = Track.y - 30 - Pencil.h;
	// ��
	// Eraser �̹���
	EraTexture = LoadTexture(Renderer, "Eraser.jpg"); // �̹��� �ҷ�����
	if (EraTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	Eraser.w = Pencil.w;
	Eraser.h = Pencil.h;
	Eraser.x = Pencil.x + 50 * (1) + 30 * (1);;
	Eraser.y = Pencil.y;
	// ��
	// New �̹���
	NewTexture = LoadTexture(Renderer, "New.jpg"); // �̹��� �ҷ�����
	if (NewTexture == nullptr) {// �����ڵ� ���
		IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
		IMG_ExceptionRoutine(Renderer2, Window2);
		IMG_ExceptionRoutine(Renderer3, Window3);
		return 0;
	}
	New.w = Eraser.w;
	New.h = Eraser.h;
	New.x = Eraser.x + 50 * (1) + 30 * (1);
	New.y = Eraser.y;

	// ��
	//DWORD th = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)thread, &SDL, 0, 0);
	bool quit = false;//�� ���� ����
	bool drag = false;// �巡�������� Ȯ���ϴ� ���� ����
	bool happen = true;
	int alpha;// ���� ä���� ������� ���� ����
	int x, y; // �����̰� �������� ���콺�� ��ǥ�� ������� ���� ����
	float r = 0, g = 0, b = 0; //rgb���� ���� ���� ���� ������ ������ �ϹǷ� �Ǽ������� ����
	float i = 0, j = 0, k = 0, l = 0, length = 0;// for������ �� ��������
	int newclick = 0;
	float xpos = 0, ypos = 0;// ���콺 x��ǥ y��ǥ�� �����ϴ� �������� 
	float strong = 49 * (float)(Box.x + Box.w / 2 - Track.x) / Track.w + 1;// ������ ����
	SDL_Rect Rect = { 0 }; // �׸� �簢���� ������ �ݺ��� �ۿ��� ����
	SDL_Rect Font = { Track.x - strong / 2 + 35 ,Track.y - strong / 2 - 50,strong,strong };// ����, ������� �����ֱ� ���� �簢�� ���� ����
	SDL_Rect Edge = { 0 };// �׵θ��� �׸��� ���� �簢�� ���� ���� 
						  // ��
						  // while������ �� ������ �ʱⰪ ����
	RenderTexture(Renderer, RgbTexture, &RgbCode);// �������� �����ϱ�
												  // ���� �ʱⰪ ������
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
	while (!quit) {// quit�� true�� �ƴҶ� ���� ���ѹݺ�
		if (SDL_PollEvent(&event)) {//�̺�Ʈ�� ������ if�� ����
			switch (event.type) {//�̺�Ʈ Ÿ�Կ� ���� ���̽��� ����
			case SDL_WINDOWEVENT://SDL���� Ÿ���� ���
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:// �ټ� â������ �ݱ��̺�Ʈ�� �߻��Ұ��
					quit = true;// quit�� true�� ����
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
							for (k = 0; k < length; k++) {// �� �������� ������ ���� �簢������ ä��� �ݺ�����
								Rect.x = xpos + k*i;// ���� ���� ������ �������� x��ǥ�� ���� 
								Rect.y = ypos + k*j;// ���� ���� ������ �������� y��ǥ�� ����
								SDL_RenderFillRect(Renderer2, &Rect);//�簢�� �������� ����
							}
							// ����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
						}
						else if (clicks.eraser == true) {// ���찳 ���
							strong *= 80 / 50.0;
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
							SDL_RenderFillRect(Renderer, &Font);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
							clicks.eraser = true;
							clicks.pencil = false;
							happen = true;
						}
						else if ((event.button.x >= New.x - 10 && event.button.x <= New.x + New.w + 10) && (event.button.y >= New.y - 10 && event.button.y <= New.y + New.h + 10)) {		//New �̹����� Ŭ��������
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
							SDL_RenderClear(Renderer2);
							Font.w += 2;// �Ϻ��� ���� �ƴ϶� �ɲ� ��������
							Font.h += 2;
							newclick = 1;
							//����~~~~~~~~~~~~~~~~~~
							if (connect_sock != 0) {
								send(connect_sock, "SDLCLEAR", 45, 0);
							}
							SDL_RenderFillRect(Renderer, &Font);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
							clicks.eraser = false;
							clicks.pencil = false;
							happen = true;
						}

						else if ((event.button.x >= Pencil.x - 10 && event.button.x <= Pencil.x + Pencil.w + 10) && (event.button.y >= Pencil.y - 10 && event.button.y <= Pencil.y + Pencil.h + 10)) {
							Font.w += 2;// �Ϻ��� ���� �ƴ϶� �ɲ� ��������
							Font.h += 2;
							SDL_RenderFillRect(Renderer, &Font);// ��Ʈ�� �����. �ٵ� ����̹Ƿ� �����ִ� ������ �ϰԵ�
							clicks.eraser = false;
							clicks.pencil = true;
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
								sprintf(query, "%d %d %d %d %d %.1f %.0f %.0f %.0f", clicks.eraser, clicks.pencil,drag,event.button. x, event.button.y,strong, r, g, b);
								send(connect_sock, query, 45, 0);
							}
							drag = true; //�巡�׷� �׸��� �ְ� ����
							happen = true;
					
							
							break;
						}
						else if (clicks.eraser == true) {
							strong *= 80 / 50.0;
							SDL_SetRenderDrawColor(Renderer2, 255, 255, 255, 0);
							int x1, y1, x2, y2;
							Rect.x = event.button.x;
							Rect.y = event.button.y;// ���⸸ŭ�� �簢���� ����
							for (l = 0; l < 180; l++) {
								x1 = sin(3.14 / 180 * l)*strong / 2;
								y1 = cos(3.14 / 180 * l)*strong / 2;
								x2 = sin(3.14 / 180 * (360 - l))*strong / 2;
								y2 = cos(3.14 / 180 * (360 - l))*strong / 2;
								SDL_RenderDrawLine(Renderer2, x1 + Rect.x, y1 + Rect.y, x2 + Rect.x, y2 + Rect.y);
							}
							// ����~~~~~~~~~~~~~~
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
		if (happen == true) {
			SDL_RenderUpdate(Renderer, Renderer2, Renderer3, TraTexture, BoxTexture, EraTexture, PenTexture, NewTexture, Track, Box, Eraser, Pencil, New, &Font, strong, r, g, b);

		}
		happen = false;

	}
	SDL_DestroyTexture(RgbTexture);// �ؽ��� �ı��ϱ�
	SDL_DestroyTexture(ChaTexture);
	SDL_DestroyTexture(BoxTexture);
	SDL_DestroyTexture(TraTexture);
	SDL_DestroyTexture(EraTexture);
	SDL_DestroyTexture(PenTexture);
	SDL_DestroyTexture(NewTexture);
	IMG_ExceptionRoutine(Renderer, Window);//IMG����ó����ƾ ����
	IMG_ExceptionRoutine(Renderer2, Window2);
	IMG_ExceptionRoutine(Renderer3, Window3);
	return 0;// ����
}