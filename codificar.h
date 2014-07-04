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
    unsigned int getFileSize(FILE *src);
    unsigned int calcNumOfFreq(unsigned int *freqList);
private:
    QString inputFileName; // String com o nome do Arquivo de Entrada
    QString inputFilePath; // String com o diretório do Arquivo de Entrada

    /** Caminho de saída do Arquivo **/
   void outputFilePath(const char * path, char * outputPath, const char * fileExtension);

    /** Nó da Árvore de Huffman **/
   struct HuffNode {
       unsigned int freq;
       unsigned char charCode;
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

    /** Cabeçalho Huffman 1**/
   struct HuffHeader {
       unsigned int numOfFreq;
       unsigned int fileSize;
   };

    /** Cabeçalho com a frequências e caractere**/
   struct HuffFreq {
       unsigned int freq;
       unsigned char charCode;
   };

    /** Calcula a frequência de cada caractere listado **/
    void calcCharFreq(FILE * src, unsigned int * freqList);

    /** Cria uma lista dos Nós de Huffman em ordem crescente **/
    void buildNodeList(HuffNode ** nodeList, unsigned int * freqList);

    /** Função auxiliar: Adciona o novo nó(newNode) na lista em ordem crescente **/
    void addToNodeList(HuffNode ** nodeList, HuffNode * newNode);

    /** Cria a Árvore de Huffman a partir da Lista **/
    void buildHuffTree(HuffNode ** nodeList);

    /** Cria uma codificação **/
    bool buildHuffCode(HuffNode * treeRoot, HuffCode * hCode, unsigned char goalChar);

    /** Escreve o cabeçalho no arquivo de saída **/
    void writeHeader(FILE * dest, HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize);

    /** Escreve as frequências e os caracteres no arquivo de saída **/
    void writeFreq(FILE * dest, unsigned int * freqList, HuffFreq hFreq);

    /** Escreve o código no arquivo de saída **/
    void writeEncodedData(FILE * src, FILE * dest, HuffCode * huffCodeTable, unsigned int fileSize);

    /** Libera a memória **/
    void freeHuffTree(HuffNode *treeRoot);
};

#endif // CODIFICAR_H
