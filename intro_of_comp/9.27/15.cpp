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

int a[2][13] = {{0,31,28,31,30,31,30,31,31,30,31,30,31},{0,31,29,31,30,31,30,31,31,30,31,30,31}};
char s[7][100] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" , "Saturday"};
int gety(int &t)
{
	int y400 = t / (400 * 365 + 100 - 3);
	t %= (400 * 365 + 97);
	if(t == 0)
		y400--,t += 400 * 365 + 97;
	int y100 = t / (100 * 365 + 24);
	t %= 100 * 365 + 24;
	if(t == 0)
		y100--,t += 100 * 365 + 24;
	int y4 = t / (4 * 365 + 1);
	t %= 4 * 365 + 1;
	if(t == 0)
		y4--,t += 4 * 365 + 1;
	int y = 0;
	if(t > 366) 
	{
		y = 1 ;
		t -= 366;
		y += t / 365;
		t %= 365;
		if(t == 0)
			t += 365,y -= 1;
	}
	else y = 0;
	return 400 * y400 + 100 * y100 + y4 * 4 + y;
}

int getm(int &t,int type)
{
	int m = 1;
	for(m = 1;m <= 12;m++)
	{
		if(t <= a[type][m])
			break;
		t -= a[type][m];
	}
	return m;
}
int main()
{
#ifdef llt
	freopen("15.in","r",stdin);
	freopen("15.out","w",stdout);
#endif
	while(1)
	{
		int t;
		int t_cp;
		scanf("%d",&t);
		t_cp = t;
		if(t == -1) break;
		t++;
		int y = gety(t);
		int m = getm(t,y % 4 == 0);
		int d = t;
		printf("%d-%02d-%02d %s\n",y + 2000,m,d,s[(6 + t_cp) % 7]);
	}

}

