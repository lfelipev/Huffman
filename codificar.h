#ifndef CODIFICAR_H
#define CODIFICAR_H

#include <QWidget>
#include<stdio.h>
#include<QString>
#include<QDialog>

namespace Ui {
class Codificar;
}

class Codificar : public QDialog {
    Q_OBJECT
public:
    explicit Codificar(QWidget *parent = 0);
    ~Codificar();

    /** Pega o arquivo de origem **/
    QString getInputFileName();

    /** Função principal **/
    void huffmanEncode(const char* inputFile);

    /** Pega o tamanho do arquivo (em bytes) **/
    unsigned int getFileSize(FILE *src);

    /** Calcula o número de frequências **/
    unsigned int calcNumOfFreq(unsigned int *freqList);

private:
    Ui::Codificar *ui;

    /** String com o nome do inputFile **/
    QString inputFileName;

    /** Diretório de saída **/
    void outputFilePath(const char *path, char *outputPath, const char *fileExtension);

    /** (Auxiliar)Mostra uma mensagem no fim da compressão **/
    void showDoneMessage(const char *msg);

    /** Nó principal(Nó de Huffman) **/
    struct HuffNode {
        unsigned int freq;
        unsigned char charCode;
        bool leaf;
        HuffNode *next;
        HuffNode *left;
        HuffNode *right;
    };

    /** Estrutura da codificação **/
    struct HuffCode {
        unsigned char code[32];
        unsigned int length;
    };

    /** Nó de cabeçalho **/
    struct HuffHeader {
        unsigned int numOfFreq;
        unsigned int fileSize;
        const char* fileExtension;
    };

    /** Nó contendo caracteres com suas frequências **/
    struct HuffFreq {
        unsigned int freq;
        unsigned char charCode;
    };

    /** Calcula a frequência de cada caractere na lista **/
    void calcCharFreq(FILE *src, unsigned int *freqList);

    /** Constroi uma lista ligada com os nós de huffman (frequências ordenadas em ordem crescente) **/
    void buildNodeList(HuffNode **nodeList, unsigned int *freqList);

    /** (Função Auxiliar)Adciona o newNode na lista em ordem crescente **/
    void addToNodeList(HuffNode **nodeList, HuffNode *newNode);

    /** Constroi a Árvore com a NodeList **/
    void buildHuffTree(HuffNode **nodeList);

    /** Constroi o código de pesquisa para compressão **/
    bool buildHuffCode(HuffNode *treeRoot, HuffCode *hCode, unsigned char goalChar);

    /** Escreve o cabeçalho no arquivo de saída **/
    void writeHeader(FILE *dest, HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize);

    /** Escreve o caractere e a freqquência no arquivo de saída **/
    void writeFreq(FILE *dest, unsigned int *freqList, HuffFreq hFreq);

    /** Escreve a codificação no arquivo de saída **/
    void writeEncodedData(FILE *src, FILE *dest, HuffCode *huffCode, unsigned int fileSize);

    /** Libera a memória **/
    void freeHuffTree(HuffNode * treeRoot);

public slots:

    /** Busca o arquivo de entrada **/
    void browseInputFile();

    /** Chama a função principal **/
    void compressFile();
};

#endif // CODIFICAR_H
