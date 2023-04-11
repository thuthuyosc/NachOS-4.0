#include "syscall.h"

int main()
{

  int i = 0;
  int count = 0;

  PrintString("\t\t\t\t\t\tASCII TABLE\n\n");

  PrintNum(32);
  PrintChar('\t');
  PrintString("[space]");
  PrintString("\t|\t");
  count++;

  for (i = 33; i < 127; i++)
  {
    PrintNum(i);
    PrintChar('\t');
    PrintChar((char)i);
    PrintString("\t|\t");
    count++;
    if (count == 5)
    {
      PrintChar('\n');
      count = 0;
    }
  }
  PrintChar('\n');

  Halt();
  /* not reached */
}