#include "Brain.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void Brain::compute(double *inputs, double *outputs) {
    
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