/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <cstdio>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

struct Node
{
	Node *nxt,*pre;
	int data;
};
Node *head = nullptr,*tail = nullptr;
void insert(int data,Node *pos)
{
	Node *xx = new Node;
	pos->nxt->pre = xx;
	xx->nxt = pos->nxt;
	xx->pre = pos;
	pos->nxt = xx;
	xx->data = data;
}

void remove(Node *pos)
{
	pos->nxt->pre = pos->pre;
	pos->pre->nxt = pos->nxt;
	delete pos;
}

char input[1000];

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	getchar();
	head = new Node;
	head->data = -1000000007;//-INF
	tail = new Node;
	tail->data = 1000000007;//INF
	tail->pre = head,head->nxt = tail;
	tail->nxt = head->pre = nullptr;
	while(T--)
	{
		fgets(input,1000,stdin);
		for(int i = 0;i < (int)strlen(input);i++)
		{
			int data;
			sscanf(input+i,"%d",&data);
			Node *cur = head;
			while(cur != nullptr && cur->nxt != nullptr)
			{
				if(cur->nxt->data > data)
					break;
				cur = cur->nxt;
			}
			insert(data,cur);
			if(data == 0)
			{
				i++;continue;
			}
			if(data < 0)
				i++,data = -data;
			while(data > 0)
				i++,data /= 10;
		}
		printf("%d",head->nxt->data);
		Node *j = head->nxt;
		Node *nxt = j->nxt;
		remove(j);
		j = nxt;
		for(;j->nxt!=nullptr;)
		{
			printf(" %d",j->data);
			nxt = j->nxt;
			remove(j);
			j = nxt;
		}
		puts("");
		
	}
}

