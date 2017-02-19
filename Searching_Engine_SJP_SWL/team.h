#define  _CRT_SECURE_NO_WARNINGS	//warning message가 나오지 않게 처리
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include <time.h>

#define TABLE_SIZE 11959	//해시 테이블 SIZE(소수)

/*word별로 string과 저장된 line을 저장할 구조체 element*/
typedef struct
{
	char word[40];			//저장될 word의 최대 char 크기는 40
	int line;				//word가 저장된 line을 저장
} element;

/*element는 해당 item과 같은 element를 가리킬 수 있는 link를 가짐*/
typedef struct list_node
{
	element item;
	struct list_node *link;
} list_node;

/*hash_table에 있는 node를 가리킬 pointer*/
typedef struct Hash
{
	list_node *head, *rear;
}Hash;

Hash hash_table[TABLE_SIZE];	//hash_table은 전역변수로 선언
FILE *ofp;			//open file pointer
int is_file_opened = 0;		//file이 열려있는지 판단하는 변수

unsigned int hash_function(char *key)
{
	/*TABLE_SIZE를 hashing에 이용*/
	unsigned long hash = TABLE_SIZE;
	int c;
	/*string key에 (hash << 5)을 통해 추가 위치 연산*/
	while (c = *key++)
		hash = ((hash << 5) + hash) + c;
	/*TABLE_SIZE로 나눈 나머지로 index 추출*/
	return hash % TABLE_SIZE;
}

/*검색기 초기화 함수*/
void initialize()
{
	int i;
	for (i = 0; i<TABLE_SIZE; i++)
	{
		/*hash_table 초기화*/
		hash_table[i].head = hash_table[i].rear = NULL;
	}

}

/*체이닝 함수*/
void hash_chain_add(element item)
{
	/*hash_function으로 index 추출*/
	int hash_value = hash_function(item.word);
	list_node *new_node;

	/*해당 노드 생성, 초기화, item 할당*/
	new_node = (list_node *)malloc(sizeof(list_node));
	new_node->item = item;
	new_node->link = NULL;

	/*해당 hash_table가 비었을 경우*/
	if (hash_table[hash_value].head == NULL && hash_table[hash_value].rear == NULL)
	{
		/*new_node를 첫 노드로 삽입*/
		hash_table[hash_value].head = hash_table[hash_value].rear = new_node;
	}
	else
	{
		/*노드의 뒤에 이어서 new_node 삽입*/
		hash_table[hash_value].rear->link = new_node;
		hash_table[hash_value].rear = new_node;
	}
}

/*탐색 및 출력 함수*/
void search_and_print(char *keyword, char *fname)
{
	list_node *node;
	int flag = 0;		//해당 keyword가 존재하는지 판별 하는 flag
	int line[100000];		//해당 string이 나타난 line들을 저장할 배열
	int search_count = -1;
	int i;
	int hash_value = hash_function(keyword);

	/*data.txt파일이 열린지 확인 후 열려 있지 않으면 open*/
	if (!is_file_opened)
		ofp = fopen(fname, "w");
	is_file_opened = 1;
	/*hash_table의 처음부터 끝까지(해당 단어에 대한 line들을 모두 탐색*/
	for (node = hash_table[hash_value].head; node; node = node->link)
	{
		/*해당 문자열이 keyword와 같으면, flag를 1로 변환하고 line[]에 저장*/
		if (strcmp(node->item.word, keyword) == 0)
		{
			flag = 1;
			/*search_count는 0부터 추가됨*/
			line[++search_count] = node->item.line;
		}
	}
	/*flag가 1이면, line이 존재하므로 해당 line[] 출력*/
	if (flag)
	{
		/*0~n까지이므로 개수는 n-0+1 = n+1*/
		fprintf(ofp, "%s %d\n", keyword, search_count + 1);
		for (i = 0; i <= search_count; i++)
		{
			fprintf(ofp, "%d ", line[i]);
		}
		fprintf(ofp, "\n");
	}
	/*flag가 0이면, line이 존재하지 않으므로 해당 keyword 0을 출력*/
	else
	{
		fprintf(ofp, "%s 0\n", keyword);
	}
}

/*file로 부터 data.txt를 읽어들여서 띄어쓰기와 개행문자로 구분하여 저장*/
void read_from_file(char *fname)
{
	element input;
	int file_line = 0;
	FILE *f;
	char p[256];		//보통 textfile에는 255 or 256개의 char가 존재
	input.line = 0;

	f = fopen(fname, "r");	//data.txt를 read only로 file open

	/*data.txt를 \n로 구분하여 한 줄씩 읽어들여서 토큰화*/
	while (fgets(p, sizeof(p), f) != NULL)
	{
		char *token;
		char *delimiter = " \t\n";		//구분하려는 문자(" ", enter, tab)
		file_line++;			//fgets가 한번 돌 때마다 line + 1
		token = strtok(p, delimiter);	//delimiter를 제외하고 다시 저장

		/*저장된 token을 하나 뽑아내서 word에 저장하고 다시 토큰화*/
		while (token != NULL)
		{
			strcpy(input.word, token);
			input.line = file_line;
			hash_chain_add(input);
			token = strtok(NULL, delimiter);
		}
	}
}
