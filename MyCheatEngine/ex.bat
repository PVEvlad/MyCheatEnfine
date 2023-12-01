cls
gcc main.c -c -o bin/main.o
gcc readmem.c -c -o bin/readmem.o
windres resources.rc -o bin/res.o
gcc bin/main.o bin/readmem.o bin/res.o bin/gdi32.dll bin/comdlg32.dll -o cheat.exe
cheat.exe