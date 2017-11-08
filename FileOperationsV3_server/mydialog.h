#define ID_FNAME 101
#define ID_COUNTCHARS 102
#define ID_COUNTWORDS 103
#define ID_CLOSE 104
#define ID_OPENFILE 105
#define ID_COUNTCAPITALSMALL 106
#define FO_SUCCESS 1
#define FO_FAILED 2
//#define FO_ALREADY_OPEN 3

typedef struct tagThreadParams
{
	HFILE hFile;
	HWND hDlg;
	HANDLE hThread;
} ThreadParams;

