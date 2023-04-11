/* add.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main()
{

  char name[11];

  ReadString(name, 11);
  PrintString(name);

  Halt();
  /* not reached */
}
