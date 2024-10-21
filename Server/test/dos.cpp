#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    for (int ind = 0; ind < 8; ind++) fork();

    system("./client < cmds.txt");

    return 0;
}