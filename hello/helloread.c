/*
 * helloread.c
 */

#include <err.h>
#include <libpmemobj.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "layout.h"

int
main(int argc, char *argv[])
{
	if (argc != 2)
		errx(1, "usage: %s file-name", argv[0]);

	PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
	if (pop == NULL)
		err(1, "pmemobj_open");

	PMEMoid root = pmemobj_root(pop, sizeof(struct my_root));
	struct my_root *rootp = pmemobj_direct(root);

	if (rootp->len == strlen(rootp->buf))
		printf("%s\n", rootp->buf);

	pmemobj_close(pop);

	exit(0);
}
