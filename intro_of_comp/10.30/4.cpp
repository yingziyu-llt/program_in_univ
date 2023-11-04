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

char s[1000];
int cnt[100];
int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	fgets(s,1000,stdin);
	for(int i = 0;i < strlen(s);i++)
		if(s[i] >= 'A' && s[i] <= 'Z')
			s[i] = s[i] - 'A' + 'a';
	for(int i = 0;i < strlen(s);i++)
		if(s[i] >= 'a' && s[i] <= 'z')
			cnt[s[i] - 'a']++;
	printf("%d %d %d %d %d",cnt[0],cnt['e' - 'a'],cnt['i' - 'a'],cnt['o' - 'a'],cnt['u' - 'a']);
}

