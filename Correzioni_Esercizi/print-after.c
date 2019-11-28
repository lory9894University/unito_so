#include <stdio.h>

int main() {
    char s1 [80], s2 [80];
    int i = 0, j = 0, s_ins = 0;

    fgets(s1, sizeof(s1), stdin);
    fgets(s2, sizeof(s2), stdin);

    while(s1[i] != '\0'){
        if(s1[i] >= 32 || s1[i] <= 126){
            s1[i] = '0';
        }
    }

    i = 0;

    while(s2[i] != '\0'){
        if(s2[i] >= 32 || s2[i] <= 126){
            s2[i] = '0';
        }

        i++;
    }

    i = 0;

    while(s1[i] != '\0') {
        if(s1[i] == s2[j]){
            j=0;
            while(s2[j]!='\0' && s1[i+j]!='\0'){
                j++;
                if(s2[j] == s1[i+j])
                    s_ins=1;
                else{
                    s_ins=0;
                    break;
                }
            }
            if(s_ins == 1){
            break;
            }
        }
        i++;
    }

    printf("%s\n", s1[i-1]);

    return 0;
}
