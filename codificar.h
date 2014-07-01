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

    /** Codificação de Huffman **/
    struct HuffCode {
        unsigned char code[32];
        unsigned int length;
    };

    /** Calcula a frequência de cada caractere listado **/
    void calcCharFreq(FILE *src, unsigned int *freqList);

    /** Cria uma lista dos Nós de Huffman em ordem crescente **/
    void buildNodeList(HuffNode **nodeList, unsigned int *freqList);

    /** Função auxiliar: Adciona o novo nó(newNode) na lista em ordem crescente **/
    void addToNodeList(HuffNode **nodeList, HuffNode *newNode);

    /** Cria a Árvore de Huffman a partir da Lista **/
    void buildHuffTree(HuffNode **nodeList);

    /** **/
    bool buildHuffCode(HuffNode *treeRoot, HuffCode *hCode, unsigned char currChar);
};

#endif // CODIFICAR_H
