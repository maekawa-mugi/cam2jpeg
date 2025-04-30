/* Casio QV-10 cam file converter.
 * Jun-ichiro "itojun" Itoh. <itojun@mt.cs.keio.ac.jp> <itojun@csl.sony.co.jp>
 * Freely redistributable.
 * $Id: cam.c,v 1.7 1996/08/27 16:26:40 itojun Exp $
 */
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/* known area IDs */
#define	AREA_COMMENT	1
#define	AREA_THUMBNAIL	2
#define	AREA_JFIF10	3
#define	AREA_JFIF100	4

/* area information */
struct areainfo {
	u_short	id;
	u_int	len;
};

static	int		narea;
static	struct areainfo	*areahdr = (struct areainfo *)NULL;

u_short
get_u_short(buf)
	u_char	*buf;
{
	return ((u_short)buf[0] << 8) | buf[1];
}

u_int
get_u_int(buf)
	u_char	*buf;
{
	u_int	t;

	t = (((u_int)buf[0] << 8) | buf[1]) << 16;;
	t |= ((u_int)buf[2] << 8) | buf[3];;
	return t;
}

void
skipfile(fd, len)
	int	fd;
	int	len;
{
	u_char	buf[256];
	int	l;

	while (len > 0) {
		l = read(fd, buf, (len > sizeof(buf)) ? sizeof(buf) : len);
		if (l < 0) {
			perror("read");
			exit(1);
		}
		len -= l;
	}
}

void
copyfile(fd1, fd2, len)
	int	fd1;
	int	fd2;
	int	len;
{
	u_char	buf[256];
	int	l;

	while (len > 0) {

		l = read(fd1, buf, (len > sizeof(buf)) ? sizeof(buf) : len);
		if (l < 0) {
			perror("read");
			exit(1);
		}
		if (write(fd2, buf, l) < 0) {
			perror("write");
			exit(1);
		}
		len -= l;
	}
}

int
readhdr(fd)
	int	fd;
{
	u_char	buf[256];
	int	ret;
	int	i;
	static u_char	cam_magic[] = { 0x07, 0x20, 0x4d, 0x4d };
	static u_char	cam_macbinsig[] = { 'C','A','M','f','Q','V','T','m' };

	/*
	 * lseek(fd) assumed to be 0.
	 */
	/*
	 * check for CAM file magic number.
	 */
	if (read(fd, buf, 4) < 0) {
		perror("read");
		exit(1);
	}
	/* SunOS dislikes \x00 form. why? */
	if (bcmp(cam_magic, buf, sizeof(cam_magic)) == 0) {
		goto magicok;	/* CAM file signature found. */
	} else {
		/* try MacBinary */
		if (read(fd, buf + 4, 128) < 0) {
			perror("read");
			exit(1);
		}
		if (bcmp(buf + 128, cam_magic, sizeof(cam_magic)) == 0
		 && bcmp(buf + 65, cam_macbinsig, sizeof(cam_macbinsig)) == 0) {
			goto magicok;		/* CAM file signature found. */
		} else {
			return 0;	/* is not cam file */
		}
	}
magicok:

	/*
	 * fill up area table.
	 */
	if (read(fd, buf, 2) < 0) {
		perror("read");
		exit(1);
	}
	narea = get_u_short(buf);
	areahdr = (struct areainfo *)calloc(sizeof(struct areainfo), narea);
	if (areahdr == (struct areainfo *)NULL) {
		perror("calloc");
		exit(1);
	}
	for (i = 0; i < narea; i++)  {
		if (read(fd, buf, 16) < 0) {
			perror("read(in seektoarea)");
			exit(1);
		}
		areahdr[i].id = get_u_short(buf);
		areahdr[i].len = get_u_int(buf + 2);
	}
	/*
	 * lseek(fd) assumed to be (4 + 2 + 16*narea).
	 */
	return 1;	/*ok*/
}

struct areainfo *
skiptoarea(fd, areaid)
	int	fd;
	u_short	areaid;
{
	int	i;
	int	len;

	len = 0;
	for (i = 0; i < narea; i++)  {
		if (areahdr[i].id == areaid) {
			skipfile(fd, len);
			return &areahdr[i];	/*ok*/
		} else {
			len += areahdr[i].len;
		}
	}

	return (struct areainfo *)NULL;	/*error*/
}

