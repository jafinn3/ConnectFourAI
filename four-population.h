#include "lib/utils.h"
#include "four-network.h"
#include "four-subnet.h"
#include "lib/four-board.h"

#ifndef _FOUR_POPULATION_H_
#define _FOUR_POPULATION_H_


struct ai_population {
  network **citizens;
  size_t size;
  size_t champ_index;
  int fitSum;
};

typedef struct ai_population population;

population *new_population(network **cits, int size);
/* A population consists of one generation of networks. 
 * It stores all the networks of a single generation along
 * with the champion. */

population *rand_pop(int size);
/* Creates a completely random population. */

/* naturalSelection() and some helper functions credited to 
 * Code Bullet on YouTube, as they were adapted from his 
 * video "How AIs learn part 2 || Coded example".
 * https://www.youtube.com/watch?v=BOZfhUcNiqk&t=475s */
population *naturalSelection(population *P);
/* Selects the networks that will move on to the next generation. */

void reset_fitness(population *P);

network *getAverageCitizen(population *P);

void calcFitnessSum(population *P);
/* Calculates the total fitness of a population, used to select
 * parents. */

network *selectParent(population *P);
/* Selects a member of the population to reproduce for the next generation. */

network *getChild(network *N);
/* Takes a parent and essentially returns an exact copy. The child is mutated
 * before moving on to the next generation, unless its parent is the champion */

network *getChamp(population *P);
network *getCitizen(population *P, int index);

size_t getPopSize(population *P);
void savePopulation(population *P, char *filename);

void free_pop(population *P);
#endif
