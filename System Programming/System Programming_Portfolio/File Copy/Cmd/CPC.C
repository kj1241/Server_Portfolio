//cpc.c
//C라이브러리를 이용한 파일복사
//목적: 소스 파일(file1)과 대상 파일(file2)을 cmd 에서 복사합니다.

#include <windows.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SIZE 256

int main (int argc, char *argv [])
{
	FILE *in_file, *out_file;
	char rec [BUF_SIZE];
	size_t bytes_in, bytes_out;
	if (argc != 3) {
		printf ("Usage: cp file1 file2\n");
		return 1;
	}
	in_file = fopen (argv [1], "rb");
	if (in_file == NULL) {
		perror (argv [1]);
		return 2;
	}
	out_file = fopen (argv [2], "wb");
	if (out_file == NULL) {
		perror (argv [2]);
		return 3;
	}

	while ((bytes_in = fread (rec, 1, BUF_SIZE, in_file)) > 0) {
		bytes_out = fwrite (rec, 1, bytes_in, out_file);
		if (bytes_out != bytes_in) {
			perror ("Fatal write error.");
			return 4;
		}
	}

	fclose (in_file);
	fclose (out_file);
	return 0;
}
