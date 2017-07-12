#pragma once
//��ó����
#ifndef CATCHMINDH
#define CATCHMINDH
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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

//extern 
INPUT_RECORD rec;
//extern
DWORD dwNOER;
//extern 
HANDLE COUT;    // �ܼ� ��� ��ġ
//extern 
HANDLE CIN;        // �ܼ� �Է� ��ġ

//extern 
CRITICAL_SECTION cs;	//�̺�Ʈ
extern char message[100];		//���� ���α׷��� ���ڿ�
extern char username[30];		//����� �̸�


#pragma warning (disable : 4101)		//������� ���� ���������Դϴ�. ��� ����
//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)X, (short)Y }) //Ŀ���̵�(����)
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



void ConsoleL(int x, int y);					//�ܼ�â�� ũ�⸦ �����ϴ� �Լ� x y�� �ʺ� ����
POINT MouseClick(void);							//���콺�� Ŭ���ϸ� �� ���� �ٷ� ��ȯ���ִ� �Լ� ��ȯ���� POINT�̴� (x, y)
void disablecursor(bool a);						//Ŀ�� ���̱�, �����  0 = ���̱� 1 = �����
void usermain(void);
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
int bangchose();
//-------------------------�ܼ� �Լ���------------------------------------
void checkword(char*nowword, char*scanword);						//�ܾ Ȯ����
LOG login();                                    //id ��й�ȣ�� ���Ŀ� �°� �Է����� 
void click(int *xx, int *yy);
//--------------------------�̴ϰ��� ���ھ߱� �Լ���----------------------
void createainumber(int *ainum);
void scanfirst(int *usnum);
void scan(int(*usre)[100], int *ainum, int turn);
void aiscan(int(*aire)[100], int *usnum, int(*aiall)[10][10], int t);
void aicheck(int(*aiall)[10][10], int(*aire)[100], int t);
void numberbaseball();
//�Լ� ���� ��  �ɼ� ������ ������ �Լ��� ������ֽñ� �ٶ��ϴ�.
#endif