#ifdef CAMDUMP
void
main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*s;
	u_short	areaid = 0;
	int	fd;
	struct areainfo	*a;

	while (--argc > 0 && **++argv == '-') {
		for (s = argv[0] + 1; *s != '\0'; s++) {
			switch (*s) {
			case '1':
			case '2':
			case '3':
				areaid = *s - '0';
				break;
			}
		}
	}

	if (areaid == 0) {
		fprintf(stderr, "areaid not specified\n");
		exit(1);
	}

	if (argc == 0) {
		fd = 0;	/*stdin*/
	} else if (argc == 1) {
		fd = open(*argv, O_RDONLY);
		if (fd < 0) {
			perror("open");
			exit(1);
		}
	} else {
		fprintf(stderr, "too many arguments\n");
		exit(1);
	}

	if (! readhdr(fd)) {
		fprintf(stderr, "%s is not cam file\n", *argv);
		exit(1);
	}
	if ((a = skiptoarea(fd, areaid)) == (struct areainfo *)NULL) {
		fprintf(stderr, "area %d not found\n", areaid);
	}
	copyfile(fd, 1, a->len);
	close(fd);
}
#endif /*CAMDUMP*/

#ifdef CAMTOPPM
void
main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*s;
	int	fd;
	struct areainfo	*a;

	while (--argc > 0 && **++argv == '-') {
		for (s = argv[0] + 1; *s != '\0'; s++) {
			switch (*s) {
			}
		}
	}

	if (argc == 0) {
		fd = 0;	/*stdin*/
	} else if (argc == 1) {
		fd = open(*argv, O_RDONLY);
		if (fd < 0) {
			perror("open");
			exit(1);
		}
	} else {
		fprintf(stderr, "too many arguments\n");
		exit(1);
	}

	if (! readhdr(fd)) {
		fprintf(stderr, "%s is not cam file\n", *argv);
		exit(1);
	}
	if ((a = skiptoarea(fd, AREA_THUMBNAIL)) == (struct areainfo *)NULL) {
		fprintf(stderr, "area %d not found\n", AREA_THUMBNAIL);
	}
	write(1, "P6\n52 36\n255\n", 13);
	copyfile(fd, 1, a->len);
	close(fd);
}
#endif /*CAMTOPPM*/

#ifdef CAMTOJPEG
#include "cam2jpgtab.h"

void
main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*s;
	int	fd;
	int	len;
	u_char	buf[256];
	struct areainfo	*a;

	u_short	njarea;
	u_short	ysize, usize, vsize;

	while (--argc > 0 && **++argv == '-') {
		for (s = argv[0] + 1; *s != '\0'; s++) {
			switch (*s) {
			}
		}
	}

	if (argc == 0) {
		fd = 0;	/*stdin*/
	} else if (argc == 1) {
		fd = open(*argv, O_RDONLY);
		if (fd < 0) {
			perror("open");
			exit(1);
		}
	} else {
		fprintf(stderr, "too many arguments\n");
		exit(1);
	}

	if (! readhdr(fd)) {
		fprintf(stderr, "%s is not cam file\n", *argv);
		exit(1);
	}

	if (a = skiptoarea(fd, AREA_JFIF100)) {
		/* this is qv100 cam file. */
		copyfile(fd, 1, a->len);
	} else if (a = skiptoarea(fd, AREA_JFIF10)) {
		/* this is qv10 cam file. */
		if (read(fd, buf, 8) < 0) {
			perror("read(reading JFIF header)");
			exit(1);
		}
		njarea = get_u_short(buf);
		ysize = get_u_short(buf + 2);
		usize = get_u_short(buf + 4);
		vsize = get_u_short(buf + 6);
		if (njarea != 3) {
			fprintf(stderr, "JFIF area# != 3, not supported\n");
			exit(1);
		}

		write(1, soi, sizeof(soi));
		write(1, app0, sizeof(app0));
		write(1, dqt0, sizeof(dqt0));

		copyfile(fd, 1, 64);

		write(1, dqt1, sizeof(dqt1));
		copyfile(fd, 1, 64);

		write(1, sof, sizeof(sof));
		write(1, dht, sizeof(dht));

		write(1, sos_y, sizeof(sos_y));
		copyfile(fd, 1, ysize);

		write(1, sos_u, sizeof(sos_u));
		copyfile(fd, 1, usize);

		write(1, sos_v, sizeof(sos_v));
		copyfile(fd, 1, vsize);

		write(1, eoi, sizeof(eoi));
	} else {
		fprintf(stderr, "JFIF area not found\n");
		exit(1);
	}

	close(fd);
}
#endif /*CAMTOJPEG*/
