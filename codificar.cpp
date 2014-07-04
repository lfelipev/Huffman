#include "codificar.h"
#include<QFileInfo>
#include<QDebug>

Codificar::Codificar() {
    QFileInfo file("../Huffman/arvore.jpg");
    inputFilePath = file.filePath();
    inputFileName = file.fileName();
}

void Codificar::compressFile() {
    QByteArray byteArray1 = inputFilePath.toUtf8();
    const char *inputFile = byteArray1.constData();

    huffmanEncode(inputFile);
}

void Codificar::outputFilePath(const char *path, char * outputPath, const char *fileExtension) {
    int i;
    const int pathLength = strlen(path);

    for(i=0; i<pathLength-4; i++) {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension);
}

unsigned int Codificar::getFileSize(FILE * src) {
    unsigned int fileSize = 0;
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) { // Função fgetc para buscar caracteres diferentes de EOF
        fileSize++;
    }
    rewind(src); // Retorna o cursor(c) ao começo do arquivo

    return fileSize;
}

void Codificar::calcCharFreq(FILE * src, unsigned int * freqList) {
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) {  //Lista os caracteres até o fim do arquivo
        freqList[c]++;
    }
    rewind(src);
}

unsigned int Codificar::calcNumOfFreq(unsigned int * freqList) {
    unsigned int i;
    unsigned int numOfFreq = 0;

    for(i=0; i<256; i++) {
        if(freqList[i] > 0) { //Conta cada vez que um caractere aparece
            numOfFreq++;
        }
    }

    return numOfFreq;
}

void Codificar::buildNodeList(HuffNode ** nodeList, unsigned int * freqList) {
    unsigned int i;
    HuffNode * newNode;

    for(i=0; i<256; i++) {
        if(freqList[i] > 0) { //Um nó será criado sempre que um caractere dentre os 256 aparecer
            newNode = (HuffNode *)calloc(1, sizeof(HuffNode)); //Aloca um espaço na memória para o nó do caractere
            newNode->charCode = i; //Caractere em questão
            newNode->freq = freqList[i]; //Quantidade em que o caractere em questão aparece
            newNode->next = NULL; //(Lista) Próximo nó da lista
            newNode->left = NULL; //(Árvore) Filho da esquerda
            newNode->right = NULL; //(Árvore) Filho da Direita
            newNode->leaf = true; //Cada novo nó que entra na LISTA será considerado uma folha

            addToNodeList(nodeList, newNode);
        }
    }
}

void Codificar::addToNodeList(HuffNode ** nodeList, HuffNode * newNode) {
    HuffNode * prevNode = NULL; //Nó auxiliar inicial
    HuffNode * currNode = *nodeList; //currNode apontará para o conteúdo da nodeList

    while(currNode != NULL && currNode->freq < newNode->freq) {
        prevNode = currNode; //Auxiliar será o currNode se este for menor que o newNode
        currNode = prevNode->next; // o currNode será o nó de menor frequência
    }

    newNode->next = currNode; //Faz newNode apontar para o nó de menor frequência

    if(prevNode == NULL) { //Condição inicial(Loop 1)
        *nodeList = newNode; //A nodeList será composta pelo primeiro nó de caractere
    }
    else {  //Loops restantes
        prevNode->next = newNode; //Faz o auxiliar apontar para o novo nó
    }
}

void Codificar::buildHuffTree(HuffNode ** nodeList) {
    HuffNode * leftNode, * rightNode;
    HuffNode * newNode;

    while((*nodeList)->next) { //Enquanto a Lista não aponta para o NULL. EX de Lista:(Lista 10-13-45-48-51)
        leftNode = *nodeList; //LeftNode será o primeiro nó da Lista  (10)
        *nodeList = leftNode->next; //A Lista será o segundo nó dela mesma (13)


        rightNode = *nodeList; //RightNode será (13)
        *nodeList = rightNode->next; //A Lista será (45) o terceiro nó da lista

        newNode = (HuffNode *)calloc(1, sizeof(HuffNode)); //Aloca espaço para o novo nó;
        newNode->charCode = 0; //Típico nó da Árvore de Huffman será um número
        newNode->freq = leftNode->freq + rightNode->freq; //Nó resultante será a soma dos filhos (13+10 = 23)
        newNode->next = NULL; //Os nós de uma arvore não tem um "Next" por isso é NULL
        newNode->left = leftNode; //Filho da Esquerda (10)
        newNode->right = rightNode; //Filho da Direita (13)
        newNode->leaf = false; //Como o nó resultante(23) tem 2 filhos (10 e 13), não é folha.

        addToNodeList(nodeList, newNode);  //Adciona o nó (23) na Lista Ligada
    }
}

