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
	bool label;
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
	xx->label = 0;
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
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	head = new Node;
	head->data = -1000000007;//-INF
	tail = new Node;
	tail->data = 1000000007;//INF
	tail->pre = head,head->nxt = tail;
	tail->nxt = head->pre = nullptr;
	tail->label = head->label = 1;
	insert(m,head);
	for(int i = 1;i <= n;i++)
	{
		char s[1000];
		int data;
		scanf("%s %d",s,&data);
		if(s[0] == 'A')
		{
			if(s[3] == 'T')
				insert(data,tail -> pre);
			else
				insert(data,head);
		}
		else
		{
			if(s[6] == 'T')
				remove(tail->pre);
			else
				remove(head->nxt);
		}
	}
	printf("%d",head->nxt->data);
	for(Node *cur=head->nxt->nxt;cur != tail;cur = cur->nxt)
		printf(" %d",cur->data);
}

