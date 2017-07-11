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
#include <stdlib.h>
#include <time.h>
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
void loadmysql(MYSQL *cons, char mysqlip[]); //MySQL�� �����ϴ� �Լ�
char **mysqlquery(MYSQL *cons, char *query, int max);

int main(int argc, char **argv) //main�Լ� SDL������ �μ��� ������ �� �������
{
	
	//���� ����
	int i, j, k, v, result;
	MYSQL *cons = mysql_init(NULL);
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	char query[400];
	char mysqlip[30] = "10.80.161.182";
	//��
	loadmysql(cons, mysqlip);
	return 0;

}


void loadmysql(MYSQL *cons, char mysqlip[])	//MYSQL ���� �ҷ�����
{
	CLS;
	printf("�����ͺ��̽� �ҷ����� �õ���...");
	if (cons == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("ä�� ���� �ҷ����� ���� (�ʱ�ȭ ����)\n");
		Sleep(1000);
		exit(1);
	}
	else
		printf("1");
	if (mysql_real_connect(cons, mysqlip, "root", "tbvjaos15", NULL, 0, NULL, 0) == NULL)
	{
		printf("\b����... \n������ �������� �ʽ��ϴ�.\n");
		fprintf(stderr, "%s\n", mysql_error(cons));
		printf("���ο� ip�� ������ �ּ���.");
		scanf("%s", mysqlip);
		loadmysql(cons, mysqlip);
	}
	else {
		printf("\b2");
		printf("\n����");
		mysql_set_character_set(cons, "euckr");
		CLS;
	}

	return;
}
char **mysqlquery(MYSQL *cons, char *query, int max) {
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	mysql_query(cons, query);
	sql_result = mysql_store_result(cons);
	return mysql_fetch_row(sql_result);

}