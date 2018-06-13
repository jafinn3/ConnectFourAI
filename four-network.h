#include "lib/utils.h"
#include "four-subnet.h"
#include "lib/four-board.h"
#include <math.h>


#ifndef _FOUR_NETWORK_H_
#define _FOUR_NETWORK_H_

#define WIN 2
#define TIE 1
#define LOSS 0
#define MAXFIT 3528 //For a 7x6 board  (42^2 * 2)
#define MAX_INNERLAYERS 3
#define INPUTSIZE 126 // For a 7x6 (7*6*3 for the 3 states of a space)
#define OUTPUTSIZE 7


struct neural_net {
  subnetwork **net;
  size_t numLayers;
  int outputCount;
  int numPlays;
  int fitness;
  int id;
  int wins;
  int losses;
};

typedef struct neural_net network;

network *new_net(subnetwork **nets, int size);
/* Takes an array of subnetworks with 'size' number of layers total. 
 * The subnetworks can then work together to produce a final output and
 * fitness for the genetic algorithm. Each network is one member of the 
 * population for a given generation. */

network *rand_net();
/* Creates and returns a randomized network. */

network *network_copy(network *N);

layer calcOutput(network *N, layer l);
/* The parameter 'l' is the input layer.
 * Sends the output of one subnetwork as the 
 * input of the next subnetwork in the network,
 * so that a final output can be obtained. */

void addFitness(network *N, int amount);

void updateFitness(network *N, short outcome, fboard *b);
/* Performance is judged first by whether the network 
 * has produced a win, loss, or tie. Then, it is judged
 * on how efficiently it won or how quickly it lost. */
int getFitness(network *N);
size_t getNumLayers(network *N);

void mutate(network *N);
void saveNetwork(network *net, char *filename);

void free_network(network *net);
#endif
