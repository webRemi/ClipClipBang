#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ERROR "[-]"
#define SUCCESS "[+]"
#define INFO "[>]"

char* GetClipboard() {
    UINT uFormat = CF_TEXT;
    HANDLE hClipData = NULL;
    char* cClipText = NULL;
    char* cClipFormatted = NULL;

    if (!OpenClipboard(NULL)) {
        printf("%s OpenClipboard failed with error: 0x%x\n", ERROR, GetLastError());
        return NULL;
    }

    printf("%s Waiting for clipboard data...\n", INFO);
    hClipData = GetClipboardData(uFormat);
    if (hClipData == NULL) {
        printf("%s GetClipboardData failed with error: 0x%x\n", ERROR, GetLastError());
        CloseClipboard();
        return NULL;
    }
    printf("%s Successfully retrieved data from clipboard!\n", SUCCESS);

    cClipText = (char*)GlobalLock(hClipData);
    if (cClipText == NULL) {
        printf("%s GlobalLock failed with error: 0x%x\n", ERROR, GetLastError());
        CloseClipboard();
        return NULL;
    }

    cClipFormatted = (char*)malloc(strlen(cClipText) + 50);
    if (cClipFormatted == NULL) {
        printf("%s Memory allocation failed with error: 0x%x\n", ERROR, GetLastError());
        GlobalUnlock(hClipData);
        CloseClipboard();
        return NULL;
    }

    sprintf(cClipFormatted, 
        "=====START=====\n"
        "%s\n"
        "======END======\n",
        cClipText);

    GlobalUnlock(hClipData);
    CloseClipboard();

    return cClipFormatted;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char* cOldClipText = NULL;
    char* cClipText = NULL;
    LPCSTR lpFileName = argv[1];
    HANDLE hFile = NULL;

    while (1) {
        hFile = NULL;
        cClipText = GetClipboard();
        if (!cClipText) {
            printf("%s Failed to get clipboard data.\n", ERROR);
            Sleep(5000);
            continue;
        }

        if (cOldClipText == NULL || strcmp(cClipText, cOldClipText) != 0) {
            printf("%s Clipboard text changed!\n", INFO);

            if (cOldClipText) {
                free(cOldClipText);
            }
            cOldClipText = _strdup(cClipText);

            DWORD sClipTextSize = strlen(cClipText);
            DWORD dwBytesWritten;

            printf("%s Writing to a file\n", INFO);
            hFile = CreateFileA(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                printf("%s CreateFile failed with error: 0x%x\n", ERROR, GetLastError());
                free(cClipText);
                return 1;
            }

            SetFilePointer(hFile, 0, NULL, FILE_END);

            if (!WriteFile(hFile, cClipText, sClipTextSize, &dwBytesWritten, NULL)) {
                printf("%s WriteFile failed with error: 0x%x\n", ERROR, GetLastError());
            }

            CloseHandle(hFile);
        }

        free(cClipText);
        Sleep(5000);
    }

    return 0;
}