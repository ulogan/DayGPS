/* Unit test code for findDiff function 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

int
findDiff(int pos1, int pos2, int base)
{
    int diff;

    if (pos1 >= pos2) {
        diff = base - pos1 + pos2 + 1;

        if (diff == 13) {
            diff = 1;
        }
    } else {
        diff = abs(pos1 - pos2) + 1;
    }

    return diff;
}


int
main(int argc, char *argv[])
{
    //variables for test code
    int i, j, b, diff;
    b = 12;

    //test code to test the findDiff function with every possible number 
    for (i = 1; i <= b; i++) {
        for (j = 1; j <= b; j++) {
            diff = findDiff(i, j, b);
            printf("pos1 = %d, pos2 = %d, diff = %d\n", i, j, diff);
        }
    }
    
}
