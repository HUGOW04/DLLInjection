#include <iostream>
#include <Windows.h>

using namespace std;

OPENFILENAME ofn;
DWORD error_value;

bool openFileDialog(TCHAR szFileName[])
{
	const TCHAR* FilterSpec = "All Files(.)\0*.*\0";
	const TCHAR* Title = "Open";

	const TCHAR* myDir = "C:\\c_plus_plus_trial";

	TCHAR szFileTitle[MAX_PATH] = { '\0' };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	*szFileName = 0;

	/* fill in non-variant fields of OPENFILENAME struct. */
	ofn.lStructSize = sizeof(OPENFILENAME);

	ofn.hwndOwner = GetFocus();
	ofn.lpstrFilter = FilterSpec;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = myDir;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrTitle = Title;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	// false if failed or cancelled
	if (GetOpenFileName(&ofn) == 1)
	{

		MessageBox(NULL, "Opening filepath", "SUCCESS !!!", MB_OK);
		return 1;
	}
	else
	{

		MessageBox(NULL, "error", "FAILURE !!!", MB_OK);
		return 0;
	}
}

int main()
{
	TCHAR DllPath[MAX_PATH]; // The Path to our DLL
	openFileDialog(DllPath);


	HWND hwnd = FindWindowA(NULL, "Clock"); // HWND (Windows window) by Window Name
	DWORD procID; // A 32-bit unsigned integer, DWORDS are mostly used to store Hexadecimal Addresses
	GetWindowThreadProcessId(hwnd, &procID); // Getting our Process ID, as an ex. like 000027AC
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); // Opening the Process with All Access

	// Allocate memory for the dllpath in the target process, length of the path string + null terminator
	LPVOID pDllPath = VirtualAllocEx(handle, 0, strlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

	// Write the path to the address of the memory we just allocated in the target process
	WriteProcessMemory(handle, pDllPath, (LPVOID)DllPath, strlen(DllPath) + 1, 0);

	// Create a Remote Thread in the target process which calls LoadLibraryA as our dllpath as an argument -> program loads our dll
	HANDLE hLoadThread = CreateRemoteThread(handle, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), pDllPath, 0, 0);

	WaitForSingleObject(hLoadThread, INFINITE); // Wait for the execution of our loader thread to finish

	cout << "Dll path allocated at: " << hex << pDllPath << endl;
	cout << "dllPath: " << DllPath << endl;
	cin.get();

	VirtualFreeEx(handle, pDllPath, strlen(DllPath) + 1, MEM_RELEASE); // Free the memory allocated for our dll path

	return 0;
}