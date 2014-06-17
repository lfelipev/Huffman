#ifndef CODIFICAR_H
#define CODIFICAR_H

#include<stdio.h>
#include<QString>


class Codificar
{
public:
    Codificar();
    void compressFile();
    void huffmanEncode(const char *inputFile);
    unsigned int inputFileSize(FILE *src);
    unsigned int calcNumFreq(unsigned int *freqList);
    void outputFilePath(const char *path, char *outputPath, const char *fileExtension);
private:
    QString inputFileName; // String com o nome do Arquivo de Entrada
    QString inputFilePath; // String com o diretório do Arquivo de Entrada

    /** Nó da Árvore de Huffman **/
    struct HuffNode {
        unsigned char chr;
        unsigned int freq;
        bool leaf;
        HuffNode * next;
        HuffNode * left;
        HuffNode * right;
    };

    /** Calcula a frequência de cada caractere listado **/
    void calcCharFreq(FILE *src, unsigned int *freqList);

    /** Cria uma lista dos Nós de Huffman em ordem crescente **/
    void buildNodeList(HuffNode **nodeList, unsigned int *freqList);

    /** Função auxiliar: Adciona o novo nó(newNode) na lista em ordem crescente **/
    void addToNodeList(HuffNode **nodeList, HuffNode *newNode);
};

#endif // CODIFICAR_H
