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
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))
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
//���� ������ (��� ����õ)
//�̺�Ʈ
CRITICAL_SECTION cs;
char message[100];

//�⺻ �Լ���
void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
//--------------------- ��Ʈ��ũ �Լ��� -----------------------------------
void ErrorHandling(char *Message);				//���� ���� ��� �ϴ� �Լ�
void Connect_Server(WSADATA wsaData, SOCKET connect_sock, SOCKADDR_IN connect_addr,char *ServerIP); //���� ���� ���ִ� �Լ�
void recieve(SOCKET connect_sock);	//�������� ������ �޾ƿ��� ������� �Լ�
//--------------------- MySQL �Լ��� --------------------------------------
void loadmysql(MYSQL *cons, char mysqlip[]);	//MySQL�� �����ϴ� �Լ�
char **onemysqlquery(MYSQL *cons, char *query); //mysql ��ɾ��� ����ϳ��� �ٷ� ��ȯ���ִ� �Լ�
void writechating(MYSQL *cons);
void readchating(MYSQL *cons);

// -------------------- SDL �׷��� �Լ��� ---------------------------------
void SDL_ErrorLog(const char * msg);			//�׷��ȿ����ڵ� ��� �Լ�
void IMG_ErrorLog(const char * msg);			//�̹��������ڵ� ��� �Լ�
void SDL_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window, char* msg, int step);	 // �׷��ȿ��� ó�� �Լ�
void IMG_ExceptionRoutine(SDL_Renderer* Renderer, SDL_Window* Window);						 // �̹������� ó�� �Լ�
SDL_Texture * LoadTexture(SDL_Renderer * Renderer, const char *file);						  // �ؽ��Ŀ� �̹������� �ε��ϴ� �Լ� ����
SDL_Texture * LoadTextureEx(SDL_Renderer * Renderer, const char *file, int r, int g, int b, int angle, SDL_Rect * center, SDL_RendererFlip flip);  // �ؽ��Ŀ� �̹������� �پ��ϰ� �ε��ϴ� �Լ� ����
void RenderTexture(SDL_Renderer* Renderer, SDL_Texture * Texture, int x, int y, int w, int h);	//�ؽ��ĸ� ����ϴ� �Լ� ����
// -------------------- ���� ���� �Լ��� ----------------------------------

//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);
void login();
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
	char mysqlip[30] = "10.80.161.182";		//mysql ip ����ip�Դϴ�
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
	//���� ���� �ڵ�ϱ� ȸ�������̶� �α��ο� �� �����ñ�
	char id[100] = { 0, }; //���̵����� �迭
	char pass[100];        //��й�ȣ���� �迭 
	int i = 0;

	/*��й�ȣ ��ȣȭ ����*/
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

	/*��й�ȣ ��ȣȭ ����*/
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
		sprintf(query, "insert into catchmind.chating (name, mean) values ('������', '%S')", buffer);
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
	exit(1);	//������ �߻������� �״�� ���α׷� ����
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
	if(connect(connect_sock, (SOCKADDR*)&connect_addr, sizeof(connect_addr))) //������ ����
		ErrorHandling("connect() error");
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recieve, &connect_sock, 0, NULL); //�������� �����͸� �޾ƿ��� ������ ����
	while (1) { //�޾ƿ� ������ ó��

	}
}
void recieve(SOCKET connect_sock) { //�������� ������ �޾ƿ��� ������� �Լ�
	while (1) {
		if (recv(connect_sock, message, 1023, 0)>0) { //�������� �����͸� �޾ƿ� message������ ����

		}
	}
}