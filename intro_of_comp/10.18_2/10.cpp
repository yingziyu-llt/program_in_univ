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

class Student
{
	public:
		int id;
		double score;
		Student(){}
		Student(int i,double s)
		{
			id = i,score = s;
		}
		bool operator< (Student a)
		{
			return score > a.score; //reverse
		}
};
Student s[110];
int main()
{
#ifdef llt
	freopen("10.in","r",stdin);
	freopen("10.out","w",stdout);
#endif
	int n,k;
	scanf("%d%d",&n,&k);
	for(int i = 1;i <= n;i++)
	{
		int id;double score;
		scanf("%d%lf",&id,&score);
		s[i] = Student(id,score);
	}
	sort(s + 1,s + 1 + n);
	printf("%d %g",s[k].id,s[k].score);
}

