#include <windows.h>	// Windows API functions (see MSDN)
#include <stdio.h>		// Standard I/O functions (sprintf, etc.)
#include "helpers.h"	// Our trainer helper functions

// ==========================
// Main
// ==========================

int main()
{
	// Acquire a handle to the target game
	wchar_t* targetProcess = L"HackSlashLoot.exe";
	int targetPid = GetProcId(targetProcess);
	if (targetPid == TERROR_PID_LOOKUP_FAILED)
	{
		PrintBanner();
		printf("Unable to locate target process \'%ls\'\n", targetProcess);
		return TERROR_PID_LOOKUP_FAILED;
	}

	// Open the target process (We could also use PROCESS_ALL_ACCESS)
	// Read more on access rights: http://msdn.microsoft.com/en-us/library/windows/desktop/ms684880%28v=vs.85%29.aspx
	HANDLE hTarget = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, TRUE, targetPid);
	if (hTarget == NULL)
	{
		PrintBanner();
		printf("OpenProcess failed on \'%ls\'\n", targetProcess);
		return TERROR_OPENPROC_FAILED;
	}

	// Get the base address of our target
	int targetBaseAddress = GetBaseAddress(targetProcess, targetPid);

	// Hack Slash Loot Demo Version DB7A specific values
	//									     BaseAddress + Offset to a Reliable Pointer Offset
	void* playerStatsPointer = (void*)(targetBaseAddress + 0x007A6140);

	BOOL showMenu = TRUE;
	int selection = 0;
	int result;
	
	while (showMenu)
	{
		PrintBanner();
		PrintMenuOptions();
		printf("\nSelect an option: ");
		scanf_s("%d", &selection);

		switch (selection)
		{
		case 0:
			showMenu = FALSE;
			break;
		case 1:
			result = ModifyHealth(hTarget, playerStatsPointer);
			break;
		default:
			printf("Please enter a valid selection.\n");
			break;
		}

		if(selection != 0)
			getchar(); getchar();
	}
	
	CloseHandle(hTarget);
	return 0;
}