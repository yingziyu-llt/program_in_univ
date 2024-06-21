#include <algorithm>
#include <cstdio>
#include <queue>
using namespace std;

queue<pair<char, int>> q[20];

int main()
{
	int n;
	scanf("%d\n", &n);
	for (int i = 1; i <= n; i++) {
		char c;
		int t;
		scanf("%c%d ", &c, &t);
		q[t].push(make_pair(c, t));
	}
	for (int i = 1; i <= 9; i++) {
		printf("Queue%d:", i);
		while (!q[i].empty())
			printf("%c%d ", q[i].front().first, q[i].front().second),
				q[q[i].front().first - 'A' + 10].push(q[i].front()),
				q[i].pop();
		puts("");
	}
	for (int i = 0; i < 4; i++) {
		printf("Queue%c:", 'A' + i);
		while (!q[i + 10].empty())
			printf("%c%d ", q[i + 10].front().first, q[i + 10].front().second),
				q[19].push(q[i + 10].front()),
				q[i + 10].pop();
		puts("");
	}
	while (!q[19].empty()) {
		printf("%c%d ", q[19].front().first, q[19].front().second);
		q[19].pop();
	}
}
