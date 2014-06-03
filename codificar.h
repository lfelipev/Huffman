#ifndef CODIFICAR_H
#define CODIFICAR_H

#include<stdio.h>
#include<QString>


class Codificar
{
public:
    Codificar();
    void comprimirArquivo();
    void huffmanCodificar(const char *inputFile);
    unsigned int tamanhoArquivo(FILE *src);
    void calcFreqChar(FILE *src, unsigned int *freqList);
    unsigned int calcNumFreq(unsigned int *freqList);
    void outputFilePath(const char *path, char *outputPath, const char *fileExtension);
private:
    QString inputFileName; // String com o nome do Arquivo de Entrada
    QString inputFilePath; // String com o diretório do Arquivo de Entrada

};

#endif // CODIFICAR_H
