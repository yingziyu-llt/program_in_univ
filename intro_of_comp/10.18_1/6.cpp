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
#include<iostream>
using namespace std;

char s1[100],s2[100];

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	cin.getline(s1 + 1,90);
	cin.getline(s2 + 1,90);
	for(int i = 1;i <= strlen(s1 + 1);i++)
	{
		if(s1[i] == '\r' || s1[i] == '\n')
			s1[i] = 0;
		if(s1[i] >= 'A' && s1[i] <= 'Z')
			s1[i] = s1[i] - 'A' + 'a';
	}
	for(int i = 1;i <= strlen(s2 + 1);i++)
		if(s2[i] >= 'A' && s2[i] <= 'Z')
			s2[i] = s2[i] - 'A' + 'a';
	if(strcmp(s1 + 1,s2 + 1) > 0)
		printf(">");
	else if(strcmp(s1 + 1,s2 + 1) == 0)
		printf("=");
	else
		printf("<");
	return 0;
}

