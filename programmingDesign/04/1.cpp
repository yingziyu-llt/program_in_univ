#include <cstring>
#include <iostream>
#include <string>
using namespace std;
class MyString {
	char* p;

public:
	MyString(const char* s)
	{
		if (s) {
			p = new char[strlen(s) + 1];
			strcpy(p, s);
		} else
			p = NULL;
	}
	~MyString()
	{
		if (p)
			delete[] p;
	}
	void Copy(const char* s)
	{
		p = new char[strlen(s) + 1];
		strcpy(p, s);
	}
	MyString(MyString& str)
	{
		p = new char[strlen(str.p) + 1];
		strcpy(p, str.p);
	}
	MyString& operator=(MyString str)
	{
		if (str.p) {
			delete[] p;
		}
		p = new char[strlen(str.p) + 1];
		strcpy(p, str.p);
		return *this;
	}
	MyString& operator=(const char* str)
	{
		if (p != NULL)
			delete[] p;
		p = new char[strlen(str) + 1];
		strcpy(p, str);
		return *this;
	}
	operator char*()
	{
		return p;
	}
};
int main()
{
	char w1[200], w2[100];
	while (cin >> w1 >> w2) {
		MyString s1(w1), s2 = s1;
		MyString s3(NULL);
		s3.Copy(w1);
		cout << s1 << "," << s2 << "," << s3 << endl;

		s2 = w2;
		s3 = s2;
		s1 = s3;
		cout << s1 << "," << s2 << "," << s3 << endl;
	}
}
