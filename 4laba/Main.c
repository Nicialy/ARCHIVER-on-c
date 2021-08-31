#define _CRT_SECURE_NO_WARNINGS
#define N_CHAR 256

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>
#include <malloc.h>
#include <Windows.h>
#include <string.h>


typedef struct Node {
	int frequence;//частота
	char letter;//буква
	struct Node* left;//левый потмоок
	struct Node* right;//правый потомок
} Node;

typedef struct Spisok {//для подсчета чистоты и ввода символов
	int info;
	char let;
	struct Spisok *next;
} Spisok;

int proverkavvoda(int rez, int d);//Проверка , того что ввел пользователь
int decode(char str, Node **n, FILE *F, int *countch);
int bubble(Node** array);//Пузырек
int drevoHufmana(Node **m, Spisok **list);//построение древа
int fordecode(FILE *F, Node **tree, Node *tr);// табличка
int postrochno(FILE *F, Spisok **m);//считывание построчно
void freeList(Spisok **first);// память для списка
int dedrevoHufmana(Node **r, char* str, int *flag);// построение дерева из таблички
Node* newuzel(Node *m1, Node *m2);// создание нового узла из двух листьев
int massstruct(Spisok **m, char *str);// посимвольная запись, проверка в список
Node* createFirststruct(char str, int val);//созданеи первой структуры
int coderovka(Node**root, FILE *F);// закодирование в файл 
int SeekM(Node* tree, char c, char* s);// поиск по дереву кода буквы
void destrodyTree(Node *nod);// уничтоженеи дерева
int addLast(Spisok **first, int value, char c);//добавление в список последнего элемента

