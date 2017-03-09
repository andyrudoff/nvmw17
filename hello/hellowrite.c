/*
 * hellowrite.c
 */

#include <err.h>
#include <libpmemobj.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "layout.h"

#define HELLOSTRING "Hello, pmem!"

int
main(int argc, char *argv[])
{
	if (argc != 2)
		errx(1, "usage: %s file-name", argv[0]);

	PMEMobjpool *pop = pmemobj_create(argv[1], LAYOUT_NAME,
				PMEMOBJ_MIN_POOL, 0666);

	if (pop == NULL)
		err(1, "pmemobj_create");

	PMEMoid root = pmemobj_root(pop, sizeof(struct my_root));
	struct my_root *rootp = pmemobj_direct(root);

	rootp->len = strlen(HELLOSTRING);
	pmemobj_persist(pop, &rootp->len, sizeof(rootp->len));

	pmemobj_memcpy_persist(pop, rootp->buf, HELLOSTRING, rootp->len);

	pmemobj_close(pop);

	exit(0);
}
