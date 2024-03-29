//cpcf.c
// CopyFile을 이요한 파일 복사
//목적: 소스 파일(file1)과 대상 파일(file2)을 CopyFile함수를 사용하여 복사합니다.

#include <windows.h>
#include <stdio.h>
#define BUF_SIZE 256

int main (int argc, LPTSTR argv [])
{
	if (argc != 3) {
		printf ("Usage: cp file1 file2\n");
		return 1;
	}
	if (!CopyFile (argv [1], argv [2], FALSE)) {
		printf ("CopyFile Error: %x\n", GetLastError ());
		return 2;
	}
	return 0;
}
