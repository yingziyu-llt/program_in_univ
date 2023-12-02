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

char s1[1000],s2[1000];

int cmp()
{
	if(strlen(s1) > strlen(s2))
		return -1;
	if(strlen(s1) < strlen(s2))
		return 1;
	int len = strlen(s1);
	for(int i = 0;i < len;i++)
	{
		if(s1[i] > s2[i]) return -1;
		else if(s1[i] < s2[i]) return 1;
	}
	return 0;
}

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	scanf("%s\n%s",s1,s2);
	int res = cmp();
	if(res == -1)
		printf("RED");
	else if(res == 0)
		printf("EQUAL");
	else
		printf("BLUE");
	return 0;
}

