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
	freopen("15.in","r",stdin);
	freopen("15.out","w",stdout);
#endif
	int len;
	char s[10000];
	fgets(s,10000,stdin);
	char s1[1000];
	char s2[1000];
	scanf("%s",s1);
	scanf("%s",s2);
	int len1 = strlen(s1);
	len = strlen(s);
	for(int i = 0;i < len;i++)
	{
		bool flag = 0;
		if(s[i] == ' ' || i == 0)
		{
			if(i != 0)
				putchar(' '),i++;
			flag = 1;
			for(int j = 0;j < len1;j++)
				if(s[i + j] != s1[j])
				{
					flag = false;
					break;
				}
		}
		if(flag && (s[i + len1] == ' ' || i + len1 >= len - 1))
			printf("%s",s2),i += len1 - 1;
		else
			putchar(s[i]);
	}
}

