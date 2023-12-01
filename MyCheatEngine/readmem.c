#include "readmem.h"
#include "res.h"
#include <tlhelp32.h>
#include <stdio.h>

extern char filename[MAX_PATH];

char BUFFER[0x1000];
long long number_of_adresses=0;
void** adresses=0;

void rClear(HANDLE curProc, HANDLE sProc)
{
    CloseHandle(curProc);
    free(adresses);
    adresses=0;
    memset(filename,0,strlen(filename));
    InvalidateRect(sProc,0,1);
    return;
}

void rScanVirtualPages(HANDLE handleToProcess, int _searching_for)
{
	SYSTEM_INFO sSysInfo = {};
	MEMORY_BASIC_INFORMATION memInfo;
    GetSystemInfo(&sSysInfo);

	void *currentScanAddress = 0;
    int current=0;
    number_of_adresses=0;
    if(adresses){free(adresses);adresses=0;}

	while (1)
	{
		SIZE_T readBytes = VirtualQueryEx(handleToProcess, currentScanAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
		if (!readBytes) break;

		currentScanAddress = (char *)memInfo.BaseAddress + memInfo.RegionSize;
		if (memInfo.State == MEM_COMMIT && memInfo.Protect == PAGE_READWRITE)
		{
            for(memInfo.BaseAddress-=0x1000,memInfo.RegionSize+=0x1000;memInfo.BaseAddress+=0x1000,memInfo.RegionSize-=0x1000;memInfo.RegionSize>0)
            {
                ReadProcessMemory(handleToProcess, memInfo.BaseAddress, BUFFER, 0x1000, &readBytes);
                for(int i=0;i<0x1000;i++)
                    if((*((int*)(BUFFER+i)))==_searching_for)number_of_adresses++;
            }
		}
	}

    currentScanAddress = 0;
    adresses=malloc(2*(number_of_adresses+10)*(sizeof(void*)));
    number_of_adresses=0;
	while (1)
	{
		SIZE_T readBytes = VirtualQueryEx(handleToProcess, currentScanAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
		if (!readBytes) break;
		currentScanAddress = (char *)memInfo.BaseAddress + memInfo.RegionSize;
		if (memInfo.State == MEM_COMMIT && memInfo.Protect == PAGE_READWRITE)
            for(
                memInfo.BaseAddress-=0x1000,memInfo.RegionSize+=0x1000;
                memInfo.BaseAddress+=0x1000,memInfo.RegionSize-=0x1000;
                memInfo.RegionSize>0
                )
            {
                ReadProcessMemory(handleToProcess, memInfo.BaseAddress, BUFFER, 0x1000, &readBytes);
                for(int i=0;i<0x1000;i++)
                    if((*((int*)(BUFFER+i)))==_searching_for)
                    {/*printf("%p|",memInfo.BaseAddress+i);*/
                        adresses[current]=memInfo.BaseAddress+i;
                        current++;

                    }
            }
		
	}
    number_of_adresses=current;
}

HANDLE rFindProc(char* exename)
{
    HANDLE          SnapShot;
    HANDLE          hProcess=0;
    char* new=exename+strlen(exename)-1;

    while(exename!=new && (*(new-1))!='\\')new--;

    PROCESSENTRY32  Entry;
    Entry.dwSize = sizeof(Entry);

    SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    BOOL bSet = Process32First(SnapShot, &Entry);
 
    while (bSet)
    {
        if (strcmp(Entry.szExeFile, new) == 0)
        {
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, Entry.th32ProcessID);
            break;
        }
        bSet = Process32Next(SnapShot, &Entry);
    }

CloseHandle(SnapShot);
if(!hProcess){MessageBoxW(0,ERR_PROC,ERR_,0); return 0;}
else return hProcess;
}