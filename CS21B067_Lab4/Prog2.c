#include <stdio.h>

int main(void)
{
    int *data = (int *)(malloc(100*sizeof(int)));
    if(data==NULL)
    {
        printf("Malloc failed\n");
        return 1;
    }
    free(data);
    printf("%d\n", data[20]);
    return 0;
}