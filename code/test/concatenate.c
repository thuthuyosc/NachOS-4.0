#include "syscall.h"

#define MAX_LENGTH 256

int main()
{
    int id;
    int size;
    char src1[MAX_LENGTH];
    char src2[MAX_LENGTH];
    char data[MAX_LENGTH];

    PrintString("Nhap ten file nguon 1: ");
    ReadString(src1, MAX_LENGTH - 1);

    PrintString("Nhap ten file nguon 2: ");
    ReadString(src2, MAX_LENGTH - 1);

    id = Open(src2, 0);
    if (id != -1)
    {
        size = Read(data, MAX_LENGTH - 1, id);
        Close(id);
    }

    id = Open(src1, 1);
    if (id != -1)
    {
        Seek(-1, id);
        Write(data, size, id);
        Close(id);
    }

    Halt();
    /* not reached */
}