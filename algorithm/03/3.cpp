/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string.h>
using namespace std;

int a[1000], b[1000];

int main()
{
#ifdef llt
	freopen("3.in", "r", stdin);
	freopen("3.out", "w", stdout);
#endif
	while (1) {
		int x, y;
		scanf("%d%d", &x, &y);
		if (x == 0 && y == 0)
			break;
		a[y] = x;
	}
	while (1) {
		int x, y;
		scanf("%d%d", &x, &y);
		if (x == 0 && y == 0)
			break;
		b[y] = x;
	}
	int c[1000];
	for (int i = 0; i <= 999; i++)
		c[i] = a[i] + b[i];
	int flag = 0;
	for (int i = 999; i >= 0; i--)
		if (c[i] != 0) {
			if (c[i] > 0) {
				if (flag == 1)
					printf("+");
				if (c[i] != 1)
					printf("%d", c[i]);
			} else {
				if (c[i] != -1)
					printf("%d", c[i]);
				else
					printf("-");
			}
			if (i == 1)
				printf("x");
			else if (i == 0)
				;
			else
				printf("x^%d", i);
			flag = 1;
		}
	printf("\n");
	memset(c, 0, sizeof(c));
	for (int i = 0; i <= 200; i++)
		if (a[i] != 0)
			for (int j = 0; j <= 200; j++) {
				c[i + j] += a[i] * b[j];
			}
	flag = 0;
	for (int i = 999; i >= 0; i--)
		if (c[i] != 0) {
			if (c[i] > 0) {
				if (flag == 1)
					printf("+");
				if (c[i] != 1)
					printf("%d", c[i]);
			} else {
				if (c[i] != -1)
					printf("%d", c[i]);
				else
					printf("-");
			}
			if (i == 1)
				printf("x");
			else if (i == 0)
				;
			else
				printf("x^%d", i);
			flag = 1;
		}
}
