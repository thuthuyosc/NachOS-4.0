#include "syscall.h"

#define MAX_LENGTH 256
#define NULL 0

int main()
{
    int id;
    int size;
    char src[MAX_LENGTH];
    char des[MAX_LENGTH];
    char data[MAX_LENGTH];

    PrintString("Nhap ten file nguon: ");
    ReadString(src, MAX_LENGTH - 1);

    PrintString("Nhap ten file dich: ");
    ReadString(des, MAX_LENGTH - 1);

    id = Open(src, 0);
    if (id != -1)
    {
        size = Read(data, MAX_LENGTH - 1, id);
        Close(id);
    }

    if (!Create(des))
    {
        id = Open(des, 1);
        if (id != -1)
        {
            Write(data, size, id);
            Close(id);
        }
    }

    Halt();
    /* not reached */
}