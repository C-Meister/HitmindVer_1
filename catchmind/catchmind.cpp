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
// ������� ����
//�⺻ �������
#include <stdio.h>
#include <conio.h>	
#include <windows.h>
#include <process.h>		//process ��Ƽ�������

//Ư�� ������� (���� ��ġ)
#include <SDL.h>			//SDL - �׷��� �������
#include <mysql/mysql.h>	//mysql ���� ������������ <mysql.h>�� ��ȯ


// ���̺귯�� ����
// #pragma comment �� visual studio������ ��� ����
#pragma comment (lib, "libmysql.lib")	//mysql���̺귯��
#pragma comment (lib, "SDL2")			//�׷��� ���̺귯�� 1
#pragma comment (lib, "SDL2main")		//�׷��� ���̺귯�� 2
#pragma comment (lib, "ws2_32.lib")		//����(��Ʈ��ũ)���̺귯��
//#define ���ǹ�
#define CLS system("cls")		//ȭ�� �����
#define gotoxy(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //Ŀ���̵�
#define cur(X,Y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { X, Y }) //Ŀ���̵�(����)
#define setcolor(X, Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X | (Y << 4))
//�⺻ �Լ���

int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� �������
{

	cur(5, 5);
	printf("hello��");
	return 0;

}
