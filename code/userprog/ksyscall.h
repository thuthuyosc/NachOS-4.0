/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "utility.h"
#include "stdio.h"
#include "synch.h"
#include "errno.h"

// so chu so toi da cua kieu int (bao gom ki tu '\0' ket thuc)
#define MAX_NUM_LENGTH 11
#define LF ((char)10)
#define CR ((char)13)
#define TAB ((char)9)
#define SPACE ((char)' ')
const int INT32_MIN = -2147483648;
const int INT32_MAX = 2147483648;

#define MAX_LENGTH 1000
#define MAX_FILE_LENGTH 256
#define MAX_FILE 20

// Doi thanh ghi Program counter cua he thong ve sau 4 byte de tiep tuc nap lenh
void IncreasePC()
{
  /*set previous programm counter(debugging only) */
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

bool isBlank(char c) { return c == LF || c == CR || c == TAB || c == SPACE; }

/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char *User2System(int virtAddr, int limit)
{
  int i; // index
  int oneChar;
  char *kernelBuf = NULL;

  kernelBuf = new char[limit + 1]; // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;
  // memset(kernelBuf,0,limit+1);

  // printf("\n Filename u2s:");
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0)
    {
      break;
    }
  }
  return kernelBuf;
}

/*
Input: - User space address (int)
 - Limit of buffer (int)
 - Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/
int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

/**********************************************************************************************/

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysReadNum()
{
  char num_string[MAX_NUM_LENGTH] = {0};
  long long ret = 0;
  int i = 0;
  char c = 0;

  while (i < MAX_NUM_LENGTH)
  {
    c = kernel->synchConsoleIn->GetChar();
    if (c >= '0' && c <= '9')
    {
      num_string[i] = c;
      i++;
    }

    else if (i == 0 && c == '-')
    {
      num_string[i] = c;
      i++;
    }
    else if (isBlank(c))
    {
      break;
    }
  }

  if (strcmp(num_string, "-2147483648") == 0)
  {
    return INT32_MIN;
  }
  else if (strcmp(num_string, "2147483648") == 0)
  {
    return INT32_MAX;
  }

  i = (num_string[0] == '-') ? 1 : 0;
  while (i < MAX_NUM_LENGTH && num_string[i] >= '0' && num_string[i] <= '9')
  {
    ret = ret * 10 + (num_string[i++] - '0');
  }

  ret = (num_string[0] == '-') ? (-ret) : ret;
  return ret;
}

int SysPrintNum(int n)
{
  char num_string[MAX_NUM_LENGTH] = {0};
  int tmp[MAX_NUM_LENGTH] = {0}, i = 0, j = 0;
  int count = 0;

  if (n == INT32_MIN)
  {
    kernel->synchConsoleOut->PutChar('-');
    for (i = 0; i < 10; ++i)
      kernel->synchConsoleOut->PutChar("2147483648"[i]);
    return 10;
  }
  else if (n == INT32_MAX)
  {
    for (i = 0; i < 10; ++i)
      kernel->synchConsoleOut->PutChar("2147483648"[i]);
    return 10;
  }

  if (n < 0)
  {
    n = -n;
    num_string[i++] = '-';
    count++;
  }

  do
  {
    tmp[j++] = n % 10;
    n /= 10;
  } while (n);

  while (j)
  {
    num_string[i++] = '0' + (char)tmp[--j];
    count++;
  }

  for (int k = 0; k < count; k++)
  {
    kernel->synchConsoleOut->PutChar(num_string[k]);
  }

  return count;
}

int SysRandomNum()
{
  srand(time(NULL));
  int randomNumber = rand();

  return randomNumber;
}

char SysReadChar()
{
  char c = kernel->synchConsoleIn->GetChar();
  return c;
}

void SysPrintChar(char c)
{
  kernel->synchConsoleOut->PutChar(c);
}

int SysReadString()
{
  int addr = kernel->machine->ReadRegister(4); // address of the buffer of user space
  int length = kernel->machine->ReadRegister(5);
  char *sysBuff = new char[length + 1];

  // The loop below reads characters input from user, forming a string.
  int idx = 0;
  while (idx < length)
  {
    char ch = kernel->synchConsoleIn->GetChar();
    if (ch == '\n' || ch == 0)
    {
      sysBuff[idx] = '\0';
      break;
    }
    sysBuff[idx] = ch;
    idx++;
  }
  sysBuff[idx] = '\0';                // The string is created.
  System2User(addr, length, sysBuff); // Copy content from kernel buffer to user buffer
  delete[] sysBuff;
  return ++idx;
}

int SysPrintString(char *input)
{
  int i;
  int memVal = 0;
  int memPt = (int)input;
  for (i = 0;; i++)
  {
    kernel->machine->ReadMem(int(memPt + i), 1, &memVal);
    if (0 == memVal)
      return i;
    kernel->synchConsoleOut->PutChar((char)memVal);
  }
  return i;
}
/*****************************************************************************/

/*
// Prototype: int CreateFile(char *name);
*/
int SysCreate() // Prototype: int CreateFile(char *name);
{
  int addr = kernel->machine->ReadRegister(4);   // Get the address of the buffer of user space
  char *sysBuff = User2System(addr, MAX_LENGTH); // copy buffer from user space to kernel space
  if (!sysBuff)
  {
    return -1;
  }

  if (strlen(sysBuff) == 0)
  {
    return -1;
  }

  bool code = kernel->fileSystem->Create(sysBuff); // Create file
  return code ? 0 : -1;                            // Return status code
}

