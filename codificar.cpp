#include "codificar.h"
#include<QFileInfo>
#include<QDebug>

Codificar::Codificar() {
    QFileInfo file("../Huffman/in.txt");
    inputFilePath = file.filePath();
    inputFileName = file.fileName();
}

void Codificar::outputFilePath(const char *path, char *outputPath, const char *fileExtension) {
    int i;
    const int pathLength = strlen(path);
    for (i=0; i<pathLength-5; ++i) {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension);
}

unsigned int Codificar::inputFileSize(FILE *src) {
    unsigned int fileSize = 0;
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) { //** Função fgetc para buscar caracteres diferentes de EOF
        fileSize++;
    }
    rewind(src); //** Retorna o cursor(c) ao começo do arquivo

    return fileSize;
}

void Codificar::calcCharFreq(FILE *src, unsigned int *freqList) {
    unsigned int c; // Cursor

    while ((c = fgetc(src)) != EOF) { //Lista os caracteres até o fim do arquivo
        freqList[c]++;
    }
    rewind(src);
}

unsigned int Codificar::calcNumFreq(unsigned int *freqList) {
    unsigned int i;
    unsigned int charFreq = 0;

    for(i=0; i<256; ++i) {
        if(freqList[i] > 0) { //Conta cada vez que um caractere aparece
            charFreq++;
        }
    }

    return charFreq;
}

void Codificar::buildNodeList(HuffNode **nodeList, unsigned int *freqList) {
    unsigned int i;
    HuffNode *newNode;

    for(i=0; i<256; ++i) {
        if(freqList[i] > 0) { //Um nó será criado sempre que um caractere dentre os 256 aparecer
            newNode = (HuffNode *)calloc(1, sizeof(HuffNode)); //Aloca um espaço na memória para o nó do caractere
            newNode->chr = i; //Caractere em questão
            newNode->freq = freqList[i]; //Quantidade em que o caractere em questão aparece
            newNode->next = NULL;
            newNode->left = NULL; //Filho da esquerda
            newNode->right = NULL; //Filho da Direita
            newNode->leaf = true; //Cada novo nó que entra na lista será considerado uma folha

            addToNodeList(nodeList, newNode);
        }
    }
}

void Codificar::addToNodeList(HuffNode **nodeList, HuffNode *newNode) {
    HuffNode *prevNode = NULL; //Último Nó
    HuffNode *currNode = *nodeList; //currNode apontará para o conteúdo da nodeList;

    while (currNode != NULL && currNode->freq < newNode->freq) {
        prevNode = currNode; //O Nó anterior será o nó atual
        currNode = prevNode->next; //O next do caractere em questão será o menor caractere anterior
    }

    newNode->next = currNode; //Faz o 'next' do novo nó apontar para a nodeList;

    if(prevNode == NULL) { //Loop inicial
        *nodeList = newNode; //A nodeList será composta pelo primeiro nó de caractere;
    }
    else { //Loops restantes
        prevNode->next = newNode; //Faz o último nó sempre apontar para o novo nó
    }
}

void Codificar::compressFile() {
    QByteArray byteArray1 = inputFilePath.toUtf8();
    const char *inputFile = byteArray1.constData();

    huffmanEncode(inputFile);
}


void Codificar::huffmanEncode(const char *inputFile) {
    /** Ler o arquivo source **/
    FILE *src = fopen(inputFile, "rb");

    /** Abre o destino do arquivo **/
    char outputPath[1000];
    const char *fileExtension = "2.txt";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE *dest = fopen(outputPath, "wb");

    /** Verificação da existência do arquivo **/
    if (src == NULL || dest == NULL) {
        qDebug() << "Nao foi possível encontrar o arquivo de origem!";
        exit(EXIT_FAILURE);
    }

    /** Pega o tamanho do Arquivo Source **/
    unsigned int fileSize;
    fileSize = inputFileSize(src);

    /** Faz uma lista de ocorrência de caracteres **/
    unsigned int *freqList;
    freqList = (unsigned int *)calloc(256, sizeof(unsigned int));
    calcCharFreq(src, freqList);

    /** Calcula número de ocorrências dos caracteres listados **/
    unsigned int charFreq;
    charFreq = calcNumFreq(freqList);

    /** **/
    HuffNode *nodeList = NULL;
    buildNodeList(&nodeList, freqList);

    /** Testes **/
    qDebug() << fileSize;
    qDebug() << charFreq;

    /**unsigned int c;
    while ((c = fgetc(src)) != EOF) {
        qDebug() << freqList[5];
    }
    rewind(src);
    char c;
    for(int i=0; i<256; ++i) {
        if(freqList[i] > 0) {
            c=i;
            qDebug() << "DEC. ASCII: " << i << "CHAR: " << c << "Frequencia: " << freqList[i];
            fwrite(&c, sizeof(char), sizeof(c), dest);
        }
    }*/

    /** Fecha o arquivo **/
    fclose(src);
    fclose(dest);
}
