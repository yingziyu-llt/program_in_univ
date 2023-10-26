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

char s[100010];

int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	scanf("%s",s + 1);
	for(int i = 1;i <= strlen(s + 1) / 2;i++)
		if(s[i] != s[strlen(s + 1) - i + 1])
		{
			printf("no");
			return 0;
		}
	printf("yes");
}

