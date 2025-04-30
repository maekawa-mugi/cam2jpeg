#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
	u_char	buf[256];
	int	len;

	if ((len = read(0, buf, 128)) < 0)
		exit(1);
	while ((len = read(0, buf, sizeof(buf))) > 0)
		write(1, buf, len);
	exit(0);
}
