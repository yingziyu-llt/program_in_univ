#include<stdio.h>
void draw(int a,int b,char c,int d);
int main(){
	int a,b,d;
	char c;
	scanf("%d %d %c %d",&a,&b,&c,&d);
	while(a!=0){
		draw(a,b,c,d);
		int e,f,h;
		char g;
		scanf("%d %d %c %d",&e,&f,&g,&h);
		a=e;
		b=f;
		c=g;
		d=h;
	}
	
	return 0;
}
void draw(int a,int b,char c,int d){
	int height = a,width = b;
	char symble = c;
	int iscontent = d;
	if(iscontent == 1){
		int i=1,j=1;
		for(i=1;i<=height;i++){
			for(j=1;j<=width;j++){
				printf("%c",symble);
			}printf("\n");
		}
	}else{
		int i=1,j=1;
		for(i=1;i<=height;i++){
			for(j=1;j<=width;j++){
				if(i==1||i==height){
					printf("%c",symble);
				}else if(j==1||j==width){
					printf("%c",symble);
				}else
				printf(" ");
			}printf("\n");
		}
	}
}
