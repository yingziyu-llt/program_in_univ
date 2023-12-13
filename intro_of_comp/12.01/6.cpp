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

char s[60];
int a[100];

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		memset(s,0,sizeof(s));
		int a1,b1;
		scanf("%d,",&a1);
		scanf("%s",s);
		{
			int i = 0;
			while(s[i] != ',')
				i++;
			s[i] = '\0';
			sscanf(s + i + 1,"%d",&b1);
		}
		int n = strlen(s);
		int up = 0;
		for(int i = 0;i < n;i++)
		{
			a[i] = a[i] * a1 + up;
			up = a[i] / a1;
		}
	}
}

