#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <alloc.h>

unsigned char *b = (char *) malloc(64000);

typedef unsigned int word;

void conv(int a)
{
	word v;

	v = (word)b[a+1]*256;
	v += (word)b[a];
	v -= 80;
	b[a+1] = v / 256;
	b[a] = v % 256;
}

void main()
{
	int h = _open(_argv[1], O_BINARY | O_RDONLY);
	int l;

	l = filelength(h);
	_read(h, b, 44);
	lseek(h, 80, SEEK_CUR);
	_read(h, b+44, l-124);
	conv(0x18);
	conv(50);
	close(h);
	_creat(_argv[1], 0);
	_write(h, b, l-80);
	close(h);
	farfree(b);
}