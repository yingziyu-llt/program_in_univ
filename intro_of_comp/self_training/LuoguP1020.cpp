#include<algorithm>
#include<stdio.h>
#include<string.h>
#define INF 0x3f
using namespace std;
const int N = 110000;
int g[N],a[N],f[N];
int cmp(int a,int b,int k)
{
	if(k == 1) return a >= b;
	return a < b;
}
int search(int l,int r,int val,int k)
{
	while (l < r)
	{
		int mid = (l + r) >> 1; 
		if(cmp(g[mid],val,k)) l = mid + 1;
		else r = mid;
	}
	return l;
}
int main()
{
	int n = 1;
	while(scanf("%d",&a[n]) != EOF)
		n++;
	n--;
	memset(g,-0x7f,sizeof(g));
	int ans = -1;
	for(int i = 1;i <= n;i++)
	{
		f[i] = search(1,i,a[i],1);
		g[f[i]] = max(g[f[i]],a[i]);
		if(f[i] > ans) ans = f[i];
	}
	printf("%d\n",ans);
	memset(g,0x7f,sizeof(g));
	ans = -1;
	for(int i = 1;i <= n;i++)
	{
		f[i] = search(1,i,a[i],2);
		g[f[i]] = min(g[f[i]],a[i]);
		if(f[i] > ans) ans = f[i];
	}
	printf("%d",ans);
	return 0;
}
