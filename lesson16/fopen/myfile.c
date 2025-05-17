#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fpw = fopen("zrctest.txt", "w");
    if (fpw == NULL)
    {
        perror("fopen");
        return 1;
    }
    char *strw = "linux so easy!";
    fwrite(strw, sizeof(char), strlen(strw), fpw);
    fclose(fpw);
    FILE *fpr = fopen("zrctest.txt", "r");
    if (fpr == NULL)
    {
        perror("fopen");
        return 1;
    }
    char strr[20];
    size_t ret=fread(strr, sizeof(char), sizeof(strr)/sizeof(char), fpr);
    printf("strr len:%lu %s\n",ret,strr);
    fclose(fpr);

    return 0;
}
