#ifndef BRAIN_HPP
#define BRAIN_HPP


typedef struct genome_s {
    int size;
    float *genome;
} genome_t;

class Brain {
    private:
        int inputSize, outputSize;
        int hiddenLayers;
        int neuronsPerLayer;
        float *inputWeights;   //Array dimensioni inputSize x neuronsPerLayer
        float *outputWeights;  //Array dimensioni neuronsPerLayer x outputSize
        float *layerWeights;   //Array dimensioni hiddenLayers-1 x neuronsPerLayer x neuronsPerLayer
        float *biases;         //Array dimensioni hiddenLayers x neuronsPerLayer
    public:
        Brain(int inputSize, int outputSize, int hiddenLayers, int neuronsPerLayer);
        ~Brain();
        genome_t getGenome();
        void setGenome(genome_t genome);
        void compute(float *inputs, float *outputs);
        int getGenomeArraySize();
    };

#endif