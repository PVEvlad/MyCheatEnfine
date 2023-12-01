#include <string.h>
#include <stdio.h>
#include <tchar.h>

#include "main.h"
#include "readmem.h"
#include "res.h"

extern void** adresses;
extern long long number_of_adresses;

HWND mainwin, sFileName,cAddrList, eSearch, eChange, bSet, bFile,bUpdate;
HINSTANCE hInst;
HFONT font;
HANDLE process=0;

int MaxW,MaxH,search_value;
char filename[MAX_PATH];
_Bool fchosen=0;

void mUpdate(int num)
{
  SendMessage(cAddrList,CB_RESETCONTENT,0,0);
  int i=0;
  int current;
  size_t readBytes;
  char string[20];
for(int k=0;k<number_of_adresses && i<1000;k++)
{
  ReadProcessMemory(process, adresses[k], &current, 4, &readBytes);
  if(current==num)
  {    sprintf(string,"%p ",adresses[k]);
    SendMessage(cAddrList,CB_ADDSTRING,0,(LPARAM)string);
    i++;
  }
  else adresses[k]=0;
}
}

int  mGetNum(HWND window)
{
  wchar_t buffer[40];
  int res=0,i=0,cur;
  GetWindowTextW(window,buffer,40);
  while(buffer[i])
  {
    cur=buffer[i]-L'0';
    res*=10;
    res+=cur;
    i++;
  }
  return res;
}

long long int  mGetPointer(HWND window)
{
  wchar_t buffer[40];
  long long int res=0,i=0,cur;
  GetWindowTextW(window,buffer,40);
  while(buffer[i+1])
  {
    if(buffer[i]>=L'a' && buffer[i]<=L'f'){cur =buffer[i]-L'a'+10;}
    else cur=buffer[i]-L'0';
    res*=16;
    res+=cur;
    //printf("|%p|\n",res);
    i++;
  }
  //printf("%p ",res);
  return res;
}

void mDumpList()
{
  char string[100];
  SendMessage(cAddrList,CB_RESETCONTENT,0,0);
  for(int i=0;i<number_of_adresses && i<1000;i++)
  {
    sprintf(string,"%p ",adresses[i]);
    SendMessage(cAddrList,CB_ADDSTRING,0,(LPARAM)string);
  }
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
  FreeConsole();
  hInst=hInstance;
  WNDCLASS mywin={0,WindowProc,0,0,0,0,0,0,0,"EngineWindow"};
  mywin.hInstance=hInstance;
  mywin.hbrBackground=CreateSolidBrush(RGB(0x99,0x99,0x99));
  mywin.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
  mywin.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IconID));

  MaxW=GetSystemMetrics(0);
  MaxH=GetSystemMetrics(1);

  if(!RegisterClass(&mywin))MessageBoxW(0,ERR_CLASS,ERR_,0);

  mainwin=CreateWindowW(L"EngineWindow",L"CheatEngine", WS_SYSMENU|WS_VISIBLE|WS_MINIMIZEBOX,0,0,MaxW,MaxH,0,0,hInstance,0);
  if(!mainwin)MessageBoxW(0,ERR_WINDOW,ERR_,0);

  sFileName=CreateWindowW(L"Static",L"",WS_CHILD | WS_BORDER | WS_VISIBLE,0,0,MaxW,MaxH/20,mainwin,0,hInstance,0);
  cAddrList=CreateWindowW(L"ComboBox", NULL,WS_CHILD | WS_VISIBLE | WS_VSCROLL |CBS_SIMPLE,0, MaxH/8, MaxW/3, MaxH*2/3,mainwin, 0, hInstance, NULL);
  eSearch=CreateWindowW(L"Edit", L"Искать...",WS_CHILD | WS_VISIBLE,0, MaxH/20, MaxW/3, MaxH/20,mainwin, 0, hInstance, NULL);
  eChange=CreateWindowW(L"Edit", L"Заменить на...",WS_CHILD | WS_VISIBLE,MaxW/2, MaxH/20, MaxW/3, MaxH/20,mainwin, 0, hInstance, NULL);

  bSet=CreateWindowW(L"Button", L"",WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,MaxW/2, MaxH/3, MaxW/20, MaxH/20,mainwin, (HMENU)IDB_Set, hInstance, NULL);
  bUpdate=CreateWindowW(L"Button", L"",WS_CHILD | WS_VISIBLE|BS_OWNERDRAW,MaxW/3, MaxH/2, MaxW/20, MaxH/20,mainwin, (HMENU)IDB_Update, hInstance, NULL);

  font=CreateFont(MaxH/20-2,MaxH/40,0,0,4,0,0,0,0,0,0,0,5,0);
  SendMessage(sFileName, WM_SETFONT, (WPARAM)font, 0);
  SendMessage(eSearch, WM_SETFONT, (WPARAM)font, 0);
  SendMessage(eChange, WM_SETFONT, (WPARAM)font, 0);

  MSG msg;
  BOOL bRet;

  while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
  { 
      if (bRet == -1) PostQuitMessage(0);
      else
      {
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
      }
  }
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
PAINTSTRUCT ps;
HDC hdc;
 switch (uMsg)
    {
    case WM_CREATE:  break;
    case WM_COMMAND: 
      switch(wParam)
      {
        case m_UPDATE: mUpdate(mGetNum(eSearch)); break;
        case m_FILE_OPEN:mFile(); break;
        case m_CHANGE: 
        int to_write=mGetNum(eChange);
        size_t bytesset;
        WriteProcessMemory(process,mGetPointer(cAddrList),&to_write,4,&bytesset);
        //printf(":%ld\n",bytesset);
        break;
        case m_EDIT_DUMP:
       //   printf("num:%d",mGetNum());
          rScanVirtualPages(process,mGetNum(eSearch)); mDumpList();
          break;
        case m_EXIT: DestroyWindow(mainwin);
        case m_FILE_CLOSE: rClear(process,sFileName); SendMessage(cAddrList,CB_RESETCONTENT,0,0); process=0; break;
        default: break;
      }break;
    case WM_DRAWITEM: mDrawButton((LPDRAWITEMSTRUCT)lParam);break;
    case WM_NCMOUSEMOVE: break;
    case WM_MOUSEMOVE:   break;
    case WM_LBUTTONDOWN: break;
    case WM_PAINT:     
    hdc=BeginPaint(mainwin, &ps);
    EndPaint(mainwin,&ps);
    break;
    case WM_DESTROY: PostQuitMessage(0);
    case WM_KEYDOWN: break;
    case WM_KEYUP:   break;
        break;
	}

    return DefWindowProcA(hwnd,uMsg,wParam,lParam);
    
}

