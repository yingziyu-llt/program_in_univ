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

bool chk(int x)
{
	int ws = log10(x) + 1;
	if(ws % 2  == 1)
		return 0;
	int xx = 0;
	for(int i = 1;i <= ws / 2;i++)
		xx += x % 10,x /= 10;
	for(int i = 1;i <= ws / 2;i++)
		xx -= x % 10,x /= 10;
	return xx == 0;
}

void probA()
{
/*	int ans = 0;
	for(int i = 1;i <= 100000000;i++)
		if(chk(i))
			ans++;*/
	printf("4430091");
}
int ans[31][11];
void probB()
{
	ans[0][0] = 1;
	for(int i = 1;i <= 30;i++)
		for(int j = 0;j <= 9;j++)
			ans[i][0] += ans[i - 1][j],ans[i][j + 1] += ans[i - 1][j];
	int tot = 0;
	for(int i = 1;i <= 30;i++)
		tot += ans[i][7];
	printf("%d",ans[30][7]);
}
int main()
{
	char c;
	scanf("%c",&c);
	if(c=='A')
	{
		probA();
	}
	else
	{
		probB();
	}
}

