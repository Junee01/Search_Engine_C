#define  _CRT_SECURE_NO_WARNINGS
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include <time.h>

#define TABLE_SIZE 11959

typedef struct
{
	char word[40];
	int line;
} element;

typedef struct list_node
{
	element item;
	struct list_node *link;
} list_node;

typedef struct Hash
{
	list_node * head, *rear;
}Hash;

Hash hash_table[TABLE_SIZE];
FILE *ofp;
int is_file_opened = 0;

unsigned int hash_function(char *key)
{
	unsigned int hash_index = 0;

	while (*key)
	{
		hash_index = hash_index + *key++;
	}

	return hash_index % TABLE_SIZE;
}

void initialize()
{
	int i;
	for (i = 0; i<TABLE_SIZE; i++)
	{
		hash_table[i].head = hash_table[i].rear = NULL;
	}

}

void hash_chain_add(element item)
{
	int hash_value = hash_function(item.word);
	list_node *new_node;

	new_node = (list_node *)malloc(sizeof(list_node));
	new_node->item = item;
	new_node->link = NULL;

	if (hash_table[hash_value].head == NULL && hash_table[hash_value].rear == NULL)
	{
		hash_table[hash_value].head = hash_table[hash_value].rear = new_node;
	}
	else
	{
		hash_table[hash_value].rear->link = new_node;
		hash_table[hash_value].rear = new_node;
	}
}

void search_and_print(char *keyword, char *fname)
{
	list_node *node;
	int flag = 0;
	int line[100000];
	int search_count = -1;
	int i;
	int hash_value = hash_function(keyword);

	if (!is_file_opened)
		ofp = fopen(fname, "w");
	is_file_opened = 1;
	for (node = hash_table[hash_value].head; node; node = node->link)
	{
		if (strcmp(node->item.word, keyword) == 0)
		{
			flag = 1;
			line[++search_count] = node->item.line;
		}
	}
	if (flag)
	{
		fprintf(ofp, "%s %d\n", keyword, search_count + 1);
		for (i = 0; i <= search_count; i++)
		{
			fprintf(ofp, "%d ", line[i]);
		}
		fprintf(ofp, "\n");
	}
	else
	{
		fprintf(ofp, "%s 0\n", keyword);
	}
}

void read_from_file(char *fname)
{
	element input;
	int file_line = 0;
	FILE *f;
	char p[256];
	input.line = 0;

	f = fopen(fname, "r");

	while (fgets(p, sizeof(p), f) != NULL)
	{
		char *token;
		char *delimiter = " \t\n";
		file_line++;
		token = strtok(p, delimiter);
		while (token != NULL)
		{
			strcpy(input.word, token);
			input.line = file_line;
			hash_chain_add(input);
			token = strtok(NULL, delimiter);
		}
	}
}

