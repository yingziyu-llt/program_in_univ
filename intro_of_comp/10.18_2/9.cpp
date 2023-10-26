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

class Point
{
	public:
		double x,y,z;
		Point(double a,double b,double c)
		{
			x = a,y = b,z = c;
		}
		Point()
		{
			x = 0,y = 0,z = 0;
		}
		double operator- (Point a)
		{
			return sqrt((a.x - x) * (a.x - x) + (a.y - y) * (a.y - y) + (a.z - z) * (a.z - z));
		}
};

class Edge
{
	public:
		double dis;
		Point a,b;
		Edge(Point x,Point y)
		{
			a = x;
			b = y;
			dis = x - y;
		}
		Edge(){}
		bool operator< (Edge a)
		{
			return dis < a.dis;
		}
};


Edge e[110];
Point p[20];

void bbsort(int n)
{
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n - 1;j++)
			if(e[j] < e[j + 1]) swap(e[j],e[j + 1]);
}

int main()
{
#ifdef llt
	freopen("9.in","r",stdin);
	freopen("9.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		double a,b,c;
		scanf("%lf%lf%lf",&a,&b,&c);
		p[i] = Point(a,b,c);
	}
	int ecnt = 0;
	for(int i = 1;i <= n;i++)
		for(int j = i + 1;j <= n;j++)
			e[++ecnt] = Edge(p[i],p[j]);
	bbsort(ecnt);
	for(int i = 1;i <= ecnt;i++)
		printf("(%.0lf,%.0lf,%.0lf)-(%.0lf,%.0lf,%.0lf)=%.2lf\n"
				,e[i].a.x,e[i].a.y,e[i].a.z,e[i].b.x,e[i].b.y,e[i].b.z,e[i].dis);
}

