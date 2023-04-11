// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	int result;

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;
		case SC_ReadNum:
			result = SysReadNum();
			DEBUG(dbgSys, "ReadSNum returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);
			/* Prepare Result */
			// kernel->machine->WriteRegister(2, (int)result);
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_PrintNum:
			result = SysPrintNum((int)kernel->machine->ReadRegister(4));
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_RandomNum:
			result = SysRandomNum();
			DEBUG(dbgSys, "RandomNum returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_ReadChar:
			result = SysReadChar();
			DEBUG(dbgSys, "ReadSchar returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);
			/* Prepare Result */
			// kernel->machine->WriteRegister(2, (int)result);
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_PrintChar:
			SysPrintChar((int)kernel->machine->ReadRegister(4));
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_ReadString:
			/* Process SysAdd Systemcall*/
			result = SysReadString();

			DEBUG(dbgSys, "ReadString returning with " << result << "\n");
			/* Prepare Result */
			// kernel->machine->WriteRegister(2, (int)result);
			IncreasePC(); // Tang Program Counter
			return;
			ASSERTNOTREACHED();
			break;

		case SC_PrintString:
			result = SysPrintString(/* int op1 */ (char *)kernel->machine->ReadRegister(4));
			kernel->machine->WriteRegister(2, (int)result); /* Prepare Result */
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Create:
			DEBUG(dbgSys, "Creating file " << (char *)kernel->machine->ReadRegister(4));
			result = SysCreate();
			kernel->machine->WriteRegister(2, result);
			if (result == 0)
				cerr << "Creation successful\n";
			else
				cerr << "Creation failed\n";
			DEBUG(dbgSys, "Creation completed");

			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Open:
			result = SysOpen();
			DEBUG(dbgSys, "Passed SysOpen()\n");
			DEBUG(dbgSys, "result:");
			DEBUG(dbgSys, result);
			if (result > 1)
			{
				DEBUG(dbgSys, "Creation completed. ID = " << result);
				// cerr << "Creation completed. ID = " << result;
			}
			else
			{
				DEBUG(dbgSys, "Creation failed")
			};
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Close:
			DEBUG(dbgSys, "Closing file " << (char *)kernel->machine->ReadRegister(4));
			result = SysClose();
			if (result == 0)
			{
				DEBUG(dbgSys, "Close completed.");
				// cerr << "Close completed";
			}
			else
			{
				DEBUG(dbgSys, "Close failed");
				// cerr << "Close failed";
			};
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Read:
			DEBUG(dbgSys, "Reading file @ ID = " << kernel->machine->ReadRegister(6));
			result = SysRead();
			switch (result)
			{
			case -1:
				DEBUG(dbgSys, "Invalid ID");
				cerr << "Invalid ID";
				break;
			case -2:
				DEBUG(dbgSys, "Tried to read stdout.")
				cerr << "Cannot read stdout";
				break;
			case -3:
				DEBUG(dbgSys, "Null file");
				cerr << "Attempted to read a null file or reached EOF";
				break;
			case -4:
				cerr << "File not opened";
				break;
			}
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Write:
			DEBUG(dbgSys, "Writing to file @ ID = " << kernel->machine->ReadRegister(6));
			result = SysWrite();
			switch (result)
			{
			case -1:
				DEBUG(dbgSys, "Invalid ID");
				cerr << "Invalid ID";
				break;
			case -2:
				DEBUG(dbgSys, "Tried to write a read-only file.")
				cerr << "This is a read-only file";
				break;
			case -4:
				cerr << "File not opened";
				break;
			}
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Remove:
			result = SysRemove();
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Seek:
			result = SysSeek();
			switch (result)
			{
			case -1:
				DEBUG(dbgSys, "Can not seek\n");
				break;
			case 0:
				DEBUG(dbgSys, "Seek Successfully\n");
				break;
			}
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	case NoException:
		return;
	case PageFaultException:
		cerr << "Unexpected user mode exception PageFaultException\n";
		SysHalt();
		break;
	case ReadOnlyException:
		cerr << "Unexpected user mode exception ReadOnlyException\n";
		SysHalt();
		break;
	case BusErrorException:
		cerr << "Unexpected user mode exception BusErrorException\n";
		SysHalt();
		break;
	case AddressErrorException:
		cerr << "Unexpected user mode exception  AddressErrorException\n";
		SysHalt();
		break;
	case OverflowException:
		cerr << "Unexpected user mode exception OverflowException\n";
		SysHalt();
		break;
	case IllegalInstrException:
		cerr << "Unexpected user mode exception IllegalInstrException\n";
		SysHalt();
		break;
	case NumExceptionTypes:
		cerr << "Unexpected user mode exception NumExceptionTypes\n";
		SysHalt();
		break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
