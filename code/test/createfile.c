#include "syscall.h"

int main()
{
    int id = Create("Hello.txt");
    if (!id)
    {
        id = Open("Hello.txt", 1);
        if (id != -1)
        {
            Write("Hello Word", 10, id);
            Close(id);
        }
    }

    Halt();
    /* not reached */
}
