#ifndef DECODIFICAR_H
#define DECODIFICAR_H

#include <QDialog>

namespace Ui {
class Decodificar;
}

class Decodificar : public QDialog {
    Q_OBJECT
public:
    explicit Decodificar(QWidget *parent = 0);
    ~Decodificar();

    /** Pega o arquivo de origem **/
    QString getInputFileName();

    /** Função principal **/
    void huffmanDecode(const char *inputFile);

private:
    Ui::Decodificar *ui;

    /** String com o nome do inputFile **/
    QString inputFileName;

    /** Diretório de saída **/
    void outputFilePath(const char *path, char *outputPath, const char *fileExtension);

    /** Pega o nome do arquivo de saída **/
    void getOutputFileName();

    /** (Auxiliar)Mostra uma mensagem no fim da descompressão **/
    void showDoneMessage(const char *msg);

    /** Nó principal(Nó de Huffman) **/
    struct HuffNode {
        unsigned char charCode;
        unsigned int freq;
        bool leaf;
        HuffNode *next;
        HuffNode *left;
        HuffNode *right;
    };

    /** Nó de cabeçalho **/
    struct HuffHeader {
        unsigned int numOfFreq;
        unsigned int fileSize;
    };

    /** Nó contendo caracteres com suas frequências **/
    struct HuffFreq {
        unsigned int freq;
        unsigned char charCode;
    };

    /** Constroi uma lista ligada com as frequências decodificadas **/
    void buildNodeList(HuffNode **nodeList, HuffFreq *hFreq, unsigned int numOfFreq);

    /** (Função Auxiliar)Adciona o newNode na lista em ordem crescente **/
    void addToNodeList(HuffNode **nodeList, HuffNode *newNode);

    /** Constroi a Árvore com a NodeList **/
    void buildHuffTree(HuffNode **nodeList);

    /** Escreve os dados decodificados no arquivo de saída **/
    void writeDecodedData(FILE *src, FILE *dest, HuffNode *rootTree, unsigned int fileSize);

    /** Libera a memória **/
    void freeHuffTree(HuffNode *treeRoot);

public slots:
    /** Busca o arquivo de entrada **/
    void browseInputFile();

    /** Chama a função principal **/
    void decompressFile();
};

#endif // DECODIFICAR_H
