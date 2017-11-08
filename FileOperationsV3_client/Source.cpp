#ifndef UNICODE
	#define UNICODE
#endif
#include<windows.h>
#include<stdio.h>
#include"mydialog.h"

// WndProc Prototype
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

// DlgProc Prototype
BOOL CALLBACK DlgProc(HWND,UINT,WPARAM,LPARAM);

// Prototype of ThreadProc
DWORD WINAPI ThreadProcOne(LPVOID);
DWORD WINAPI ThreadProcTwo(LPVOID);

HANDLE g_hMutex;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wndclass;
	MSG msg;
	HWND hwnd;
	TCHAR AppName[] = TEXT("Windows");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = AppName;
	wndclass.lpszMenuName = NULL;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(AppName, TEXT("Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if(NULL == hwnd)
	{
		MessageBox(hwnd, TEXT("Window not created"),TEXT("Error"),MB_OK);
		return 0;
	}

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst;
	int dResult;
	TCHAR str[30] = TEXT("DATAENTRY");
	switch(iMsg)
	{
	case WM_CREATE:
		hInst = (HINSTANCE) GetWindowLong(hwnd,GWL_HINSTANCE);
		// Create dialog box using demo.rc resource script
		dResult = DialogBox(hInst,str,hwnd,DlgProc);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR fileName[100],demo[100];
	char fileNameA[100];
	static HFILE hFile;
	static OFSTRUCT ofStruct;
	static HANDLE hThread1, hThread2;
	int result;
	RECT clientRect;
	DWORD countOfChars=0,countOfWords=0;
	HDC hdc;
	static HMODULE hDll;

	switch(iMsg)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg,ID_FNAME));
		EnableWindow(GetDlgItem(hDlg,ID_COUNTCHARS),FALSE);
		EnableWindow(GetDlgItem(hDlg,ID_COUNTWORDS),FALSE);
		EnableWindow(GetDlgItem(hDlg,ID_COUNTCAPITALSMALL),FALSE);

		hDll = LoadLibrary(TEXT("FileOperationsV3_server.dll"));
		if(hDll==NULL)
		{
			DWORD ret = GetLastError();
			TCHAR str[100];
			wsprintf(str,TEXT("Unable to load DLL: server.dll: Error code : %d"),ret);
			MessageBox(hDlg,str, TEXT("Error"),MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
		}

		return (TRUE);

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_OPENFILE:
			GetDlgItemText(hDlg,ID_FNAME,fileName,100);

			typedef void (*WtoA)(TCHAR*,char*);
			WtoA WidetoAscii;
			WidetoAscii = (WtoA) GetProcAddress(hDll,"WidetoAscii");
			WidetoAscii(fileName,fileNameA);

			typedef int (*Fopen)(char *,HFILE *, OFSTRUCT *);
			Fopen FileOpen;
			FileOpen = (Fopen) GetProcAddress(hDll,"FileOpen");
			result = FileOpen(fileNameA,&hFile,&ofStruct);
			if(result == FO_SUCCESS)
			{
					TCHAR demo[100];
					wsprintf(demo,TEXT("File Opened Successfully : %s"),fileName);
					MessageBox(hDlg,demo,TEXT("FO_SUCCESS"),MB_OK);

					EnableWindow(GetDlgItem(hDlg,ID_COUNTCHARS),TRUE);
					EnableWindow(GetDlgItem(hDlg,ID_COUNTWORDS),TRUE);
					EnableWindow(GetDlgItem(hDlg,ID_COUNTCAPITALSMALL),TRUE);
			}
			else if(result == FO_FAILED)
			{
				MessageBox(hDlg,TEXT("Cannot open file. Please specify correct File Name."),TEXT("Error"),MB_OK);
				SetDlgItemText(hDlg,ID_FNAME,TEXT(""));
				SetFocus(GetDlgItem(hDlg,ID_FNAME));
			}
			return (TRUE);

		case ID_COUNTCHARS:
			typedef DWORD (*cChars)(HWND, HFILE);
			cChars countChars;
			countChars = (cChars) GetProcAddress(hDll,"countChars");
			countOfChars = countChars(hDlg,hFile);
			
			wsprintf(demo,TEXT("Total characters in file are %d"),countOfChars);
			hdc = GetDC(hDlg);
			TextOut(hdc,200,300,demo,lstrlen(demo));
			ReleaseDC(hDlg,hdc);

			return (TRUE);

		case ID_COUNTWORDS:
			typedef DWORD (*cWords)(HWND, HFILE);
			cWords countWords;
			countWords = (cWords) GetProcAddress(hDll,"countWords");
			countOfWords = countWords(hDlg,hFile);
			
			wsprintf(demo,TEXT("Total words in file are %d"),countOfWords);

			GetClientRect(hDlg,&clientRect);
			InvalidateRect(hDlg,&clientRect,TRUE);
			UpdateWindow(hDlg);

			hdc = GetDC(hDlg);
			TextOut(hdc,200,300,demo,lstrlen(demo));
			ReleaseDC(hDlg,hdc);

			return (TRUE);

		case ID_CLOSE:
			CloseHandle((HANDLE) hFile);
			EndDialog(hDlg,0);
			PostQuitMessage(0);
			return (TRUE);

		case ID_COUNTCAPITALSMALL:
			ThreadParams t1;
			t1.hDlg = hDlg;
			t1.hFile = hFile;
			
			g_hMutex = CreateMutex(NULL, FALSE,(LPCWSTR) TEXT("Mutex1"));

			hThread1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ThreadProcOne,(LPVOID) &t1,0,NULL);
			hThread2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ThreadProcTwo,(LPVOID) &t1,0,NULL);

			WaitForSingleObject(hThread1,INFINITE);
			WaitForSingleObject(hThread2,INFINITE);
		}
		return (TRUE);
	}
	return(FALSE);
}

