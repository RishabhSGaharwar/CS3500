#include <stdio.h>

int main(void)
{
    int *data = (int *)(malloc(100*sizeof(int)));
    if(data==NULL)
    {
        printf("Malloc failed\n");
        return 1;
    }
    data[100] = 0;
    printf("Last element assigned value 0\n");
    //free(data);
    return 0;
}