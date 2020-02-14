#include <io.h>
#include <fcntl.h>
#include <dos.h>

void main()
{
	int h = _open(_argv[1], O_BINARY | O_RDONLY), h1 = _creat(_argv[2], 0);
	int l = filelength(h), a, b;
	char by, t;

	for (a = 0; a < 500; a++)
	{
		t = 0;
		for (b = 0; b < 8; b++)
		{
			_read(h, &by, 1);
			if (by)
				t |= (128 >> b);
		}
		_write(h1, &t, 1);
	}

	close(h); close(h1);
}