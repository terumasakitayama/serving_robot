#include <stdio.h>
#include <stdbool.h>
#include <math.h>

void main(){
    int a;
    bool b = false;
    printf("判定する整数を入力してください。\n");
    scanf("%d", &a);
    for(int i=2; i<=sqrt(a); i++){
        if(a % i == 0){
            b = true;
        }
    }
    if(b){
        printf("%d は素数じゃないです。\n", a);
    }else{
        printf("%d は素数です。\n", a);
    }
}