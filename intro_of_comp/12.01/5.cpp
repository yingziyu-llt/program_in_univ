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

char s[1100];
int res[1100];

int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int m;
	scanf("%s%d",s,&m);
	int n = strlen(s);
	int ret = 0;
	for(int i = 0;i < n;i++)
	{
		ret = ret * 10 + s[i] - '0';
		res[i] = ret / m;
		ret %= m;
	}
	int from = 0;
	while(res[from] == 0)
		from++;
	for(int i = from;i < n;i++)
		printf("%d",res[i]);
	printf(" %d",ret);
}

