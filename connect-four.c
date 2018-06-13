#include <stdlib.h>
#include <stdio.h>
#include "four-population.h"
#include "four-network.h"
#include "four-subnet.h"
#include "lib/four-board.h"
#include "lib/utils.h"
#include <time.h>

#define POP_SIZE 300

layer board2inputLayer(fboard *b, int me) {
#ifdef DEBUG
  assert(me == 1 || me == 2);
#endif

  int states[] = {me, 3 - me, 0};
  int width = getWidth(b);
  int height = getHeight(b);
  layer ret = calloc(sizeof(bool), 3 * width * height);
  memCheck((void *)ret);
  
  /*for(int y = 0; y < height; y++)
    for(int x = 0; x < width; x++)
      if(getState(b, x, y))*/
  for(int k = 0; k < 3; k++)
    for(int y = 0; y < height; y++) {
      for(int x = 0; x < width; x++) {
        if(getState(b, x, y) == states[k]) {
          ret[k * (width * height) + getIndex(b, x, y)] = 1;
        }
      }
    }
  
  return ret;
}

int layer2boardinput(layer output) {
  int hitCount = 0;
  for(int a = 0; a < OUTPUTSIZE; a++) 
    if(output[a] == true)
      hitCount++;
  if(hitCount == 0) 
    return rand_lim(OUTPUTSIZE - 1);
  
  
  int *indices = calloc(sizeof(int), hitCount);
  memCheck((void *)indices);
  int hitIndexer = 0;
  for(int a = 0; a < OUTPUTSIZE; a++)
    if(output[a] == true) {

#ifdef DEBUG
  assert(hitIndexer < hitCount);
#endif
      indices[hitIndexer] = a;
      hitIndexer++;
    } 
  int ret = indices[rand_lim(hitCount - 1)];
  free(indices);
  return ret;

}

network *train(int GENS) {
/* Returns the best result of training */
#ifdef DEBUG
  assert(GENS > 1);
#endif
  network *ret = NULL;
  /* Start with a completely random generation */
  population *curr_gen = rand_pop(POP_SIZE);
  network *opponent;

  for(int generation = 0; generation < GENS; generation++) {
#ifdef DEBUG
    fprintf(stderr, "Generation: %d\n", generation);
    //assert(opponent != NULL);
#endif
    for(unsigned int a = 0; a < getPopSize(curr_gen); a++) {
      //printf("player %d\n", (int)a);
      network *ai_player = getCitizen(curr_gen, a);

#ifdef DEBUG
      assert(ai_player != NULL);
      //fprintf(stderr, "Playing games for id %d...\n", ai_player->id);
#endif
      for(unsigned int i = 0; i < getPopSize(curr_gen); i++) {
        opponent = getCitizen(curr_gen, i);
        fboard *b = board_new(7, 6);
        int player = 0;
        int ai_chip = rand_lim(1) + 1;
        int penalty_count_ai = 0;
        int penalty_count_mid = 0;
        while(true) {
          layer input = board2inputLayer(b, 3 - ai_chip);
          layer output = calcOutput(opponent, input);
          int x = layer2boardinput(output);
          if(player % 2 + 1 == ai_chip) {
            input = board2inputLayer(b, ai_chip);
            output = calcOutput(ai_player, input);
            x = layer2boardinput(output);
          }

          //printf("x: %d\n", x);
          int p = play(b, x, player % 2 + 1);
          
          if(p > 0 && player % 2 + 1 == ai_chip) {
            /* win for our network! */
            updateFitness(ai_player, WIN, b);
            ai_player->wins++;
            //printf("win %d\n", getFitness(ai_player));
            break;        
          }

          else if(p > 0) {
            /* loss :( */
            ai_player->losses++;
            updateFitness(ai_player, LOSS, b);
            //printf("loss %d\n", getFitness(ai_player));
            break;        
          }
          
          else if(isFull(b)) {
            updateFitness(ai_player, TIE, b);
            //printf("tie %d\n", getFitness(ai_player));
            break;
          }

          else if(p == 0)
            player++;
          
          else if(player % 2 + 1 == ai_chip) {
            penalty_count_ai++;
          }
            
          else if(player % 2 + 1 != ai_chip) {
            penalty_count_mid++;
          }

          if(penalty_count_ai > 1) {
#ifdef DEBUG
            //fprintf(stderr, "ai %d has gotten a penalty\n", ai_player->id);
#endif
            addFitness(ai_player, -200 * POP_SIZE);
            updateFitness(ai_player, LOSS, b);
            //printf("loss %d\n", getFitness(ai_player));
            break;
          }

          if(penalty_count_mid > 2) {
            updateFitness(ai_player, TIE, b);
            //printf("ties %d\n", getFitness(ai_player));
            break;
          }
          if((int)a == -10) {
            printBoard(b);
            char c;
            (void)c;
            scanf("%c", &c);
            printf("\n");
          }
        }
      }      
    }
    /*This generation has played all its games. On to the next! */
    
    population *temp = curr_gen;
    ret = network_copy(getChamp(temp));
#ifdef DEBUG
    fprintf(stderr, "Going into natural selection.\n");
#endif
    curr_gen = naturalSelection(temp);
#ifdef DEBUG
    assert(curr_gen->citizens[0] != NULL);
#endif
    free_pop(temp);
#ifdef DEBUG
      fprintf(stderr, "ending generation.\n");
#endif
    reset_fitness(curr_gen);
  }
  
  /* Done training, save data. */
  char a;
  char *filename = calloc(sizeof(char), 20);
  memCheck(filename);
  printf("Would you like to save the champion? (y/n)\n");
  printf("Id:%d Wins:%d Losss:%d avg output:%f fitness:%d\n", ret->id, ret->wins, 
    ret->losses, (float)(ret->outputCount) / (float)(ret->numPlays), ret->fitness);
  scanf("%c", &a);
  if(a != 'n') {
    printf("Please enter a filename:\n");
    scanf("%s", filename);
    saveNetwork(ret, filename);
  }

  printf("Would you like to save the population? (y/n)\n");
  scanf("%c", &a);
  if(a != 'n') {
    printf("Please enter a filename:\n");
    scanf("%s", filename);

    savePopulation(curr_gen, filename);
  }

  return ret;
}

void playMachine(network *ai) {
  fboard *b = board_new(7, 6);
  int player = 0;
  int ai_chip = rand_lim(1) + 1;  
  int ai_penalty = 0;
  while(true) {
    printBoard(b);
    int x;
    if(player % 2 + 1 == ai_chip) {
      layer input = board2inputLayer(b, ai_chip);
      layer output = calcOutput(ai, input);
      x = layer2boardinput(output);
    }

    else 
      scanf("%d", &x);
    int p = play(b, x, player % 2 + 1);
    
    if(p > 0) {
      printf("Win for player %d!\n", player % 2 + 1);
      break;
    }

    else if(p == 0)
      player++;

    else {
      if(player % 2 + 1 == ai_chip)
        ai_penalty++;
      if(ai_penalty > 10) {
        printf("AI forfeits!\n");
        break;
      }

      printf("Column filled. Try a different one!");
    }

    //printBoard(b);
    printf("\n");
  }
  printf("Would you like to play again? (y/n)\n");
  char a;
  scanf("%c", &a);
  if(a != 'n')
    playMachine(ai);
}

int main() {
  srand(time(NULL));
  playMachine(train(5));
  return 0;
}
