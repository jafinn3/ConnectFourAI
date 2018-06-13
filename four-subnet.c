#include "lib/utils.h"
#include <math.h>
#include "four-subnet.h"

/* Defined in the header file, provided here for reference
#define MUTATE_RATE
#define WEIGHT_MAX
#define BIAS_MAX 
#define MAX_LAYERSIZE */


subnetwork *new_subnetwork(int numInputs, int numOutputs) {
  subnetwork *SN = malloc(sizeof(subnetwork));
  memCheck((void *)SN);

  SN->size_input = numInputs;
  SN->size_output = numOutputs;

  SN->inputLayer = calloc(sizeof(bool), numInputs);
  memCheck((void *)SN->inputLayer);
  SN->outputLayer = calloc(sizeof(bool), numOutputs);
  memCheck((void *)SN->outputLayer);

  SN->weights = calloc(sizeof(int *), numOutputs);
  memCheck((void *)SN->weights);
  for(int a = 0; a < numOutputs; a++)
    SN->weights[a] = calloc(sizeof(int), numInputs);
  memCheck((void *)*(SN->weights));
  
  SN->biases = calloc(sizeof(int), numOutputs);
  memCheck((void *)SN->biases);
  
  return SN;
}

subnetwork *rand_subnet(int numInputs, int numOutputs) {
  subnetwork *ret = new_subnetwork(numInputs, numOutputs);
  randomBiases(ret);  
  randomWeights(ret);
  return ret;
}

subnetwork *subnet_copy(subnetwork *SN) {
  subnetwork *copy = new_subnetwork(SN->size_input, SN->size_output);
  /* Biases... */
  for(unsigned int a = 0; a < SN->size_output; a++) 
    setBias(copy, a, SN->biases[a]);
  
  /* Weights... */
  for(unsigned int oc = 0; oc < SN->size_output; oc++) {
    for(unsigned int ic = 0; ic < SN->size_input; ic++) {
      setWeight(copy, ic, oc, SN->weights[oc][ic]);
    }
  }
  
  fillInputNodes(copy, SN->inputLayer);
  return copy;
}

void fillInputNodes(subnetwork *SN, layer l) {
/* Takes an input set and calculates the output. */
#ifdef DEBUG
  assert_msg(SN != NULL, "SN is null. ");
  assert_msg(l != NULL, "l is null. ");
#endif
  for(unsigned int a = 0; a < SN->size_input; a++) {
    SN->inputLayer[a] = l[a];
  }
  feedForward(SN);
}

void setWeight(subnetwork *SN, int inIndex, int outIndex, int weight) {
#ifdef DEBUG
  assert(SN != NULL);
  assert(0 <= inIndex && inIndex < (int)SN->size_input);
  assert(0 <= outIndex && outIndex < (int)SN->size_output);
#endif
  SN->weights[outIndex][inIndex] = weight;
}

void randomWeights(subnetwork *SN) {
  for(unsigned int oc = 0; oc < SN->size_output; oc++) {
    for(unsigned int ic = 0; ic < SN->size_input; ic++) {
      setWeight(SN, ic, oc, rand_lim(WEIGHT_MAX) * pow(-1, rand_lim(1)));
    }
  }
}

void setBias(subnetwork *SN, int index, int bias) {
#ifdef DEBUG
  assert(SN != NULL);
  assert(0 <= index && index < (int)SN->size_output);
#endif
  SN->biases[index] = bias;
}

void randomBiases(subnetwork *SN) {
  for(unsigned int a = 0; a < SN->size_output; a++) 
    setBias(SN, a, rand_lim(BIAS_MAX) * pow(-1, rand_lim(1)));
}

void feedForward(subnetwork *SN) {
  for(unsigned int oc = 0; oc < SN->size_output; oc++) {
    int sum = SN->biases[oc];
    for(unsigned int ic = 0; ic < SN->size_input; ic++) {
      sum += SN->weights[oc][ic] * SN->inputLayer[ic];
    } 
    SN->outputLayer[oc] = (sum >= 0) ? 1:0;
  }
}

layer getOutput(subnetwork *SN) {
  /* Requires fillInputNodes() called first */
  return SN->outputLayer;
}

void mutateSubnet(subnetwork *SN) {
  for(unsigned int oc = 0; oc < SN->size_output; oc++) {
    for(unsigned int ic = 0; ic < SN->size_input; ic++) {
      if(rand_lim(100) <= (int)(MUTATE_RATE * 100))
        SN->weights[oc][ic] = rand_lim(WEIGHT_MAX);
    }
  }

  for(unsigned int oc = 0; oc < SN->size_output; oc++) {
    if(rand_lim(100) <= (int)(MUTATE_RATE * 100))
      SN->biases[oc] = rand_lim(BIAS_MAX);
  }
}

void free_subnet(subnetwork *SN) {
  assert(SN->inputLayer != NULL);
  free(SN->inputLayer);
  assert(SN->outputLayer != NULL);
  free(SN->outputLayer);
  free(*(SN->weights));
  free(SN->weights);
  free(SN->biases);
  assert(SN != NULL);
  free(SN);
}


