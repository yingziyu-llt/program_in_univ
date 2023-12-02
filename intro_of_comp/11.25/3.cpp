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


int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int m;
	scanf("%d",&m);
	char s[1000];
	scanf("%s",s);
	int len = strlen(s);
	int ans = 0;
	for(int i = 0;i < len;i++)
	{
		if(s[i] >= '0' && s[i] <= '9')
			ans = ans * m + s[i] - '0';
		else
			ans = ans * m + s[i] - 'A' + 10;
	}
	printf("%d",ans);
}

