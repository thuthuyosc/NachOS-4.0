#include "syscall.h"

#define LENGTH 256

int main()
{
    int id;
    char *buffer;
    char *fileName;

    PrintString("Nhap file name de doc (vi du: Hello.txt): ");
    ReadString(fileName, LENGTH - 1);

    id = Open(fileName, 0);
    if (id != -1)
    {
        Read(buffer, LENGTH - 1, id);
        PrintString(buffer);
        PrintChar('\n');
        Close(id);
    }
    Halt();
    /* not reached */
}
