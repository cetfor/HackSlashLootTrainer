#include "helpers.h"

// ==========================
// Helper Functions
// ==========================

/*
* int GetProcId(char* procName)
* Get the Process ID (PID) from a process name (i.e. "game.exe")
*		procName: Pointer to the target process name string
*/
int GetProcId(wchar_t* procName)
{
	PROCESSENTRY32 pe32;	// Standard Windows structure (Google "MSDN PROCESSENTRY32" for details)
	HANDLE hSnapshot;
	BOOL procInfo = FALSE;

	// Get a "snapshot" of all processes currently running
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Error out if the snapshot fails, informing the user
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("Error: Unable to create toolhelp snapshot. Failed with error: %d\n", GetLastError());
		return TERROR_PID_LOOKUP_FAILED;
	}

	// Set the PROCESSENTRY32 size
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Reference the first process in the snapshot
	procInfo = Process32First(hSnapshot, &pe32);

	// Enumerate processes in the snapshot looking for procName
	while (procInfo)
	{
		// case-insensitive string search
		if (StrStrI(procName, pe32.szExeFile))
		{
			// If we reach this point it means we found the process
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}

		// Reference the next process in the snapshot
		procInfo = Process32Next(hSnapshot, &pe32);
	}

	// If we reach this point it means we couldn't find the process
	CloseHandle(hSnapshot);
	return TERROR_PID_LOOKUP_FAILED;
}

/*
* int GetBaseAddress(int targetPid)
* Returns the base address (RVA) for the target process
* This is safer than always assuming 0x00400000 for 32-bit procs
*		procName:	The name of the module (executable)
*		targetPid:	The process ID of the target game
*/
int GetBaseAddress(wchar_t* procName, int targetPid)
{
	MODULEENTRY32 lpModuleEntry;
	HANDLE hSnapshot;
	BOOL moduleInfo = FALSE;

	// Get a "snapshot" of the game process
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, targetPid);

	// Error out if the snapshot fails, informing the user
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("Error: Unable to create toolhelp snapshot. Failed with error: %d\n", GetLastError());
		return TERROR_GET_BASE_ADDR_FAILED;
	}

	// Set the PROCESSENTRY32 size
	lpModuleEntry.dwSize = sizeof(MODULEENTRY32);

	// Reference the first process in the snapshot
	moduleInfo = Module32First(hSnapshot, &lpModuleEntry);

	// Enumerate processes in the snapshot looking for procName
	while (moduleInfo)
	{
		// case-insensitive string search
		if (StrStrI(procName, lpModuleEntry.szModule))
		{
			// If we reach this point it means we found the process
			CloseHandle(hSnapshot);
			return (int)lpModuleEntry.modBaseAddr;
		}

		// Reference the next process in the snapshot
		moduleInfo = Module32Next(hSnapshot, &lpModuleEntry);
	}

	// If we reach this point it means we couldn't find the process
	CloseHandle(hSnapshot);
	return TERROR_GET_BASE_ADDR_FAILED;

}


/*
* void ClearConsole(void)
*	Clears the console window.
*	Code from MSDN:
*   https://msdn.microsoft.com/en-us/library/windows/desktop/ms682022(v=vs.85).aspx
*   Alternatively, you can simply call: system('cls');
*/
void ClearConsole()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks
	if (!FillConsoleOutputCharacter(
		hConsole,        // Handle to console screen buffer 
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write 
		coordScreen,     // Coordinates of first cell 
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(
		hConsole,         // Handle to console screen buffer 
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute 
		coordScreen,      // Coordinates of first cell 
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}

/*
* void PrintBanner(void)
*	Clears the console and prints the trainer banner window.
*/
void PrintBanner(void)
{
	ClearConsole();
	printf(" _____ _____ _____ _____   _ __    _____ _____ _____ \n");
	printf("|  |  |  _  |     |  |  | / |  |  |     |     |_   _|\n");
	printf("|     |     |   --|    -|/ /|  |__|  |  |  |  | | |  \n");
	printf("|__|__|__|__|_____|__|__|_/ |_____|_____|_____| |_|  \n");
	printf("     _____ _____ _____ _____ _____ _____ _____ \n");
	printf("    |_   _| __  |  _  |     |   | |   __| __  |\n");
	printf("      | | |    -|     |-   -| | | |   __|    -|\n");
	printf("      |_| |__|__|__|__|_____|_|___|_____|__|__|\n\n");
}


// ==========================
// Menu Functions
// ==========================

void PrintMenuOptions(void)
{
	printf("0. Exit trainer\n");
	printf("1. Modify Player Health\n");
}

int ModifyHealth(HANDLE hTarget, void* playerStatsPointer)
{
	void* ptr;
	unsigned int desiredHealth;

	if (!ReadProcessMemory(hTarget, playerStatsPointer, &ptr, sizeof(void*), NULL))
	{
		printf("ReadProcessMemory() failed with error: %d.\n", GetLastError());
		return TERROR_MODIFY_HEALTH_FAILED;
	}

	// ptr now contains the address to the player stats structure. 
	// We'll add 0x50 to this address to point to the player's health.
	void* addr = (void*)((int)ptr + 0x50);

	// Get the player's current health
	unsigned int currentHealth;
	if (!ReadProcessMemory(hTarget, addr, &currentHealth, sizeof(int), NULL))
	{
		printf("ReadProcessMemory() failed with error: %d.\n", GetLastError());
		return TERROR_MODIFY_HEALTH_FAILED;
	}

	// Ask the user what they want their health to be (unsigned int max)
	printf("Your current health is %d.\nWhat would you like your new health to be? >> ", currentHealth);
	scanf_s("%u", &desiredHealth);

	// INT_MAX = 2147483647
	if (desiredHealth > INT_MAX || desiredHealth <= 0)
	{
		printf("ERROR: I can only modify health within the range [1,%d]\nHealth unchanged.\n", INT_MAX);
		return TERROR_MODIFY_HEALTH_FAILED;
	}

	if (!WriteProcessMemory(hTarget, addr, &desiredHealth, sizeof(desiredHealth), NULL))
	{
		printf("WriteProcessMemory() failed with error: %d.\n", GetLastError());
		return TERROR_MODIFY_HEALTH_FAILED;
	}
	else
	{
		printf("Your health has been changed.");
	}

	return TERROR_SUCCESS;
}