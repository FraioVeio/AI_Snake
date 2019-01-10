#include "Evolution.hpp"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

void Evolution::evolve(int podio, float mutationFactor) {
    int orderi[population];
    for(int i=0;i<population;i++) {
        orderi[i] = i;
    }

    /* Ordina migliori */
    int temp=0;  //Variabile di supporto
    for(int n1=0;n1<population;n1++){
        for(int n2=0;n2<population-n1-1;n2++){
            if(results[orderi[n2]]<results[orderi[n2+1]]){  //Scambio valori
                temp=orderi[n2];
                orderi[n2]=orderi[n2+1];
                orderi[n2+1]=temp;
			}
		}
	}

    int podiocount = 0;
    float *genomes[podio];
    for(int i=0;i<podio;i++) {
        genomes[i] = brains[orderi[i]]->getGenome().genome;
    }

    for(int i=0;i<population;i++) {
        float genome[genomeSize];
        for(int y=0;y<genomeSize;y++) {
            float rf = ((float)random()/(float)RAND_MAX)*2-1;
            if(rf < mutationFactor) {
                genome[y] = ((float)random()/(float)RAND_MAX)*100-50;
            } else {
                genome[y] = genomes[podiocount++][y];
                if(podiocount >= podio)
                    podiocount = 0;
            }
        }
    }
}

Brain *Evolution::getBrain(int index) {
    return brains[index];
}

int Evolution::getBestBrainIndex() {
    float max = -1;
    int ind = -1;

    for(int i=0;i<population;i++) {
        if(results[i]>max) {
            max = results[i];
            ind = i;
        }
    }

    return ind;
}

void Evolution::setResult(int index, float result) {
    results[index] = result;
}

Evolution::Evolution(int population, int inputs, int outputs, int hiddenLayers, int neuronsPerLayer) {
    this->population = population;
    genomeSize = inputs*neuronsPerLayer + outputs*neuronsPerLayer + (hiddenLayers-1) * neuronsPerLayer * neuronsPerLayer;

    brains = (Brain**) calloc(population, sizeof(Brain*));

    srand (time(NULL));

    float genome[genomeSize];

    for(int i=0;i<population;i++) {
        brains[i] = new Brain(inputs, outputs, hiddenLayers, neuronsPerLayer);
        for(int y=0;y<genomeSize;y++) {
            genome[y] = ((float)random()/(float)RAND_MAX)*100-50;
        }
        genome_t gs;
        gs.genome = genome;
        gs.size = genomeSize;
        brains[i]->setGenome(gs);
    }


    results = (float*) calloc(population, sizeof(float));
}

Evolution::~Evolution() {
    free(brains);
    free(results);
}