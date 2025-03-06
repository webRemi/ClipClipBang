#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

#define ERROR "[-]"
#define SUCCES "[+]"
#define INFO "[>]"

char* GetClipboard() {
	UINT uFormat = CF_TEXT;
	HANDLE hClipData = NULL;
	char* cClipText = NULL;
	char* cClipFormated = NULL;

	if (!OpenClipboard(NULL)) {
		printf("%s OpenClipboard failed with error: 0x%x\n", ERROR, GetLastError());
		return 1;
	}

	printf("%s Waiting for clipboard data...\n", INFO);
	hClipData = GetClipboardData(uFormat);
	if (hClipData == NULL) {
		printf("%s GetClipboardData failed with error: 0x%x\n", ERROR, GetLastError());
		return 1;
	}
	printf("%s Successfully retrieved data from clipboard!\n", SUCCES);

	if (!CloseClipboard) {
		printf("%s CloseClipboard failed with error: 0x%x\n", ERROR, GetLastError());
		return 1;
	}

	cClipText = GlobalLock(hClipData);
	if (cClipText == NULL) {
		printf("%s GlobalLock failed with error: 0x%x\n", ERROR, GetLastError());
	}

	if (!GlobalUnlock(hClipData)) {
		printf("%s GlobalUnlock failed with error: 0x%x\n", ERROR, GetLastError());
		return 1;
	}
	cClipFormated = (char*)malloc(strlen(cClipText) + 1);
	if (cClipFormated == NULL) {
		printf("%s Memory allocation failed with error: 0x%x", ERROR, GetLastError());
		return 1;
	}

	printf("%s Clipboard data:\n", SUCCES);
	sprintf(cClipFormated, "====================\n%s\n====================", cClipText);

	return cClipFormated;
}

int main(int argc, char *argv[]) {
	/*
	char* cOldClipText = NULL;
	char* cCurrentClipText = NULL;

	while (1) {
		cCurrentClipText = GetClipboard();
		if (cOldClipText == NULL || strcmp(cCurrentClipText, cOldClipText) != 0) {
			puts("Clipboard text changed!");
			cOldClipText = cCurrentClipText;
			printf("%s\n", cCurrentClipText);
		}
		
		Sleep(5000);
	}
	*/
	char* cClipText = NULL;
	cClipText = GetClipboard();
	printf("%s\n", cClipText);

	LPCSTR lpFileName = argv[1];
	HANDLE hFile = NULL;
	DWORD sClipTextSize = strlen(cClipText);
	LPDWORD lpNumberOfBytesWritten;

	printf("%s Writing to a file\n", INFO);

	hFile = CreateFileA(lpFileName, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL) {
		printf("%s CreateFile failed with error: 0x%x\n", ERROR, GetLastError());
		return 1;
	}

	if (!WriteFile(hFile, cClipText, sClipTextSize, &lpNumberOfBytesWritten, NULL)) {
		printf("%s WriteFile failed with error: 0x%x\n", ERROR, GetLastError());
	}

	return 0;
}