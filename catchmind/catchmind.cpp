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
#include <windows.h>
#include <process.h>		//process ��Ƽ�������
#include <iostream>
//#include <WinSock2.h>		//�������α׷���

//Ư�� ������� (���� ��ġ) 
#include "SDL/SDL.h"			//SDL - �׷��� �������
#include "SDL/SDL_image.h"
			//���� ����
//#include <mysql/mysql.h>	//DB�������

#include "mysql/mysql.h"


// ���̺귯�� ���� ���̺귯�������� ���� �߰����ص� �˴ϴ�.
// #pragma comment �� visual studio������ ��� ���� *�ַ�� �÷����� 64��Ʈ�� ���ּ���
#pragma comment (lib, "libmysql.lib")	//mysql���̺귯��
#pragma comment (lib, "SDL2")			//�׷��� ���̺귯�� 1
#pragma comment (lib, "SDL2main")		//�׷��� ���̺귯�� 2
#pragma comment (lib, "SDL2_image")
#pragma comment (lib, "ws2_32.lib")		//����(��Ʈ��ũ)���̺귯��

#pragma warning (disable : 4101)		//������� ���� ���������Դϴ�. ��� ����
//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�(����)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))		//������ X��� Y ����
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

//���� ������ (��� ����õ)

INPUT_RECORD rec;
DWORD dwNOER;
HANDLE COUT = 0;    // �ܼ� ��� ��ġ
HANDLE CIN = 0;        // �ܼ� �Է� ��ġ

CRITICAL_SECTION cs;	//�̺�Ʈ
char message[100];		//���� ���α׷��� ���ڿ�
char username[30];		//����� �̸�

//�⺻ �Լ���
void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
//--------------------- ��Ʈ��ũ �Լ��� -----------------------------------
void ErrorHandling(char *Message);				//���� ���� ��� �ϴ� �Լ�
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr, char *ServerIP); //���� ���� ���ִ� �Լ�
void recieve(SOCKET connect_sock);				//�������� ������ �޾ƿ��� ������� �Լ�
//--------------------- MySQL �Լ��� --------------------------------------
int sqllogin(MYSQL *cons);						//mysql�� ����� �����͸� ���� �α����� �ϴ� �Լ�
int sqlsignup(MYSQL *cons);						//mysql�� ���������͸� �߰��ϴ� �Լ�
void loadmysql(MYSQL *cons, char mysqlip[]);	//mysql�� �����ϴ� �Լ�
char **onemysqlquery(MYSQL *cons, char *query); //mysql��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
void writechating(MYSQL *cons);					//mysql�� ä���� �Է��ϴ� �Լ�
void readchating(MYSQL *cons);					//mysqm�� ä���� �д� �Լ�

// -------------------- SDL �׷��� �Լ��� ---------------------------------
void SDL_ErrorLog(const char * msg);			//�׷��ȿ����ڵ� ��� �Լ�
void IMG_ErrorLog(const char * msg);			//�̹��������ڵ� ��� �Լ�
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // �׷��ȿ��� ó�� �Լ�
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // �̹������� ó�� �Լ�
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // �ؽ��Ŀ� �̹������� �پ��ϰ� �ε��ϴ� �Լ� ����
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h);	//�ؽ��ĸ� ����ϴ� �Լ� ����
// -------------------- ���� ���� �Լ��� ----------------------------------
void mainatitleimage();
void maintitle(); //���� ����Ÿ��Ʋ ���
void banglist();
void bangchose();
//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);						//�ܾ Ȯ����
LOG login();                                    //id ��й�ȣ�� ���Ŀ� �°� �Է����� 
void click(int *xx, int *yy);

//�Լ� ���� ��  �ɼ� ������ ������ �Լ��� ������ֽñ� �ٶ��ϴ�.



