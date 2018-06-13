#include "lib/utils.h"
#include <math.h>

#ifndef _FOUR_SUBNET_H_
#define _FOUR_SUBNET_H_

#define MUTATE_RATE .03 /* The percentage chance that a parameter (weight, bias) is randomized during mutation */
#define WEIGHT_MAX 100
#define BIAS_MAX 1000
#define MAX_LAYERSIZE 50


struct l_connection {
  layer inputLayer;
  layer outputLayer;
  int **weights;
  int *biases;
  size_t size_input;
  size_t size_output;
};

typedef struct l_connection subnetwork;

subnetwork *new_subnetwork(int numInputs, int numOutputs);
/* A subnetwork is a system of two layers within a neural network.
 * It contains all the information to relate the two layers, including
 * biases and weights. */
subnetwork *rand_subnet();

subnetwork *subnet_copy(subnetwork *SN);

void fillInputNodes(subnetwork *SN, layer l);
/* Takes an input set and calculates the output. */

layer getOutput(subnetwork *SN);

void setWeight(subnetwork *SN, int inIndex, int outIndex, int weight);
/* The index parameters are indexes to the node in the 
   layer of the subnetwork */

void randomWeights(subnetwork *SN);
/* Assigns random weights to the SN connections. */

void setBias(subnetwork *SN, int index, int bias);
/* Index is the index of the desired node in the output layer */

void randomBiases(subnetwork *SN);
/* Assigns random biases to the SN output nodes. */

void feedForward(subnetwork *SN);
/* Calculates output layer based on given weights and biases */

void mutateSubnet(subnetwork *SN);

void free_subnet(subnetwork *SN);


#endif
