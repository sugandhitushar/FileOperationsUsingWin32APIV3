#ifndef UNICODE
	#define UNICODE
#endif
#include<windows.h>
#include"mydialog.h"

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return(TRUE);
}

// This functions converts Wide Character string to ANSI character string
extern "C" __declspec(dllexport) void WidetoAscii(TCHAR *wStr,char* str)
{
	char Str[100];
	int numBytes;
	numBytes = WideCharToMultiByte(CP_UTF8,0,wStr,-1,Str,sizeof(Str),NULL,NULL);
	strcpy(str,Str);
}

// Opening a File
extern "C" __declspec(dllexport) int FileOpen(char *fileNameA,HFILE *hFile, OFSTRUCT *ofStruct)
{
	*hFile = OpenFile(fileNameA,&(*ofStruct),OF_READ);
	if(*hFile == HFILE_ERROR)
	{
		return FO_FAILED;
	}
	return FO_SUCCESS;
}

// Count Number of characters in file hFile
extern "C" __declspec(dllexport) DWORD countChars(HWND hDlg, HFILE hFile)
{
	BOOL rfReturn;
	DWORD count=0, numBytesRead,fReturn;
	char readData[100];

	fReturn = SetFilePointer((HANDLE) hFile,0,NULL,FILE_BEGIN);
	
	if(INVALID_SET_FILE_POINTER == fReturn|| ERROR_NEGATIVE_SEEK == fReturn)
	{
		MessageBox(hDlg,TEXT("Error in setting File Pointer"),TEXT("Error"),MB_OK | MB_ICONERROR);
		return -1;
	}
	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			count+=numBytesRead;
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);

	return count;
}

// Count number of words in file hFile
extern "C" __declspec(dllexport) DWORD countWords(HWND hDlg, HFILE hFile)
{
	BOOL rfReturn;
	DWORD count=0,fReturn;
	unsigned int i;
	BOOL flag=TRUE;
	char readData[100];
	DWORD numBytesRead;

	fReturn = SetFilePointer((HANDLE) hFile,0,NULL,FILE_BEGIN);
	
	if(INVALID_SET_FILE_POINTER == fReturn|| ERROR_NEGATIVE_SEEK == fReturn)
	{
		MessageBox(hDlg,TEXT("Error in setting File Pointer"),TEXT("Error"),MB_OK | MB_ICONERROR);
		return -1;
	}

	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			if(numBytesRead == 0)
			{
				count++;
			}
			else
			{
				for(i=0; i<numBytesRead; i++)
				{
					if(flag==FALSE)
					{
						if(readData[i] != ' ' || readData[i] != '\t' || readData[i] != '\n')
						{
							flag=TRUE;
							continue;
						}
					}
					else if(flag==TRUE)
					{
						if(readData[i] == ' ' || readData[i] == '\t' || readData[i] == '\n')
						{
							count++;
							flag=FALSE;
						}
					}
				}
			}
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);

	return count;
}