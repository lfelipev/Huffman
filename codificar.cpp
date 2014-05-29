#include "codificar.h"
#include<QFileInfo>
#include<QDebug>

Codificar::Codificar() {
    QFileInfo file("C:/Qt/Tools/QtCreator/bin/Huffman Compressor/HuffmanComp/in.txt");
    inputFilePath = file.filePath();
    inputFileName = file.fileName();
}

unsigned int Codificar::tamanhoArquivo(FILE *src) {
    unsigned int fileSize = 0;
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) { //** Função fgetc para buscar caracteres diferentes de EOF
        fileSize++;
    }
    rewind(src); //** Retorna o cursor(c) ao começo do arquivo

    return fileSize;
}

void Codificar::calcFreqChar(FILE *src, unsigned int *freqList) {
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

void Codificar::comprimirArquivo() {
    /** O problema está aqui **/
    QByteArray byteArray1 = inputFilePath.toUtf8();
    const char *inputFile = byteArray1.constData();

    huffmanCodificar(inputFile);

    /** Testes **/
    qDebug() << byteArray1; //Não seria para imprimir uma cadeia de códigos?
    qDebug() << inputFile;

}


void Codificar::huffmanCodificar(const char *inputFile) {
    /** Ler o arquivo source **/
    FILE *src = fopen(inputFile, "rb");


    /** Verificação da existência do arquivo **/
    if (src == NULL) {
        qDebug() << "Nao foi possível encontrar o arquivo de origem!";
        exit(EXIT_FAILURE);
    }


    /** Pega o tamanho do Arquivo Source **/
    unsigned int fileSize;
    fileSize = tamanhoArquivo(src);


    /** Faz uma lista de ocorrência de caracteres **/
    unsigned int *freqList;
    freqList = (unsigned int *)calloc(256, sizeof(unsigned int));
    calcFreqChar(src, freqList);

    /** Calcula número de ocorrências dos caracteres listados **/
    unsigned int charFreq;
    charFreq = calcNumFreq(freqList);


    /** Testes **/
    qDebug() << fileSize;
    qDebug() << *freqList;
    qDebug() << charFreq;


    /**unsigned int c;
    while ((c = fgetc(src)) != EOF) {
        qDebug() << freqList[5];
    }
    rewind(src); **/

     /**for(int i=0; i<256; ++i) {
        if(freqList[i] > 0) {
            qDebug() << freqList[i];
        }
    }**/


    /** Fecha o arquivo **/
    fclose(src);
}
