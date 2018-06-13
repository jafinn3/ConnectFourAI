#include "lib/utils.h"
#include "four-population.h"
#include "four-network.h"
#include "four-subnet.h"
#include "lib/four-board.h"


population *new_population(network **cits, int size) {
#ifdef DEBUG
  assert(cits != NULL);
#endif
  population *ret = malloc(sizeof(population));
  memCheck((void *)ret);
  ret->citizens = calloc(sizeof(network *), size);
  memCheck((void *)ret->citizens);
#ifdef DEBUG
  //fprintf(stderr, "New population! Ids:\n");
#endif
  int max = -10000;
  ret->champ_index = 0;
  for(int a = 0; a < size; a++) {
    ret->citizens[a] = network_copy(cits[a]);
#ifdef DEBUG
    //fprintf(stderr, "%d\n", cits[a]->id);
#endif
    if(max < getFitness(ret->citizens[a])) {
      max = getFitness(ret->citizens[a]);
      ret->champ_index = a;
      //printf("max fit: %d\n", max);
    }
  }
  ret->size = size;
  calcFitnessSum(ret);
  return ret;
}

void reset_fitness(population *P) {
  for(unsigned int a = 0; a < P->size; a++) {
    P->citizens[a]->fitness = 100;
    P->citizens[a]->numPlays = 0;
    P->citizens[a]->outputCount = 0;
  }
  calcFitnessSum(P);
}

population *rand_pop(int size) {
#ifdef DEBUG
  assert(size >= 0);
#endif
  network **cits = calloc(sizeof(network *), size);
  memCheck((void *)cits);
  for(int a = 0; a < size; a++) {
    cits[a] = rand_net();
    memCheck((void *) cits[a]);
  }
   
  population *ret = new_population(cits, size);
  return ret;
}

/* naturalSelection() and some helper functions credited to 
 * Code Bullet on YouTube, as they were adapted from his 
 * video "How AIs learn part 2 || Coded example".
 * https://www.youtube.com/watch?v=BOZfhUcNiqk&t=475s */
population *naturalSelection(population *P) {
  network **cits = calloc(sizeof(network *), P->size);
  memCheck((void *)cits);
  calcFitnessSum(P);
#ifdef DEBUG
  fprintf(stderr, "Fitness sum before natural selection: %d\n", P->fitSum);
  fprintf(stderr, "Copying champion into new network. Id:%d Wins:%d Losss:%d avg output:%f fitness:%d\n", getChamp(P)->id, getChamp(P)->wins, 
    getChamp(P)->losses, (float)(getChamp(P)->outputCount) / (float)(getChamp(P)->numPlays), getChamp(P)->fitness);
#endif
  cits[0] = network_copy(getChamp(P));
  for(unsigned int a = 1; a < P->size; a++) {
    network *parent = selectParent(P);
    network *child = getChild(parent);
    mutate(child);
#ifdef DEBUG
    assert(child != NULL);
    //printf("fitness: %d\n", getFitness(child));
#endif
    cits[a] = child;
    
  }
#ifdef DEBUG
  fprintf(stderr, "Giving birth to the new population!\n");
#endif
  population *ret = new_population(cits, P->size);
  return ret;
}

void calcFitnessSum(population *P) {
  int i = 0;
  int maxFit = 0;
  int sum = 0;
  for(unsigned int a = 0; a < P->size; a++) {
    int fit = getFitness(P->citizens[a]);
    if(fit > maxFit) {
      maxFit = fit;
      i = a;
    }
    sum += fit;
  }
  P->champ_index = i;
  P->fitSum = sum;
}

network *selectParent(population *P) {
  int runningSum = 0;
  int rand = rand_lim(P->fitSum); 
  for(unsigned int a = 0; a < P->size; a++) {
    runningSum += getFitness(P->citizens[a]);
    if(runningSum >= rand)
      return P->citizens[a];
  }
  return P->citizens[P->size - 1];
}

network *getChild(network *N) {
#ifdef DEBUG
  assert(N != NULL);
#endif
  network *copy = network_copy(N);
  copy->id = rand_lim(100);
  return copy;
}

network *getCitizen(population *P, int index) {
  return P->citizens[index];
}

network *getChamp(population *P) {
  return P->citizens[P->champ_index];
}

network *getAverageCitizen(population *P) {
  float avg_fit = (float)P->fitSum / (float)P->size;
  int dist = 2^31;
  int i = 0;
  for(unsigned int a = 0; a < P->size; a++) {
    int fit = getFitness(P->citizens[a]);
    if(abs(fit - avg_fit) < dist) {
      dist = abs(fit - avg_fit);
      i = a;
    }
  }
  return P->citizens[i];
}

size_t getPopSize(population *P) {
  return P->size;
}

void savePopulation(population *P, char *filename) {
  FILE *f = fopen(filename, "w\0");
#ifdef DEBUG
  fprintf(stderr, "here");
#endif
  fprintf(f, "---Population save data---\n");
  for(unsigned int n = 0; n < P->size; n++) {
    network *net = P->citizens[n];
    fprintf(f, "Network %d:\n", (int)n);
    fprintf(f, "numLayers: %d\n", (int)net->numLayers);
    fprintf(f, "outputCount: %d\n", (int)net->outputCount);
    fprintf(f, "numPlays: %d\n", (int)net->numPlays);
    fprintf(f, "fitness: %d\n", (int)net->fitness);
    for(unsigned int sn = 0; sn < getNumLayers(net) - 1; sn++) {
      subnetwork *subnet = net->net[sn];
      fprintf(f, "Subnet %d:\n", (int)sn);
      fprintf(f, "Input size: %d\n", (int)subnet->size_input);
      fprintf(f, "Output size: %d\n",(int) subnet->size_output);

      /* Print weights */
      for(unsigned int oc = 0; oc < subnet->size_output; oc++) {
        for(unsigned int ic = 0; ic < subnet->size_input; ic++) {
          fprintf(f, "%d ", (int)subnet->weights[oc][ic]);
        }
        fprintf(f, "\n");
      }

      /* Print biases */
      for(unsigned int a = 0; a < subnet->size_output; a++) 
        fprintf(f, "%d ", (int)subnet->biases[a]);
    }
  }
}

void free_pop(population *P) {
  for(unsigned int a = 0; a < P->size; a++) {
#ifdef DEBUG
    assert(P->citizens[a] != NULL);
#endif
    free_network(P->citizens[a]); 
  }

  free(P->citizens);
  free(P);
}
