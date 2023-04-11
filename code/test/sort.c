/* sort.c
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

/*
#define UNIX
#define UNIX_DEBUG
*/

#include "syscall.h"

#define SIZE (100)

/* size of physical memory; with code, we'll run out of space!*/

int sortASC(int a, int b)
{
    if (a > b)
    {
        return 1;
    }
    return 0;
}

int sortDESC(int a, int b)
{
    if (a < b)
    {
        return 1;
    }
    return 0;
}

void BubbleSort(int arr[], int n, int (*sortOrder)(int, int))
{
    int i = 0;
    int j = 0;
    for (i = n - 1; i > 0; i--)
    {
        for (j = 0; j < i; j++)
        {
            if (sortOrder(arr[j], arr[j + 1]))
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main()
{
    int arr[SIZE];
    int n = 0;
    int i = 0;
    int j = 0;
    int choice = 0;
    do
    {
        PrintString("Enter n:\n");
        n = ReadNum();
    } while (n <= 0 || n > SIZE);

    PrintString("Enter array: ");
    for (i = 0; i < n; i++)
    {
        arr[i] = ReadNum();
    }

    do
    {
        PrintString("\n0. Increasingly\n1. Decreasingly\nEnter your choice:");
        choice = ReadNum();
    } while (choice != 0 && choice != 1);

    if (!choice)
    {
        BubbleSort(arr, n, sortASC);
    }
    else
    {
        BubbleSort(arr, n, sortDESC);
    }

    for (i = 0; i < n; i++)
    {
        PrintNum(arr[i]);
        PrintChar(' ');
    }

    PrintChar('\n');
    Halt();
}