int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� ������� 
{
	//���� ����
	InitializeCriticalSection(&cs);
	int i, j, k, v, result;
	POINT pos;
	MYSQL *cons = mysql_init(NULL);			//mysql �ʱ�ȭ
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	char query[400];						//mysql ��ɾ ������
	char mysqlip[30] = "10.80.162.92";		//mysql ip ����ip�Դϴ�	���� ������7����
	WSADATA wsaData;						//���� WSAStartup()�Լ��� ���̴� ����
	SOCKET connect_sock;					//���� ���Ϻ���
	SOCKADDR_IN connect_addr;				//���� �ּ����� �����ϴ� ����
	char *ServerIP = "10.80.162.41";		//���� ip ��ȣip��
	char data[1000][30] = { 0, };           //�ܾ����
	char nowword[30] = { 0, };              //�������� �ܾ�
	char scanword[30] = { 0, };             //���� ģ �ܾ�

	//���� ���� ��

	disablecursor(1);
	//	ConsoleL(30, 30);
#ifdef SANGHO
	

#else
#ifdef SANGHIE											//���� �׽�Ʈ��
	loadmysql(cons, mysqlip);
	bangchose();
	/*uintptr_t pc = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)writechating, cons, 0, NULL);
	uintptr_t ac = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)readchating, cons, 0, NULL);
	cur(120, 30);
	printf("-------------");
	while (1) {
		Sleep(1000);
		//	cur(0, 0);
		//	printf("%d %d  ", pos.x, pos.y);
	}*/
	return 0;
#else			//�� �����Լ��� ���������ּ���
	loadmysql(cons, mysqlip);
	maintitle();
	loadmysql(cons, mysqlip);
	sqllogin(cons);
	return 0;
#endif
#endif
}