bool Codificar::buildHuffCode(HuffNode * treeRoot, HuffCode * hCode, unsigned char goalChar) {
    if(treeRoot->charCode == goalChar && treeRoot->leaf) {
        return true;
    }

    if(treeRoot->left) {
        hCode->code[hCode->length] = '0';
        hCode->length++;

        if(hCode->length == 32) {
            printf("Code size exceeds limit!");
            return false;
        }

        if(buildHuffCode(treeRoot->left, hCode, goalChar)) {
            hCode->code[hCode->length] = 0;
            return true;
        }
        else {
            hCode->length--;
            hCode->code[hCode->length] = 0;
        }
    }

    if(treeRoot->right) {
        hCode->code[hCode->length] = '1';
        hCode->length++;

        if(buildHuffCode(treeRoot->right, hCode, goalChar)) {
            return true;
        }
        else {
            hCode->length--;
            hCode->code[hCode->length] = 0;
        }
    }

    return false;
}

void Codificar::writeHeader(FILE * dest, HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize) {
    hHeader.numOfFreq = numOfFreq;
    hHeader.fileSize = fileSize;

    fwrite(&hHeader, sizeof(hHeader), 1, dest);
}

void Codificar::writeFreq(FILE * dest, unsigned int * freqList, HuffFreq hFreq) {
    unsigned int i;

    for(i=0; i<256; i++) {
        if(freqList[i] > 0) {
            hFreq.charCode = i;
            hFreq.freq = freqList[i];

            fwrite(&hFreq, sizeof(HuffFreq), 1, dest);
        }
    }
}

void Codificar::writeEncodedData(FILE * src, FILE * dest, HuffCode * huffCodeTable, unsigned int fileSize) {
    unsigned int i, c;
    unsigned int bits = 0;
    char currChar = 0;
    HuffCode currCode;
    unsigned int bytes = 0;

    while((c = fgetc(src)) != EOF) {
        bytes++;
        currCode = huffCodeTable[c];

        for(i=0; i<currCode.length; i++) {
            currChar = (currChar << 1) + (currCode.code[i] == '1' ? 1 : 0);
            bits++;

            if(bits == 8) {
                fputc(currChar, dest);
                currChar = 0;
                bits = 0;
            }
        }
    }

    if(bits > 0) {
        currChar = currChar << (8 - bits);
        fputc(currChar, dest);
    }
}

void Codificar::freeHuffTree(HuffNode * treeRoot){
    if(treeRoot)
    {
        freeHuffTree(treeRoot->left);
        freeHuffTree(treeRoot->right);

        free(treeRoot);
    }
}

void Codificar::huffmanEncode(const char *inputFile) {
    /** Ler o arquivo source **/
    FILE * src = fopen(inputFile, "rb");

    /** Abre o destino do arquivo **/
    char outputPath[1000];
    const char * fileExtension = "3.huff";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE * dest = fopen(outputPath, "wb");

    /** Verificação da existência do arquivo **/
    if (src == NULL || dest == NULL) {
        qDebug() << "Nao foi possível encontrar o arquivo de origem!";
        exit(EXIT_FAILURE);
    }

    /** Pega o tamanho do Arquivo Source **/
    unsigned int fileSize;
    fileSize = getFileSize(src);

    /** Faz uma lista de ocorrência de caracteres **/
    unsigned int * freqList;
    freqList = (unsigned int *)calloc(256, sizeof(unsigned int));
    calcCharFreq(src, freqList);

    /** Calcula número de ocorrências dos caracteres listados **/
    unsigned int numOfFreq;
    numOfFreq = calcNumOfFreq(freqList);

    /** Cria uma lista dos Nós de Huffman em ordem crescente **/
    HuffNode * nodeList = NULL;
    buildNodeList(&nodeList, freqList);

    /** Cria a Árvode de Huffman **/
    buildHuffTree(&nodeList);
    HuffNode * treeRoot = nodeList;

    /** Cria o código de Huffman **/
    unsigned int i;
    HuffCode newCode;
    HuffCode * huffCodeTable;
    huffCodeTable = (HuffCode *)calloc(256, sizeof(HuffCode));
    for(i=0; i<256; i++)
    {
        if(freqList[i] > 0)
        {
            newCode.length = 0;
            buildHuffCode(treeRoot, &newCode, i);
            huffCodeTable[i] = newCode;
        }
    }

    /** Escreve o cabeçalho no arquivo destino **/
    HuffHeader hHeader;
    writeHeader(dest, hHeader, numOfFreq, fileSize);

    /** Escreve as frequências no arquivo destino **/
    HuffFreq hFreq;
    writeFreq(dest, freqList, hFreq);

    /** Escreve o código no arquivo destino **/
    writeEncodedData(src, dest, huffCodeTable, fileSize);

    /** Libera a memória **/
    freeHuffTree(treeRoot);
    treeRoot = NULL;
    free(huffCodeTable);
    free(freqList);

    /** Fecha o arquivo **/
    fclose(src);
    fclose(dest);
}
