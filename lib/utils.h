#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef _UTILS_H_
#define _UTILS_H_


typedef bool node;
typedef node* layer;

void assert(bool b);
void assert_msg(bool b, char *msg);
void memCheck(void *ptr);
int rand_lim(int limit);
#endif
