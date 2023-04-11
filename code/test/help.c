#include "syscall.h"

int main()
{
    PrintString("Cac thanh vien trong nhom:\n");
    PrintString("\tVo Duy Truong - 20120232\n");
    PrintString("\tHuynh Quoc Thai - 20120577\n");
    PrintString("\tHoang Thu Thuy - 20120382\n\n");

    PrintString("Chuong trinh ascii:\n");
    PrintString("Chuong trinh ascii in ra cac ki tu ASCII nhin thay duoc co ten la 'ascii.c'.\n\n");

    PrintString("Chuong trinh sort:\n");
    PrintString("Chuong trinh sort co ten file la 'sort.c' su dung thuat toan bubble sort\n\t1. cho phep nguoi dung nhap n so (1<=n<=100)\n");
    PrintString("\t2. Cho phep nguoi dung chon kieu sap xep (0: tang dan, 1: giam dan)\n");
    PrintString("\t3. Sau do in ra mang da duoc sap xep\n");

    PrintString("\n---------------END-----------------------\n");
    Halt();
}