/*
// Prototype: OpenFileID Open(char *name, int type)
*/
OpenFileId SysOpen()
{
  int addr = kernel->machine->ReadRegister(4);
  int type = kernel->machine->ReadRegister(5);
  char *name = User2System(addr, MAX_LENGTH);

  if (!name)
  {
    return -1;
  }

  if (strlen(name) == 0)
  {
    return -1;
  }

  if (kernel->fileSystem->isOpen(name) == true)
  {
    delete[] name;
    return -1;
  }

  if (strcmp(name, "stdin") == 0)
  {
    delete[] name;
    return 0;
  }
  if (strcmp(name, "stdout") == 0)
  {
    delete[] name;
    return 1;
  }

  int freeSlot = kernel->fileSystem->findFreeSlot();
  if (freeSlot != -1) // Chi xu li khi con slot trong
  {
    kernel->fileSystem->type[freeSlot] = type;
    switch (type)
    {
    case 0:
      kernel->fileSystem->openFiles[freeSlot] = kernel->fileSystem->OpenReadOnly(name);
      break;
    case 1:
      kernel->fileSystem->openFiles[freeSlot] = kernel->fileSystem->Open(name);
      break;
    default:
      delete[] name;
      cerr << "type must be 0 or 1\n";
      return -1;
    }

    if (kernel->fileSystem->openFiles[freeSlot] != NULL) // Mo file thanh cong
    {
      kernel->fileSystem->fileNames[freeSlot] = name;
      return freeSlot;
    }
  }

  delete[] name;
  return -1;
}

/*
// Prototype: int Close(OpenFileId id);
*/
int SysClose()
{
  int fileid = kernel->machine->ReadRegister(4);
  if (fileid >= 0 && fileid < MAX_FILE)
  {
    if (kernel->fileSystem->openFiles[fileid])
    {
      delete kernel->fileSystem->openFiles[fileid];
      kernel->fileSystem->openFiles[fileid] = NULL;
      delete[] kernel->fileSystem->fileNames[fileid];
      kernel->fileSystem->fileNames[fileid] = NULL;
      return 0;
    }
  }
  return -1;
}

/*
// Prototype: int Read(char *buffer, int size, OpenFileId id)
*/
int SysRead()
{
  int virtualAddr = kernel->machine->ReadRegister(4),
      charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId >= MAX_FILE || openFileId < 0)
    return -1; // File id out of table -> error
  if (!kernel->fileSystem->openFiles[openFileId])
    return -4; // Reading an unopened file -> error
  char *sysBuff = new char[charcount + 1];
  char ch;
  int idx = 0, size;
  switch (openFileId)
  {
  case 0: // Read stdin
    while (idx < charcount)
    {
      char ch = kernel->synchConsoleIn->GetChar();
      if (isBlank(ch))
      {
        sysBuff[idx] = '\0';
        break;
      }
      sysBuff[idx] = ch;
      idx++;
    }
    sysBuff[idx] = '\0';                          // The string is created.
    System2User(virtualAddr, charcount, sysBuff); // Copy content from kernel buffer to user buffer
    delete[] sysBuff;
    return idx;
  case 1:
    return -2; // Reading stdout -> error
  default:     // Read files
    size = kernel->fileSystem->openFiles[openFileId]->Read(sysBuff, charcount);
    if (size > 0)
      System2User(virtualAddr, size, sysBuff);
    else
    {
      delete[] sysBuff;
      return -3;
    } // Can't read jack shit/null file -> error
    delete[] sysBuff;
    return size;
  }
  return -1; // Dummy return point
}

/*
// Prototype: int Write(char *buffer, int size, OpenFileId id);
*/
int SysWrite()
{
  int virtualAddr = kernel->machine->ReadRegister(4),
      charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId >= MAX_FILE || openFileId < 0)
    return -1; // File ID out of the table -> error
  if (kernel->fileSystem->openFiles[openFileId] == 0)
    return -4; // Writing an unopened file -> error
  if (kernel->fileSystem->type[openFileId] == 0)
    return -2; // Writing a read-only file -> error
  char *sysBuff = User2System(virtualAddr, charcount + 1);
  char ch;
  int size;
  int i;
  switch (openFileId)
  {
  case 1: // Write stdout
    i = 0;
    while (i < MAX_LENGTH)
    {
      char c = sysBuff[i];
      if (c != '\0')
        kernel->synchConsoleOut->PutChar(c);
      else
        break;
      ++i;
    }
    // deallocate memory
    delete[] sysBuff;
    return i;
  case 0:
    return -2; // Writing in stdin -> error
  default:
    size = kernel->fileSystem->openFiles[openFileId]->Write(sysBuff, charcount);
    delete[] sysBuff;
    return size;
  }
  return -1; // Dummy return point
}

/*
// Prototype: int Remove(char* name)
*/
int SysRemove()
{
  int virtAddr = kernel->machine->ReadRegister(4);             // Lay dia chi cua tham so name tu thanh ghi so 4
  char *fileName = User2System(virtAddr, MAX_FILE_LENGTH - 1); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
  if (kernel->fileSystem->isOpen(fileName) == true)            // Check whether the file's already opened or not
  {
    cerr << "This file is opening\n";
    return -1;
  }

  bool result = kernel->fileSystem->Remove(fileName);
  if (result == true)
  {
    return 0;
  }
  return -1;
}

/*
// Prototype: int Seek(int position, OpenFileID id)
*/
int SysSeek()
{
  int pos = kernel->machine->ReadRegister(4);
  int index = kernel->machine->ReadRegister(5);
  if (index <= 1 || index >= MAX_FILE)
    return -1;
  if (kernel->fileSystem->openFiles[index] == NULL)
    return -1;
  // use seek(-1) to move to the end of file
  if (pos == -1)
    pos = kernel->fileSystem->openFiles[index]->Length();
  if (pos < 0 || pos > kernel->fileSystem->openFiles[index]->Length())
    return -1;
  kernel->fileSystem->openFiles[index]->Seek(pos);
  return pos;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
