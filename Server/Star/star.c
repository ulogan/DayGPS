
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

void
star()
{
}

int
main(int argc, char *argv[])
{
    char *base = basename(argv[0]);

    if (strcmp(base, "star") == 0) {
        star();
    }

    exit(0);
}
