#include "lib/utils.h"
#include "four-network.h"
#include "four-subnet.h"
#include "lib/four-board.h"
#include <math.h>


network *new_net(subnetwork **nets, int size) {
/* Takes an array of subnetworks with 'size' number of layers total. 
 * The subnetworks can then work together to produce a final output and
 * fitness for the genetic algorithm. Each network is one member of the 
 * population for a given generation. */
  network *N = malloc(sizeof(network));
  memCheck((void *)N);
  
  N->net = calloc(sizeof(subnetwork *), size);
  memCheck((void *)N->net);
  for(int a = 0; a < size - 1; a++) {
#ifdef DEBUG
    assert(nets[a] != NULL);
#endif
    N->net[a] = subnet_copy(nets[a]);
  }

  N->numLayers = size;
  N->fitness = 100;
  N->outputCount = 0;
  N->numPlays = 0;
  N->id = rand_lim(100 * 2);
  N->wins = 0;
  N->losses = 0;
  return N;
}

network *network_copy(network *N) {
  subnetwork **nets = calloc(sizeof(subnetwork *), N->numLayers - 1);
  memCheck((void *)nets);
  for(unsigned int a = 0; a < N->numLayers - 1; a++) 
    nets[a] = subnet_copy(N->net[a]);
  network *ret = new_net(nets, N->numLayers);
  ret->fitness = N->fitness;
  ret->numPlays = N->numPlays;
  ret->outputCount = N->outputCount;
  ret->id = N->id;
  ret->wins = N->wins;
  ret->losses = N->losses;
  return ret;
}

network *rand_net() {
  int size = rand_lim(MAX_INNERLAYERS) + 2;
  subnetwork **nets = calloc(sizeof(subnetwork *), size - 1);
  memCheck((void *)nets);

  int numInputs = rand_lim(MAX_LAYERSIZE) + 7;
  int numOutputs = rand_lim(MAX_LAYERSIZE) + 7;
  /* Randomize the inner subnetworks. */
  for(int a = 1; a < size - 1; a++) {
    subnetwork *SN = rand_subnet(numInputs, numOutputs);
    numInputs = numOutputs;
    numOutputs = rand_lim(MAX_LAYERSIZE) + 7;
    nets[a] = SN;
  }
  
  if(size == 2) {
    /* If the size is two, we just have one subnetwork. This
     * is a special case and must be treated as such.  */
    subnetwork *SN = rand_subnet(INPUTSIZE, OUTPUTSIZE);
    nets[0] = SN;
  }
  
  else {
    /* Create a subnetwork to hold the input layer and randomize its
     * connections to the next subnetwork. */
    subnetwork *input = rand_subnet(INPUTSIZE, nets[1]->size_input);
    nets[0] = input;

    subnetwork *output = rand_subnet(nets[size-3]->size_output, OUTPUTSIZE);
    nets[size - 2] = output;
  }
  
  network *ret = new_net(nets, size);
  return ret;
}

size_t getNumLayers(network *N) {
  return N->numLayers;
}

layer calcOutput(network *N, layer l) {
/* The parameter 'l' is the input layer.
 * Sends the output of one subnetwork as the 
 * input of the next subnetwork in the network,
 * so that a final output can be obtained. */
#ifdef DEBUG
  assert_msg(N != NULL, "N is null. ");
  assert_msg(N->net[0] != NULL, "The first SN is null. ");
  assert_msg(l != NULL, "l is null. ");
#endif

  fillInputNodes(N->net[0], l);
  layer out = getOutput(N->net[0]);
  for(unsigned int a = 1; a < N->numLayers - 1; a++) {
#ifdef DEBUG
    assert_msg(N->net[a]->size_input == N->net[a - 1]->size_output, "uh oh");
#endif
    fillInputNodes(N->net[a], out);
    //free(out);
    out = getOutput(N->net[a]);
  }
  
  int sum = 0;
  for(unsigned int a = 0; a < N->net[N->numLayers - 2]->size_output; a++) 
    sum += out[a];
  N->outputCount += sum;
  N->numPlays++;
  return out;
}

void addFitness(network *N, int amount) {
  N->fitness += amount;
  /*if(N->fitness < 0)
    N->fitness = 0;*/
}

void updateFitness(network *N, short outcome, fboard *b) {
/* Performance is judged first by the number of choices it outputs for 
 * a play, then by whether the network 
 * has produced a win, loss, or tie. Finally, it is judged
 * on how efficiently it won or how quickly it lost. */
  int sum = 0;
  for(unsigned int x = 0; x < getWidth(b); x++) 
    for(unsigned int y = 0; y < getHeight(b); y++) 
      if(getState(b, x, y) != 0)
        sum++;
  float avgOutput = (float)N->outputCount / (float)N->numPlays;
  /* Testing number of outputs */
  if(abs((int)((avgOutput * 100.0) - 100.0)) < 10)
    N->fitness += 100;
  else if(avgOutput - 1.0 > 0)
    N->fitness -= (int)(avgOutput * 2);
  else
    N->fitness -= 10;
  
  if(outcome == WIN)
    N->fitness += 25;
  else
    N->fitness -= 2 * (42 - sum);
  /*if(N->fitness < 0)
    N->fitness = 0;*/
}

int getFitness(network *N) {
  if(N->fitness < 0)
    return 0;
  return N->fitness;
}

void mutate(network *N) {
  for(unsigned int a = 0; a < N->numLayers - 1; a++) {
    mutateSubnet(N->net[a]);
  }
}

void saveNetwork(network *net, char *filename) {
  fprintf(stderr, "here");
  FILE *f = fopen(filename, "w\0");
  fprintf(stderr, "here");
  fprintf(f, "---Network save ---\n");
  fprintf(f, "numLayers: %d\n", (int)net->numLayers);
  fprintf(f, "outputCount: %d\n", net->outputCount);
  fprintf(f, "numPlays: %d\n", net->numPlays);
  fprintf(f, "fitness: %d\n", net->fitness);
  for(unsigned int subnet = 0; subnet < getNumLayers(net) - 1; subnet++) {
    fprintf(f, "Subnet %d:\n", (int)subnet);
    fprintf(f, "Input size: %d\n", (int)net->net[subnet]->size_input);
    fprintf(f, "Output size: %d\n", (int)net->net[subnet]->size_output);

    /* Print weights */
    for(unsigned int oc = 0; oc < net->net[subnet]->size_output; oc++) {
      for(unsigned int ic = 0; ic < net->net[subnet]->size_input; ic++) {
        fprintf(f, "%d ", (int)net->net[subnet]->weights[oc][ic]);
      }
      fprintf(f, "\n");
    }

    /* Print biases */
    for(unsigned int a = 0; a < net->net[subnet]->size_output; a++) 
      fprintf(f, "%d ", (int)net->net[subnet]->biases[a]);
  }
}

void free_network(network *net) {
#ifdef DEBUG
  assert(net != NULL);
#endif
  for(unsigned int a = 0; a < net->numLayers - 1; a++) {
#ifdef DEBUG
    assert(net->net[a] != NULL);
#endif
    free_subnet(net->net[a]);
  }
#ifdef DEBUG
  assert(net->net != NULL);
#endif
  free(net->net);
  free(net);
}
