#include <stdio.h>
int a;
int func(int x){
    x = x + 1;
    a = 7;
    {
        int x;
    }
    return x;
}
int main(){
    int y = func(a);
    printf("%d %d\n", a, func(func(a)));
    return 0;
}






































