#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
    // printf(1, "My first xv6 program\n");
    for (int i = 0; i < 10; i++)
    {
        int id = fork();
        if (id < 0)
            printf(1, "Error forking\n");
        else if (id > 0)
        {
            int a, b;
            waitx(&a, &b);
            printf(1, "The process waited for %d and ran for %d\n", a, b);
        }
        else
        {
            for (volatile int k = 0; k < 10000000; k++)
                k = k + 1 - 2 + 2;
            break;
        }
    }
    exit();
}