LOG login() {
	//���� ���� �ڵ�ϱ� ȸ�������̶� �α��ο� �� �����ñ�
	LOG user = { 0, 0 };
	int i = 0;

	/*���̵� ��ȣȭ X*/
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
	/*��й�ȣ ��ȣȭ ����*/
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
		printf("���ο� ip�� ������ �ּ���.");
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
	fputs(Message, stderr);
	fputc('\n', stderr);
	return;
}
void SDL_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	std::cout << msg << " Error: " << SDL_GetError() << std::endl;
	return;
}
void IMG_ErrorLog(const char * msg) {//�����ڵ� ��� �Լ�
	std::cout << msg << " Error: " << IMG_GetError() << std::endl;
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
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h) {//�ؽ��ĸ� ����ϴ� �Լ� ����
	SDL_Rect Src;// ���簢�� ����
	Src.x = 0;// ���簢���� ������ �������� x��ǥ�ʱ�ȭ
	Src.y = 0;// ���簢���� ������ �������� y��ǥ�ʱ�ȭ
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h); // Texture�� �ʺ�� ���� ������ Src.w, Src.h�� ����
	SDL_Rect Dst;
	Dst.x = x;//�Ű�����x�� ������ �������� x��ǥ�� ����
	Dst.y = y;//�Ű�����y�� ������ �������� y��ǥ�� ����
	Dst.w = w;//�Ű�����w�� ���簢���� �ʺ� ����
	Dst.h = h;//�Ű�����h�� ���簢���� ���̿� ����
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst);//Src�� ������ ������ �ִ� Texture�� Dst�� ������ ���� Texture �� ��ȯ�Ͽ� �������� ����
}
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr, char *ServerIP) { //���� ���� ���ִ� �Լ�
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	//���� ���α׷��� ����
		ErrorHandling("WSAStartup() error");
	connect_sock = socket(PF_INET, SOCK_STREAM, 0);	//connect_sock������ ���� �Ҵ�
	connect_addr.sin_family = AF_INET;				//������ ������ �ּ� ����
	connect_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP); //���� IP
	connect_addr.sin_port = htons(5555);					 //���� ��Ʈ
	if (connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //������ ����
		ErrorHandling("connect() error");
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, &connect_sock, 0, NULL); //�������� �����͸� �޾ƿ��� ������ ����
	while (1) { //�޾ƿ� ������ ó��

	}
}
void recieve(SOCKET connect_sock) { //�������� ������ �޾ƿ��� ������� �Լ�
	while (1) {
		if (recv(connect_sock, message, 1023, 0) > 0) { //�������� �����͸� �޾ƿ� message������ ����

		}
	}
}
int sqllogin(MYSQL *cons) {
	LOG user;								//����� ���� ����ü
	MYSQL_RES *sql_result;					//mysql ����� ������ �����ϴ� ����
	MYSQL_ROW sql_row;						//mysql ����� ������ �ϳ��� �����ϴ� ����
	char query[100];
	user = login();							//login �Լ��� ���
	sprintf(query, "select * from catchmind.login where id = '%s'", user.id);	//id�� DB���� ã��
	mysql_query(cons, query);
	sql_result = mysql_store_result(cons);

	if (mysql_fetch_row(sql_result) == NULL)									//�؈� id�� ������ 
	{
		printf("\n���̵� �������� �ʽ��ϴ�.\n");
		return 0;
	}
	else
		printf("\n���̵� OK");
	sprintf(query, "select * from catchmind.login where password = password('%s')", user.pass); 
	mysql_query(cons, query);	//password�� DB�� ��ȣȭ�Ǿ� �־ ���� ���Ҷ��� ���� ��ȣȭ�ؼ� �񱳸���
	sql_result = mysql_store_result(cons);
	if (mysql_fetch_row(sql_result) == NULL)
	{
		printf("\n��й�ȣ�� Ʋ�Ƚ��ϴ�.\n");
		return 0;
	}
	else {
		printf("\n�α��� ����");
		return 1;
	}

}
int sqlsignup(MYSQL *cons) {
	LOG user;
	char query[100];
	printf("ȸ�������� �մϴ�. ��й�ȣ�� ��ȣȭ�Ǿ� �����̵˴ϴ�.\n");
	user = login();
	printf("\n�̸� : ");
	fgets(user.name, sizeof(user.name), stdin);
	CHOP(user.name);							//���ڿ��� �����̽��ٸ� ��������
	sprintf(query, "insert into catchmind.login (name, id, password) values ('%s', '%s', password('%s'))", user.name, user.id, user.pass);
	if (!(mysql_query(cons, query)))											//		 password�� mysql���� �����ϴ� ��ȣȭ ������.
	{
		printf("\nȸ������ ����");
		return 1;
	}
	else
	{
		printf("\nȸ������ ����");
		return 0;
	}

}
void mainatitleimage() {
	gotoxy(6, 3);
	printf("������  ��  ��      ����    ��    ������  ��        ����    ��      ������                                           ����");
	gotoxy(6, 4);
	printf("        ��  ��  ��                ��    ��      ��  ��      ��      ��  ��      ��                                                 ��      ��");
	gotoxy(6, 5);
	printf("  �����  ��  ��    ������  ��    ��      ��  ��      ��      ��  ��      ��              ��      ��   ��    ��    ��      ��");
	gotoxy(6, 6);
	printf("      ��    ����        ��      ��    ��      ��  ���      ����    ��      ������      ��      ��       ����  ��      ��");
	gotoxy(6, 7);
	printf("    ��      ��  ��      ��  ��    ��    ��      ��  ��                                          ��  ��  ��   ��    ��    ���    ��");
	gotoxy(6, 8);
	printf("  ��        ��  ��    ��      ��  ��    ��      ��  ��        ��                                ��  ��  ��   ��    ��    ��  ��    ��      ��");
	gotoxy(6, 9);
	printf("��          ��  ��    ��      ��  ��    ������  ��        �������    ��������      ��  ��     ��    ��    ��  ��      ����");
	gotoxy(12, 20);
	printf("����������                      ����������                      ����������");
	gotoxy(12, 21);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 22);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 23);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 24);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 25);
	printf("��              ��                      ��              ��                      ��              ��");
	gotoxy(12, 26);
	printf("����������                      ����������                      ����������");
}
void maintitle() { //���� ����Ÿ��Ʋ ���
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
	int mode;
	CIN = GetStdHandle(STD_INPUT_HANDLE); //���콺 ��Ȱ��ȭ
	GetConsoleMode(CIN, &mode);
	SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);
	ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER); // �ܼ�â �Է��� �޾Ƶ���.
	if (rec.EventType == MOUSE_EVENT) {// ���콺 �̺�Ʈ�� ���

		if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // ���� ��ư�� Ŭ���Ǿ��� ���
			int mouse_x = rec.Event.MouseEvent.dwMousePosition.X; // X�� �޾ƿ�
			int mouse_y = rec.Event.MouseEvent.dwMousePosition.Y; // Y�� �޾ƿ�

			*xx = mouse_x / 2;
			*yy = mouse_y;
		}
		else {
			*xx = 0;
			*yy = 0;
		}
	}

}
void banglist() {																				//���콺���� 2�� �������� �޴´�
	printf("                ��������������������������������\n"); //�游��� 9 ~ 22 , 2
	printf("                ��          �游���          ��          ��������          ��\n"); //�������� 24 ~ 37 , 2
	printf("                ��������������������������������\n");
	printf("                ��                          ����                          ��\n"); //���� 9 ~ 37, 4
	printf("                ��������������������������������\n");
	printf("                ��                            ��                            ��\n"); //��1 9 ~ 22 , 6 ~ 8 
	printf("                ��           1����            ��                            ��\n"); //��2 24 ~ 37 , 6 ~ 8
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
void bangchose() {

	int xx = 0, yy = 0;
	banglist();
	while (1) {
		printf("%3d %3d\n", xx, yy);
		
		click(&xx, &yy);

		if (9 <= xx && xx <= 22 && 6 <= yy && yy <= 8)
			break;

		gotoxy(0, 0);
	}

}