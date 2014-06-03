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
    QByteArray byteArray1 = inputFilePath.toUtf8();
    const char *inputFile = byteArray1.constData();

    huffmanCodificar(inputFile);
}


void Codificar::huffmanCodificar(const char *inputFile) {
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
    qDebug() << charFreq;


    /**unsigned int c;
    while ((c = fgetc(src)) != EOF) {
        qDebug() << freqList[5];
    }
    rewind(src); **/
    char c;
    for(int i=0; i<256; ++i) {
        if(freqList[i] > 0) {
            c=i;
            qDebug() << "DEC. ASCII: " << i << "CHAR: " << c << "Frequencia: " << freqList[i];
            fwrite(&c, sizeof(char), sizeof(c), dest);
        }
    }


    /** Fecha o arquivo **/
    fclose(src);
    fclose(dest);
}
