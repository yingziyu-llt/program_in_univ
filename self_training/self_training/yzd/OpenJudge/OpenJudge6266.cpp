#include<stdio.h>
#include<algorithm>
using namespace std; 
int pp=1;
bool dfs(int a,int b)
{
	if(a%b==0||a/b>=2) return true;
	else               return !dfs(b,a%b);
}
int main()
{
	while(1)
	{
		int a,b;
		pp=1;
		scanf("%d%d",&a,&b);
		if(a<b) swap(a,b);
		if(a==0&&b==0) 
			break;
		if(dfs(a,b)) printf("win\n");
		else         printf("lose\n");
	}
	return 0;
}
