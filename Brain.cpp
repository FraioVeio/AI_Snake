#include "Brain.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Brain::compute(double *inputs, double *outputs) {
    // hidden layers
    double neuronValuesPrev[neuronsPerLayer];
    double neuronValuesCurr[neuronsPerLayer];
    
    double (*iw)[inputSize][neuronsPerLayer] = (double (*)[inputSize][neuronsPerLayer]) inputWeights;
    double (*ow)[neuronsPerLayer][outputSize] = (double (*)[neuronsPerLayer][outputSize]) outputWeights;
    double (*lw)[hiddenLayers-1][neuronsPerLayer][neuronsPerLayer] = (double (*)[hiddenLayers-1][neuronsPerLayer][neuronsPerLayer]) inputWeights;
    

    /* Compute first hidden layer */
    memset(neuronValuesPrev, 0, neuronsPerLayer*sizeof(double));
    for(int i=0;i<inputSize;i++) {
        for(int h=0;h<neuronsPerLayer;h++) {
            neuronValuesPrev[h] += inputs[i] * (*iw)[i][h];
        }
    }
    for(int h=0;h<neuronsPerLayer;h++) {
        neuronValuesPrev[h] = tanh(neuronValuesPrev[h]);
    }

    /* Compute layers */
    for(int l=0;l<hiddenLayers-1;l++) {
        memset(neuronValuesCurr, 0, neuronsPerLayer*sizeof(double));
        for(int i1=0;i1<neuronsPerLayer;i1++) {
            for(int i2=0;i2<neuronsPerLayer;i2++) {
                neuronValuesCurr[i2] += neuronValuesPrev[i1] * (*lw)[l][i1][i2];
            }
        }
        for(int i=0;i<neuronsPerLayer;i++) {
            neuronValuesCurr[i] = tanh(neuronValuesPrev[i]);
        }
        memcpy(neuronValuesPrev, neuronValuesCurr, neuronsPerLayer*sizeof(double));
    }

    /* Compute output */
    memset(outputs, 0, outputSize*sizeof(double));
    for(int o=0;o<outputSize;o++) {
        for(int i=0;i<neuronsPerLayer;i++) {
            outputs[o] += neuronValuesPrev[i] * (*ow)[i][o];
        }
    }
    for(int o=0;o<outputSize;o++) {
        outputs[o] = tanh(outputs[o]);
    }

}

genome_t Brain::getGenome() {
    unsigned long gs = (inputSize*neuronsPerLayer + outputSize*neuronsPerLayer + (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer)*sizeof(double);
    
    double *genome = (double*) malloc(gs);

    unsigned long offset = 0;
    memcpy(genome, inputWeights, inputSize*neuronsPerLayer*sizeof(double));
    offset += inputSize*neuronsPerLayer;

    memcpy(&genome[offset], outputWeights, outputSize*neuronsPerLayer*sizeof(double));
    offset += outputSize*neuronsPerLayer;

    memcpy(&genome[offset], layerWeights, (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer * sizeof(double));
    genome_t genomes;
    genomes.genome = genome;
    genomes.size = gs;
    return genomes;
}

void Brain::setGenome(genome_t genomes) {
    double *genome = genomes.genome;

    unsigned long offset = 0;
    memcpy(inputWeights, genome, inputSize*neuronsPerLayer*sizeof(double));
    offset += inputSize*neuronsPerLayer;

    memcpy(outputWeights, &genome[offset], outputSize*neuronsPerLayer*sizeof(double));
    offset += outputSize*neuronsPerLayer;

    memcpy(layerWeights, &genome[offset], (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer * sizeof(double));
}

Brain::Brain(int inputSize, int outputSize, int hiddenLayers, int neuronsPerLayer) {
    this->inputSize = inputSize;
    this->outputSize = outputSize;
    this->hiddenLayers = hiddenLayers;
    this->neuronsPerLayer = neuronsPerLayer;

    inputWeights = (double*) calloc(inputSize*neuronsPerLayer, sizeof(double));
    outputWeights = (double*) calloc(outputSize*neuronsPerLayer, sizeof(double));
    layerWeights = (double*) calloc((hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer, sizeof(double));
}

Brain::~Brain() {
    free(inputWeights);
    free(outputWeights);
    free(layerWeights);
}