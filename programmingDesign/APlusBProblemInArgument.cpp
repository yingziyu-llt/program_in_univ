/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


int main(int argc, char** argv)
{
	if(argc != 3) {
		cerr << "Argument number must be 2";
		return 1;
	}
	long long a = 0;
	sscanf(argv[1], "%lld", &a);
	long long b = 0;
	sscanf(argv[2], "%lld", &b);
	printf("%lld",a + b);
	return 0;

}
