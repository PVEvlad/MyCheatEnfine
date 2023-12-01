#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

void mDrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap);
void mDrawButton(LPDRAWITEMSTRUCT lpInfo);
void mFile();
void mDumpList();
int  mGetNum(HWND window);
void mUpdate(int num);
long long int  mGetPointer(HWND window);
