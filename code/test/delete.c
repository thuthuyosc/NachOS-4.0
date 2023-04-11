#include "syscall.h"

int main()
{
    if (!Remove("hello.c"))
    {
        PrintString("Xoa file thanh cong.\n");
    }
    else
    {
        PrintString("Xoa file that bai.\n");
    }
    Halt();
    /* not reached */
}