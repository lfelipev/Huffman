#ifndef DECODIFICAR_H
#define DECODIFICAR_H

#include<stdio.h>
#include<QString>

class decodificar
{
public:
    decodificar();
    void decompressFile();
    void huffmanDecode(const char *inputFile);
private:
    QString inputFileName; // String com o nome do Arquivo de Entrada
    QString inputFilePath; // String com o diretório do Arquivo de Entrada

    /** Caminho de Saída do Arquivo **/
    void outputFilePath(const char *path, char *outputPath, const char *fileExtension);

    /** Nó da Árvore de Huffman **/
    struct HuffNode {
        unsigned char charCode;
        unsigned int freq;
        bool leaf;
        HuffNode *next;
        HuffNode *left;
        HuffNode *right;
    };

    /** Cabeçalho de Huffman(Nó) **/
    struct HuffHeader {
        unsigned int charFreq;
        unsigned int fileSize;
    };

    /** Nós com a frequência e o caractere **/
    struct HuffFreq {
        unsigned int freq;
        unsigned int chr;
    };

    /** Cria uma lista com as frequências decodificadas **/
    void buildNodeList(HuffNode **nodeList, HuffFreq *hFreq, unsigned int charFreq);

    /** Função auxiliar: Adciona o novo nó(newNode) na lista **/
    void addToNodeList(HuffNode **nodeList, HuffNode *newNode);

    /** Cria a Árvore de Huffman **/
    void buildHuffTree(HuffNode **nodeList);

    /** **/
    void writeDecodedData(FILE *src, FILE *dest, HuffNode *rootTree, unsigned int fileSize);

    /** **/
    void freeHuffTree(HuffNode *treeRoot);
};

#endif // DECODIFICAR_H
