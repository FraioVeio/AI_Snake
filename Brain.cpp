#include "Brain.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

void Brain::compute(float *inputs, float *outputs) {
    // hidden layers
    float neuronValuesPrev[neuronsPerLayer];
    float neuronValuesCurr[neuronsPerLayer];
    
    float (*iw)[inputSize][neuronsPerLayer] = (float (*)[inputSize][neuronsPerLayer]) inputWeights;
    float (*ow)[neuronsPerLayer][outputSize] = (float (*)[neuronsPerLayer][outputSize]) outputWeights;
    float (*lw)[hiddenLayers-1][neuronsPerLayer][neuronsPerLayer] = (float (*)[hiddenLayers-1][neuronsPerLayer][neuronsPerLayer]) inputWeights;
    

    /* Compute first hidden layer */
    memset(neuronValuesPrev, 0, neuronsPerLayer*sizeof(float));
    for(int i=0;i<inputSize;i++) {
        for(int h=0;h<neuronsPerLayer;h++) {
            neuronValuesPrev[h] += inputs[i] * (*iw)[i][h];
        }
    }
    for(int h=0;h<neuronsPerLayer;h++) {
        //neuronValuesPrev[h] = tanh(neuronValuesPrev[h]);
    }

    /* Compute layers */
    for(int l=0;l<hiddenLayers-1;l++) {
        memset(neuronValuesCurr, 0, neuronsPerLayer*sizeof(float));
        for(int i1=0;i1<neuronsPerLayer;i1++) {
            for(int i2=0;i2<neuronsPerLayer;i2++) {
                neuronValuesCurr[i2] += neuronValuesPrev[i1] * (*lw)[l][i1][i2];
            }
        }
        for(int i=0;i<neuronsPerLayer;i++) {
            //neuronValuesCurr[i] = tanh(neuronValuesPrev[i]);
        }
        memcpy(neuronValuesPrev, neuronValuesCurr, neuronsPerLayer*sizeof(float));
    }

    /* Compute output */
    memset(outputs, 0, outputSize*sizeof(float));
    for(int o=0;o<outputSize;o++) {
        for(int i=0;i<neuronsPerLayer;i++) {
            outputs[o] += neuronValuesPrev[i] * (*ow)[i][o];
        }
    }
    for(int o=0;o<outputSize;o++) {
        //outputs[o] = tanh(outputs[o]);
    }

}

genome_t Brain::getGenome() {
    unsigned long gs = getGenomeArraySize()*sizeof(float);
    
    float *genome = (float*) malloc(gs);

    unsigned long offset = 0;
    memcpy(genome, inputWeights, inputSize*neuronsPerLayer*sizeof(float));
    offset += inputSize*neuronsPerLayer;

    memcpy(&genome[offset], outputWeights, outputSize*neuronsPerLayer*sizeof(float));
    offset += outputSize*neuronsPerLayer;

    memcpy(&genome[offset], layerWeights, (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer * sizeof(float));
    genome_t genomes;
    genomes.genome = genome;
    genomes.size = gs;
    return genomes;
}

void Brain::setGenome(genome_t genomes) {
    float *genome = genomes.genome;

    unsigned long offset = 0;
    memcpy(inputWeights, genome, inputSize*neuronsPerLayer*sizeof(float));
    offset += inputSize*neuronsPerLayer;

    memcpy(outputWeights, &genome[offset], outputSize*neuronsPerLayer*sizeof(float));
    offset += outputSize*neuronsPerLayer;

    memcpy(layerWeights, &genome[offset], (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer * sizeof(float));
}

int Brain::getGenomeArraySize() {
    return (inputSize*neuronsPerLayer + outputSize*neuronsPerLayer + (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer);
    
}

Brain::Brain(int inputSize, int outputSize, int hiddenLayers, int neuronsPerLayer) {
    this->inputSize = inputSize;
    this->outputSize = outputSize;
    this->hiddenLayers = hiddenLayers;
    this->neuronsPerLayer = neuronsPerLayer;

    inputWeights = (float*) calloc(inputSize*neuronsPerLayer, sizeof(float));
    outputWeights = (float*) calloc(outputSize*neuronsPerLayer, sizeof(float));
    layerWeights = (float*) calloc((hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer, sizeof(float));
}

Brain::~Brain() {
    free(inputWeights);
    free(outputWeights);
    free(layerWeights);
}