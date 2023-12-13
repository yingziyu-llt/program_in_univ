/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

char s[100];
bool vis[100];
char ans[100];
int len;

void nextp(int ret)
{
	if(ret == 0)
	{
		for(int i = 0;i < len;i++)
			putchar(ans[i]);
		puts("");
		return ;
	}
	for(int i = 0;i < len;i++)
		if(!vis[i])
		{
			vis[i] = true;
			ans[len - ret] = s[i];
			nextp(ret - 1);
			vis[i] = false;
		}
}

int main()
{
#ifdef llt
	freopen("19.in","r",stdin);
	freopen("19.out","w",stdout);
#endif
	scanf("%s",s);
	len = strlen(s);
	nextp(len);
}

