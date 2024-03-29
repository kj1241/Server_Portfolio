//cpu.c
//유닉스용 파일 복사 
//목적: 소스 파일(file1)과 대상 파일(file2). file1의 내용을 file2로 복사합니다. 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SIZE 256

int main (int argc, char *argv [])
{
	int input_fd, output_fd;
	ssize_t bytes_in, bytes_out;
	char rec [BUF_SIZE];
	if (argc != 3) {
		printf ("Usage: cp file1 file2\n");
		return 1;
	}
	input_fd = open (argv [1], O_RDONLY);
	if (input_fd == -1) {
		perror (argv [1]);
		return 2;
	}
	output_fd = open (argv [2], O_WRONLY | O_CREAT, 0666);
	if (output_fd == -1) {
		perror (argv [2]);
		return 3;
	}


	while ((bytes_in = read (input_fd, &rec, BUF_SIZE)) > 0) {
		bytes_out = write (output_fd, &rec, bytes_in);
		if (bytes_out != bytes_in) {
			perror ("Fatal write error.");
			return 4;
		}
	}
	close (input_fd);
	close (output_fd);
	return 0;
}
