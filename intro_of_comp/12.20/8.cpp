#include <iostream>
using namespace std;
int main() {
    int a[10], i[10];
    for (int j = 1; j <= 9; j++) {
        cin >> a[j];
    }

    for (i[1] = 0; i[1] <= 4; i[1]++) //遍历每一种情况
        for (i[2] = 0; i[2] <= 4; i[2]++)
            for (i[3] = 0; i[3] <= 4; i[3]++)
                for (i[4] = 0; i[4] <= 4; i[4]++)
                    for (i[5] = 0; i[5] <= 4; i[5]++)
                        for (i[6] = 0; i[6] <= 4; i[6]++)
                            for (i[7] = 0; i[7] <= 4; i[7]++)
                                for (i[8] = 0; i[8] <= 4; i[8]++)
                                    for (i[9] = 0; i[9] <= 4; i[9]++) {
                                        int sum = 0;
                                        sum += (a[1] + i[1] + i[2] + i[4]) % 4; //受到影响后的情况，如果为0则成功
                                        sum += (a[2] + i[1] + i[2] + i[3] + i[5]) %  4;
                                        sum += (a[3] + i[2] + i[3] + i[6]) % 4;
                                        sum += (a[4] + i[1] + i[4] + i[5] + i[7]) % 4;
                                        sum += (a[5] + i[1] + i[3] + i[5] + i[7] + i[9]) % 4;
                                        sum += (a[6] + i[3] + i[5] + i[6] + i[9]) % 4;
                                        sum += (a[7] + i[4] + i[7] + i[8]) % 4;
                                        sum +=  (a[8] + i[5] + i[7] + i[8] + i[9]) %  4;
                                        sum += (a[9] + i[6] + i[8] + i[9]) % 4;
                                        if (sum == 0) {
											bool flag = 0;
                                            for (int j = 1; j <= 9; j++)
                                                while (i[j]--)
												{
													if(flag) cout << ' ';
													flag = 1;
                                                    cout << j;//可能有多次
												}
                                            return 0;
                                        }
									}
}