// Функция DrawButton перерисовывает кнопку
void mDrawButton(LPDRAWITEMSTRUCT lpInfo)
{
  HBITMAP hbm;
  int ResourceID;

  if(lpInfo->CtlType != ODT_BUTTON) return;// Обрабатываем сообщение WM_DRAWITEM только если оно поступило от кнопки 
  
  switch (lpInfo->CtlID)
  {
    case    IDB_Set:    ResourceID=IDR_bSetDown;  break;
    case    IDB_Update: ResourceID=IDR_bUpdDown;  break;
    default:            ResourceID=IDR_bDown;     break;
  }

  if (!(lpInfo->itemState & ODS_SELECTED)) ResourceID++;// Загружаем идентификатор изображения кнопки в нужном состоянии
   
  hbm = LoadBitmap(hInst, MAKEINTRESOURCE(ResourceID));  // Загружаем изображение кнопки из ресурсов
  if(!hbm) {MessageBoxW(0,ERR_BUTTON,ERR_,0); return;}

  if((lpInfo->itemAction & ODA_DRAWENTIRE) || (lpInfo->itemAction & ODA_SELECT))// Если кнопка выбрана и ее надо целиком перерисовать
  mDrawBitmap(lpInfo->hDC, (lpInfo->rcItem).left, (lpInfo->rcItem).top , hbm);// Рисуем кнопку

  DeleteObject(hbm);// Удаляем изображение кнопки
}

void mDrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
  HDC hMemDC;
  BITMAP bm;

  hMemDC = CreateCompatibleDC(hDC);                 // Создаем совместимый контекст памяти
  SelectObject(hMemDC, hBitmap); // Выбираем изображение bitmap
  
  GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);// Определяем размеры изображения
  StretchBlt(hDC, x, y, MaxW/20,MaxH/20,hMemDC, 0,0,bm.bmWidth,bm.bmHeight, SRCCOPY);// Рисуем изображение с искажением по размерам

  DeleteDC(hMemDC);// Удаляем контекст памяти
}


void mFile()
{
  //if(process){rClear(process,sFileName); process=0;}
  OPENFILENAME A={sizeof(OPENFILENAME)};
  PAINTSTRUCT sp;
  memset(filename,0,MAX_PATH);
  A.lpstrFilter="Executable \0*.exe";
  A.nMaxFile=MAX_PATH;
  A.lpstrFile=filename;    
  GetOpenFileName(&A);
  process=rFindProc(filename);
 
  fchosen=1;
 if(process==0)goto end;

  InvalidateRect(sFileName,0,1);
  HDC hdc = BeginPaint(sFileName,&sp);
  SelectObject(hdc,font);
  SetBkMode(hdc,TRANSPARENT);
  TextOut(hdc,0,0,filename,strlen(filename));
  EndPaint(sFileName,&sp);
  end:
  return;
}