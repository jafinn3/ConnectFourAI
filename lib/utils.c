#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include <math.h>

void assert(bool b) {
  if(!b) {
    fprintf(stderr, "Assert failed. ");
    abort();
  }
}

void assert_msg(bool b, char *msg) {
  if(!b) {
    fprintf(stderr, "%s", msg);
    abort();
  }
}

void memCheck(void *ptr) {
  assert_msg(ptr != NULL, "Heap is full! Exiting...\0");
}

/* Credit to Jerry Coffin on stackoverflow.com 
 * for this random function.
https://stackoverflow.com/questions/2999075/generate-a-random-number-within-range/2999130#2999130 */
int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive. */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}
