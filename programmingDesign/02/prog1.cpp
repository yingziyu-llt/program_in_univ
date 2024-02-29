#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class Student {
	int age, id;
	int average[5];
	string name;
	double result;

	int toNumber(const char* s)
	{
		int num = 0;
		int len = strlen(s);
		while (s[len - 1] > '9' || s[len - 1] < '0')
			len--;
		for (int i = 0; i < len; i++)
			num = num * 10 + s[i] - '0';
		return num;
	}

public:
	void input()
	{
		char s[1000];
		fgets(s, 1000, stdin);
		char split[] = ",";
		char* p = strtok(s, split);
		name.append(p);
		p = strtok(NULL, split);
		age = toNumber(p);
		p = strtok(NULL, split);
		id = toNumber(p);
		p = strtok(NULL, split);
		average[1] = toNumber(p);
		p = strtok(NULL, split);
		average[2] = toNumber(p);
		p = strtok(NULL, split);
		average[3] = toNumber(p);
		p = strtok(NULL, split);
		average[4] = toNumber(p);
	}
	void calculate()
	{
		result = (double)(average[1] + average[2] + average[3] + average[4]) / 4.0;
	}
	void output()
	{
		printf("%s,%d,%d,%g", name.c_str(), age, id, result);
	}
};

int main()
{
	Student student;	 // 定义类的对象
	student.input();	 // 输入数据
	student.calculate(); // 计算平均成绩
	student.output();	 // 输出数据
}
