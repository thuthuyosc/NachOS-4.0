// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available

#define MAX_PROCESS 20

class FileSystem
{
public:
	OpenFile **openFiles;
	int type[MAX_PROCESS];
	int idx;
	char **fileNames;

	FileSystem()
	{
		openFiles = new OpenFile *[MAX_PROCESS];
		fileNames = new char *[MAX_PROCESS];
		for (int i = 0; i < MAX_PROCESS; i++)
		{
			openFiles[i] = NULL;
			fileNames[i] = NULL;
			type[i] = 0;
		}
		idx = 0;
		this->Create("stdin");
		this->Create("stdout");
		type[1] = 1;

		openFiles[idx++] = Open("stdin");
		openFiles[idx++] = Open("stdout");
	}
	~FileSystem()
	{
		for (int i = 0; i < MAX_PROCESS; i++)
		{
			if (openFiles[i])
			{
				delete openFiles[i];
			}
			if (fileNames[i])
			{
				delete[] fileNames[i];
			}
		}
		delete[] openFiles;
		delete[] fileNames;
	}

	bool Create(char *name)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor);
	}

	OpenFile *OpenReadOnly(char *name)
	{
		int fileDescriptor = OpenForRead(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor);
	}

	int findFreeSlot()
	{
		for (int i = 2; i < MAX_PROCESS; i++)
			if (openFiles[i] == NULL)
				return i;
		return -1;
	};

	bool isOpen(char *name)
	{
		for (int i = 2; i < MAX_PROCESS; i++)
			if (fileNames[i] != NULL)
				if (strcmp(name, fileNames[i]) == 0)
				{
					return true;
				}

		return false;
	}

	bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem
{
public:
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)

	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
