#include <stdio.h>
int f1(int a[], int b[][10])
{
    return a[0] * b[0][0];
}
int f2(int a[][10], int b, int c[][10])
{
    return (a[0][0] + c[1][0]) * b;
}
int main(){
    printf("20376158\n");
    //常量定义
    const int a[10] = {1};
    const int b[10][10] = {{1}, {2}};
    const int c[10] = {};
    int x[10] = {1};
    int y[10][10] = {{10}, {20}};
    int z = 100;
    int m[10] = {};
    //左值表达式
    x[1] = 10;
    printf("%d\n", x[1]);
    y[0][1] = 10;
    printf("%d\n", y[0][1]);
    //函数实参
    int xx = f1(x, y);
    int yy = f2(y, z, y);
    int zz = f1(y[1], y);	//部分数组传参
    printf("%d %d %d\n", xx, yy, zz);
    printf("1\n");printf("1\n");printf("1\n");printf("1\n");printf("1\n");printf("1\n");
    return 0;
}