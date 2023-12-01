#include <windows.h>
//#define _searching_for (int)8889

HANDLE rFindProc(char* exename);
void rScanVirtualPages(HANDLE handleToProcess, int _searching_for);
void rClear(HANDLE curProc, HANDLE sProc);