DWORD WINAPI ThreadProcOne(LPVOID param)
{
	ThreadParams *t2 = (ThreadParams*) param;
	ThreadParams t1 = *t2;
	int i;
	BOOL rfReturn;
	HFILE hFile = t1.hFile;
	HWND hDlg = t1.hDlg;
	DWORD count=0, numBytesRead;
	char readData[100];
	HDC hdc;
	TCHAR demo[100];
	DWORD dwStatus;

	WaitForSingleObject(g_hMutex,INFINITE);
	DWORD fReturn = SetFilePointer((HANDLE) hFile,0,NULL,FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER == fReturn|| ERROR_NEGATIVE_SEEK == fReturn)
	{
		MessageBox(hDlg,TEXT("Error in setting File Pointer"),TEXT("Error"),MB_OK | MB_ICONERROR);
		ExitThread(0);
	}

	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			for(i=0; i<numBytesRead; i++)
			{
				if(readData[i] >=65 && readData[i] <=90)
					count++;
			}
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);
	
	ReleaseMutex(g_hMutex);
	wsprintf(demo,TEXT("Total capital letters in file are %d"),count);
	hdc = GetDC(hDlg);
	TextOut(hdc,200,300,demo,lstrlen(demo));
	ReleaseDC(hDlg,hdc);
	return 0;
}

DWORD WINAPI ThreadProcTwo(LPVOID param)
{
	ThreadParams *t2 = (ThreadParams*) param;
	ThreadParams t1 = *t2;
	int i;
	BOOL rfReturn;
	HFILE hFile = t1.hFile;
	HWND hDlg = t1.hDlg;
	DWORD count=0, numBytesRead;
	char readData[100];
	HDC hdc;
	TCHAR demo[100];
	DWORD dwStatus;

	WaitForSingleObject(g_hMutex,INFINITE);
	DWORD fReturn = SetFilePointer((HANDLE) hFile,0,NULL,FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER == fReturn|| ERROR_NEGATIVE_SEEK == fReturn)
	{
		MessageBox(hDlg,TEXT("Error in setting File Pointer"),TEXT("Error"),MB_OK | MB_ICONERROR);
		ExitThread(0);
	}

	do
	{
		rfReturn = ReadFile((HANDLE) hFile,readData,100,&numBytesRead,NULL);
		if(rfReturn == TRUE)
		{
			for(i=0; i<numBytesRead; i++)
			{
				if(readData[i] >=97 && readData[i] <=122)
					count++;
			}
		}
		else
		{
			MessageBox(hDlg,TEXT("Unable to read data"),TEXT("Error"),MB_OK);
		}
	}while(numBytesRead!=0);

	ReleaseMutex(g_hMutex);

	wsprintf(demo,TEXT("Total small letters in file are %d"),count);
	hdc = GetDC(hDlg);
	TextOut(hdc,200,330,demo,lstrlen(demo));
	ReleaseDC(hDlg,hdc);
	return 0;
}