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
#include<set>
using namespace std;

set <long long> s;

int main()
{
#ifdef llt
	freopen("9.in","r",stdin);
	freopen("9.out","w",stdout);
#endif
	for(int i = 0;i <= 32;i++)
		s.insert(1ll << i);
	int t;
	while(~scanf("%d",&t))
	{
		if(s.find(t + 1) != s.end())
			printf("YES");
		else
			printf("NO");
		puts("");
	}
}

