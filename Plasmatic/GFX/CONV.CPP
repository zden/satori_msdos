#include <io.h>
#include <fcntl.h>
#include <dos.h>

char conv(char t)
{
	if (!t) return(0);
	if (t == 16) return(1);
	if (t == 19) return(2);
	return(3);
}

void main()
{
	int h = _open(_argv[1], O_BINARY | O_RDONLY), h1 = _creat(_argv[2], 0);
	int l = filelength(h), a;
	char b, t;

	for (a = 0; a < l/4; a++)
	{
		b = 0;
		_read(h, &t, 1);
		b |= conv(t) << 6;
		_read(h, &t, 1);
		b |= conv(t) << 4;
		_read(h, &t, 1);
		b |= conv(t) << 2;
		_read(h, &t, 1);
		b |= conv(t);
		_write(h1, &b, 1);
	}
	close(h); close(h1);
}