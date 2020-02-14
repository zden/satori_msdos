#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <alloc.h>
#include <stdlib.h>

void main()
{
	int h = _open(_argv[1], O_BINARY | O_RDWR);
	long a, l;
	char ch;

	l = atol(_argv[2]) - filelength(h);

	lseek(h, 0, SEEK_END);
	for (a = 0; a < l; a++)
	{
		ch = random(256);
		_write(h, &ch, 1);
	}
	close(h);
}