#include<stdio.h>
#include<math.h>

double a[5];
#define EPS 1e-6

int isZero(double x) {
  return fabs(x) <= EPS;
}

int count24(double a[], int n){
  //用数组a里的n个数，计算24
  double b[5];
  int i, j, m, k;
  if(n == 1){
    if(isZero(a[0] - 24))
      return 1;
    else
      return 0;
  }
  for(i=0; i < n-1; ++i)
    for(j=i+1; j < n; ++j){
      //枚举两个数的组合
      m = 0;//还剩下m个数，m = n - 2
      for(k=0; k<n; ++k)
        if( k != i && k != j )
          b[m++] = a[k];//把其余的数放入b
      b[m] = a[i] + a[j];
      if( count24(b, m+1) )
        return 1;
      b[m] = a[i] - a[j];
      if( count24(b, m+1) )
        return 1;
      b[m] = a[j] - a[i];
      if( count24(b, m+1) )
        return 1;
      b[m] = a[i] * a[j];
      if( count24(b, m+1) )
        return 1;
      if( !isZero(a[j]) ){
        b[m] = a[i]/a[j];
        if(count24(b, m+1))
          return 1;
      }
      if( !isZero(a[i]) ){
        b[m] = a[j]/a[i];
        if( count24(b, m+1) )
          return 1;
       }
   }
   return 0;
}

int main(){
  int i;
  while(1) {
    for(i=0; i < 4; ++i)
       scanf("%lf",&a[i]);
    if( isZero(a[0]) )
       break;
    if( count24(a, 4) )
       printf("YES\n");
    else
       printf("NO\n");
  }
  return 0;
}

