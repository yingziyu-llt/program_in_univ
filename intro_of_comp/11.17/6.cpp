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
#include<string>
#include<map>
#include<iostream>
using namespace std;

map <string,int> cnt;

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	char s[1000];
	scanf("%s",s);
	for(int i = 0;i < (int)strlen(s) - n + 1;i++)
	{
		string hash = "";
		for(int j = 0;j < n;j++)
			hash.push_back(s[i + j]);
		cnt[hash]++;
	}
	int maxn = 0;
	for(auto i : cnt)
		maxn = max(maxn,i.second);
	if(maxn == 1)
	{
		printf("NO");
		return 0;
	}
	printf("%d\n",maxn);
	for(int i = 0;i < (int)strlen(s) - n + 1;i++)
	{
		string hash = "";
		for(int j = 0;j < n;j++)
			hash.push_back(s[i + j]);
		if(cnt[hash] == maxn)
			cnt[hash] = 0,cout << hash << endl;
	}
}

