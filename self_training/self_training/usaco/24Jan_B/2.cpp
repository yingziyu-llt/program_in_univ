/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int type[100020],val[100010];
int mem[100020];

int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,pos,dir = 1;
	scanf("%d%d",&n,&pos);
	for(int i = 1;i <= n;i++)
		scanf("%d%d",&type[i],&val[i]);
	int ans = 0,power = 1;;
	while(1)
	{
		if(pos <= 0 || pos > n)
			break;
		if(mem[pos] == power)
			break;
		if(type[pos] == 0)
		{
			dir = -dir,power += val[pos];
			if(dir == -1)
				mem[pos] = power - val[pos];
		}
		else if(power >= val[pos])
			ans++,val[pos] = 0x7fffffff;
		pos += dir * power;
	}
	printf("%d",ans);
}