int nmass;
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "Russian");
	FILE *F,*Frazj;
	int rez, x, i, flagEndHafman = 0, k, count = 0, countchar = 0,ncountchar;
	char st[N_CHAR];
	unsigned char cl=0;
	_Bool fdec;
	char estr;
	Node **massnode = NULL,*Dec=NULL;
	Spisok *list = NULL,*cur;
	
	
	if (argc < 2)
	{
		printf("\nUSAGE: FilePaging filename\n");
		return 1;
	}
	
	printf("\nЧто делаем с файлом?\n");
	printf("1: Сжатие файла\n");
	printf("2: Разжатие файла\n");
	printf("Введите цифру\n");
	rez = scanf("%d", &x);
	if (!proverkavvoda(rez, x))
		return 1;
	switch (x)
	{
		case 1:
			if (!(F = fopen(argv[1], "rt")))
			{
				printf("\nНе удалось открыть файл\n");
				return 1;
			}
					
			if (postrochno(F,&list) == 1)
				return 1;
			if (!(massnode = (Node*)malloc(sizeof(Node)*nmass)))
			{
				printf("Ошибка выделения памяти");
				return 1;

			}
			cur = list;
			for (i = 0; i < nmass && cur != NULL; i++) {

				if (!(massnode[i] = createFirststruct(cur->let, cur->info)))
				{
					printf("Ошибка выделения памяти!");
					return 1;
				}
				
				cur = cur->next;
			}

			if (drevoHufmana(massnode,&list) == 1)
		     	return 1;
			freeList(&list);

			if (coderovka(massnode, F) == 1)
				return 1;
			fclose(F);
			destrodyTree(*massnode);
			free(massnode);
			break;
		case 2: 
			if (!(F = fopen("Cjatie.bin", "rb")))
			{
				printf("\nНе удалось открыть файл\n");
				return 1;
			}
			if (!(Frazj = fopen("Razjat.txt", "w")))
			{
				printf("\nНе удалось открыть ,создать файл\n");
				return 1;
			}
			
			while (1)
			{
				if (flagEndHafman == 0)
					estr = fgets(st, sizeof(st), F);
				else
					if (flagEndHafman==1)
						estr = fgets(st, sizeof(st), F);
					else
					{
						estr = fread(&cl, sizeof(unsigned char), 1, F);
						
					}

				if (estr == NULL||estr==0)
				{

					if (feof(F) != 0)
					{


						break;
					}
					else
					{

						printf("\nОшибка чтения из файла\n");
						return 1;
					}
				}
				if (Dec == NULL)
				{
					if (!(Dec = (Node*)malloc(sizeof(Node)))) {
						printf("Что-то не так с памятью....");
						return NULL;
					}
					Dec->left = NULL;
					Dec->right = NULL;


				}
				if (flagEndHafman == 0)
				{
					if (dedrevoHufmana(&Dec, st, &flagEndHafman) == 1)
						return 1;
					
				}
				else {
					if (flagEndHafman==1)
					{ 
						if ((ncountchar = atoi(st)) == 0)
						{
							printf("Ошибка в кол-ве символов!");
							return 1;
						}
						flagEndHafman = 2;
					}
					else 
					{
							for (; (count != 8)&& (countchar < ncountchar); count++)
							{
								fdec = cl & (1 << (7 - count));
								if (fdec == 0)
								{
									if (decode('0', &Dec, Frazj,&countchar) == 1)
										return 1;
								}
								else if (decode('1', &Dec, Frazj,&countchar) == 1)
									return 1;
							}
							if (count == 8)
							{
								count = 0;
								cl = 0;
							}
						
					}
					
				}




				
			}

			

			fclose(F);
			fclose(Frazj);

			destrodyTree(Dec);
			
			break;
	
	}
		
	
	
}
int proverkavvoda(int rez, int d) {
	if (rez == 0 || (d!=1 && d!=2) ) {
		printf("\nВведен символ или цифра не  соответствует 1 или 2\n");
		
		return 0;
	}
	return 1;
}
int postrochno(FILE *F,Spisok **m) {
	char estr;
	char st[N_CHAR];
	if (!F)
	{
		printf("\n Что-то не так с файлом...");
		return 1;
	}
	if (!m)
	{
		printf("\n Что-то не c spiskom");
		return 1;
	}
	
	nmass = 1;
	while (1)
	{
		
		estr = fgets(st, sizeof(st), F);

		
		if (estr == NULL)
		{
			
			if (feof(F) != 0)
			{
				
				
				break;
			}
			else
			{
				
				printf("\nОшибка чтения из файла\n");
				return 1;
			}
		}
		
		if (massstruct(m, st) == 1)
			return 1;

		
	}
	
	return 0;
}
int massstruct(Spisok **m, char *str)
{
	int k ,flagmass;
	Spisok *cur;
	if (!m)
	{
		printf("\n Что-то не c spiskom");
		return 1;
	}
	if (nmass == 1)
	{
		if (addLast(m, 0, str[0]) == 1)///****************************************************
			return 1;
		
	}
	for (k = 0; k < N_CHAR; k++) 
	{
		if (str[k] != '\0')
		{
			flagmass = 0;
			
			cur = *m;
			while (cur!= NULL) {
				if (cur->let == str[k])
				{
					cur->info = cur->info + 1;
					flagmass = 1;

				}
				cur = cur->next;

			}
					
						

			//********************
			if (flagmass == 0)
			{
				nmass = nmass + 1;
				if (addLast(m,1,str[k])==1)
					return 1;
				

			}
		}
		else
		{
			break;
		}
	}
	return 0;

}
int addLast(Spisok **first, int value,char c) {//поправить
	Spisok *newNode, *current;
	if (!first)///************
		return 1;
	if (!(newNode = (Spisok*)malloc(sizeof(Spisok))))
	{
		printf("Ошибка Выделения памяти для списка");
		return 1;
	}
	newNode->info = value;
	newNode->let = c;
	newNode->next = NULL;
	if (*first == NULL)/*Нет элементов*/
	{
		*first = newNode;
		return 0;
	}
	current = *first;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = newNode;
	return 0;
}
Node* createFirststruct(char s,int val) {
	Node *New;
	if (!(New = (Node*)malloc(sizeof(Node)))) {
		printf("Что-то не так с памятью....");
		return NULL;
	}
	New->letter = s;
	New->frequence = val;
	New->left = NULL;
	New->right = NULL;
	

	return New;

}
int drevoHufmana(Node**m ,Spisok **list) {
	
	
	if (!m||!list)
	{
		printf("Что-то не так с памятью....");
		return 1;
	}
	
	while (1) {
		if (bubble(m) == 1)
			return 1;
		if (nmass != 1)
		{
			
			m[nmass - 2] = newuzel(m[nmass-2],m[nmass-1]) ;
			if (m[nmass - 2] == NULL) {
				printf("Ошибкочка в памяти");
				return 1;
			}
			nmass = nmass - 1;
			if (!(*m = (Node*)realloc(*m, sizeof(Node) *  nmass)))
			{
				printf("Ошибка выделения памяти");
				return 1;

			}
		}
		else break;
	}

	return 0;
}
Node* newuzel(Node *m1, Node *m2)
{
	Node *New;
	//проверка на null
	if (!m1 || !m2)
	{
		printf("Что-то не так с памятью....");
		return NULL;

	}
	if (!(New = (Node*)malloc(sizeof(Node)))) {
		printf("Что-то не так с памятью....");
		return NULL;
	}
	if (m1->frequence > m2->frequence)
	{
	
		New->right = m1;
		
		New->left = m2;
		New->frequence = m1->frequence + m2->frequence;
	}
	else {
		
		New->right = m2;
		
		New->left = m1;
		New->frequence = m1->frequence + m2->frequence;

	}
	return New;
	
	// правый 1(больше) левый 0(меньше)

}
int dedrevoHufmana(Node **r,char* str,int *flag) {
	static int flagforN=0;
	int i,k;
	Node *cur,*cr;
	if (!r) 
	{
		printf("пАмять!");
		return 1;
	}
	cur = *r;
	k = 1;
	if (flagforN == 1)
		k = 0;
	if (str[0] == '\n')
	{
		flagforN = 1;
		return 0;
	}
	
	for (i = k; str[i] != '\n';i++)
	{
		
		switch (str[i]) {

			case '1':
				if (cur->right == NULL)
				{
					if (!(cr = (Node*)malloc(sizeof(Node)))) {
					printf("Что-то не так с памятью....");
					return NULL;
					}
					cr->left = NULL;
					cr->right = NULL;
					cur->right = cr;
					if (str[i + 1] == '\n')
						if (flagforN == 0)
							cr->letter = str[0];
						else {
							cr->letter = '\n';
							flagforN = 0;
						}
					cur = cur->right;
				}
				else cur = cur->right;
			
			break;
			case '0':
				if (cur->left == NULL)
				{
					if (!(cr = (Node*)malloc(sizeof(Node)))) {
						printf("Что-то не так с памятью....");
						return NULL;
					}
					cr->left = NULL;
					cr->right = NULL;
					cur->left = cr;
					if (str[i + 1] == '\n')
						if (flagforN == 0)
							cr->letter = str[0];
						else {
							cr->letter = '\n';
							flagforN = 0;
						}
					cur = cur->left;
				}
				else cur = cur->left;

			
			break;
			case '#':
				if (str[0] == '!') {
					*flag = 1;

				}

				break;
		default:
			printf("\n Ошибка в декодинге файла!!!!!!!!!!");
			return 1;
			break;
		}
	}

}
int decode(char str, Node **n,FILE *F,int *countch) {
	static Node *cur=NULL;
	
	
	if (!n||!F)
	{
		printf("Память!");
		return 1;
	}
	if (cur == NULL)
		cur = *n;
	
		switch (str)
		{
		case '1':
			
				cur = cur->right;
			if (cur->left == NULL && cur->right == NULL)
			{
				fprintf(F, "%c", cur->letter);
				*countch=*countch+1;
				cur = *n;
			}
			
			break;
		case '0':
			
				cur = cur->left;
			if (cur->left == NULL && cur->right == NULL)
			{
				fprintf(F, "%c", cur->letter);
				 *countch = *countch + 1;
				cur = *n;
			}
			
				
			
			break;

		default:
			printf("ОШИБКА В ФАЙЛЕ!");
			return 1;
			break;
		}

	



}
int bubble(Node** array)//Пузырек, очень не эффектиный, но с закрытыми глазами сойдет
{
	int i, j;
	Node *tmp;
	if (!array)
	{
		printf("Что-то не так с памятью....");
		return 1;
	}
	for (i = nmass - 1; i > 0; i--)
	{
		for (j = 0; j < i; j++)
			if (array[j]->frequence < array[j + 1]->frequence)
			{
				tmp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = tmp;
			}
	}
	return 0;
}
int coderovka(Node**root,FILE *F) {
	
	// правый 1(больше) левый 0(меньше)
	FILE *Fnew;
	char  s[N_CHAR];
	char   s1[N_CHAR] = "\0";
	unsigned char c=0;
	char estr;
	Node *fr = *root;
	int i, g, count = 0;
	if (!root || (!F) ) {

		printf("Что-то не так с памятью или файлом...");
		return 1;
	}
	rewind(F);
	if (!(Fnew = fopen("Cjatie.bin", "wb")))
	{
		printf("\nНе удалось открыть ,создать файл\n");
		return 1;
	}
	
	fordecode(Fnew,root,*root);
	fprintf(Fnew, "%s", "!#\n");
	fprintf(Fnew, "%d\n", fr->frequence);
	
	while (1)
	{

		estr = fgets(s, sizeof(s), F);
		

		if (estr == NULL)
		{

			if (feof(F) != 0)
			{


				break;
			}
			else
			{

				printf("\nОшибка чтения из файла\n");
				return 1;
			}
		}
		for (i = 0; i < N_CHAR && s[i] != '\0'; i++)
		{
			SeekM(*root, s[i], &s1);
			for (g = 0; (g < N_CHAR) && (s1[g] != '\0') ; g++)
			{
				if (s1[g]=='1')
					c = c |( 1 << (7 - count));
				else c = c | (0 << (7 - count));
				count++;

				if (count == 8)
				{
					count = 0;
					fwrite(&c, sizeof(unsigned char), 1, Fnew);
					c = 0;
				}
			}
			memset(s1, '\0', sizeof(s1));
		}

	}
	fwrite(&c, sizeof(unsigned char), 1, Fnew);
	
	fclose(Fnew);

}
int fordecode(FILE *F , Node **tree,Node *tr) {
	char   s1[N_CHAR] = "\0";
	if (!F || !tree || !tr)
		return 1;
	if (tr->left == NULL && tr->right == NULL)
	{
		
		s1[0] = tr->letter;
		SeekM(*tree, tr->letter, s1);
		strcat(s1, "\n");
		fprintf(F, "%s", s1);

	}
	else
	{
		fordecode(F,tree,tr->left);
		fordecode(F,tree,tr->right);
	}

}
int SeekM(Node* tree, char c,char *s1) {
	
	int i;
	if (!tree)
		return 1;
	if (tree->left == NULL && tree->right == NULL) {
		if ((tree->letter == c))
		{

			return 1;
		}
		else
		{
			

			return 0;
		}
	}
	else {
		strcat(s1 ,"0");
		if (SeekM(tree->left,c,s1) == 1)
			return 1;
		else {
			for (i = 0; s1[i] != '\0'; i++);
			if (i!=0)
				s1[i - 1] = '\0';

		}
		strcat(s1, "1");
		if (SeekM(tree->right,c,s1) == 1)
			return 1;
		else {

			for (i = 0; s1[i] != '\0'; i++);
			if (i != 0)
				s1[i - 1] = '\0';

		}
		return 0;
	}
		
}
void freeList(Spisok **first)
{
	Spisok *h;
	while (*first != NULL)
	{

		h = *first;
		*first = (*first)->next;
		free(h);
	}
	return;
}
void destrodyTree(Node *nod) {
	if (nod != NULL) {
		destrodyTree(nod->left);
		destrodyTree(nod->right);
		free(nod);
	}
}
