#include "decodificar.h"
#include<QFileInfo>
#include<QDebug>

decodificar::decodificar() {
    QFileInfo file("../Huffman/in2.bin");
    inputFilePath = file.filePath();
    inputFileName = file.fileName();
}

void decodificar::decompressFile() {
    QByteArray byteArray1 = inputFilePath.toUtf8();
    const char *inputFile = byteArray1.constData();

    huffmanDecode(inputFile);
}

void decodificar::outputFilePath(const char *path, char *outputPath, const char *fileExtension) {
    int i;
    const int pathLength = strlen(path);
    for (i=0; i<pathLength-4; ++i) {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension);
}

void decodificar::buildNodeList(HuffNode **nodeList, HuffFreq *hFreq, unsigned int charFreq) {
    unsigned int i;
    HuffNode *newNode;

    for(i=0; i<charFreq; ++i) {
        newNode = (HuffNode *)malloc(sizeof(HuffNode));
        newNode->charCode = hFreq[i].chr;
        newNode->freq = hFreq[i].freq;
        newNode->next = NULL;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->leaf = true;

        addToNodeList(nodeList, newNode);
    }
}

void decodificar::addToNodeList(HuffNode **nodeList, HuffNode *newNode) {
    HuffNode *prevNode = NULL; //Nó auxiliar inicial
    HuffNode *currNode = *nodeList; //currNode apontará para o conteúdo da nodeList

    while (currNode != NULL && (currNode->freq < newNode->freq)) {
        prevNode = currNode; //Auxiliar será o currNode se este for menor que o newNode
        currNode = prevNode->next; // o currNode será o nó de menor frequência
    }

    newNode->next = currNode; //Faz newNode apontar para o nó de menor frequência

    if(prevNode == NULL) { //Condição inicial(Loop 1)
        *nodeList = newNode; //A nodeList será composta pelo primeiro nó de caractere
    }
    else { //Loops restantes
        prevNode->next = newNode; //Faz o auxiliar apontar para o novo nó
    }
}

void decodificar::buildHuffTree(HuffNode **nodeList) {
    HuffNode *leftNode;
    HuffNode *rightNode;
    HuffNode *newNode;

    while((*nodeList)->next != NULL) {
        newNode = (HuffNode *)malloc(sizeof(HuffNode));

        leftNode = *nodeList;
        *nodeList = leftNode->next;

        rightNode = *nodeList;
        *nodeList = rightNode->next;

        newNode->charCode = 0;
        newNode->freq = leftNode->freq + rightNode->freq;
        newNode->next = NULL;
        newNode->left = leftNode;
        newNode->right = rightNode;
        newNode->leaf = false;

        addToNodeList(nodeList, newNode);
    }
}

void decodificar::writeDecodedData(FILE *src, FILE *dest, HuffNode *rootTree, unsigned int fileSize) {
    int bit = -1;
    unsigned int c;
    unsigned int byteWritten = 0;
    HuffNode *currNode = rootTree;

    while(byteWritten < fileSize) {
        if(bit < 0) {
            c = fgetc(src);

            if(c == EOF) {
                break;
            }

            bit = 7;
        }

        if((c >> bit) & 1) {
            currNode = currNode->right;
        }
        else {
            currNode = currNode->left;
        }

        if(currNode->leaf) {
            fputc(currNode->charCode, dest);
            byteWritten++;
            currNode = rootTree;
        }
        bit--;
    }
}

void decodificar::freeHuffTree(HuffNode *treeRoot) {
    if(treeRoot) {
        freeHuffTree(treeRoot->left);
        freeHuffTree(treeRoot->right);

        free(treeRoot);
    }

}

void decodificar::huffmanDecode(const char *inputFile) {
    /** Ler o arquivo source **/
    FILE *src = fopen(inputFile, "rb");

    /** Abre o destino do arquivo **/
    char outputPath[1000];
    const char *fileExtension = ".txt";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE *dest = fopen(outputPath, "wb");

    /** Verificação da existência do arquivo **/
    if (src == NULL) {
        qDebug() << "Nao foi possível encontrar o arquivo de origem!";
        exit(EXIT_FAILURE);
    }

    /** Lê o Cabeçalho de Huffman **/
    HuffHeader hHeader;
    fread(&hHeader, sizeof(hHeader), 1, src);

    /** Lê as frequências **/
    HuffFreq *hFreq = (HuffFreq *)calloc(hHeader.charFreq, sizeof(HuffFreq));
    fread(hFreq, sizeof(HuffFreq), hHeader.charFreq, src);

    /** Cria a lista ligada com os caracteres e as frequências **/
    HuffNode *nodeList = NULL;
    buildNodeList(&nodeList, hFreq, hHeader.charFreq);

    /** Cria a Árvore de Huffman**/
    buildHuffTree(&nodeList);

    /** Escreve os dados decodificados no arquivo destino **/
    writeDecodedData(src, dest, nodeList, hHeader.fileSize);

    /** Libera a memória **/
    freeHuffTree(nodeList);
    nodeList = NULL;
    free(hFreq);

    /** Fecha os Arquivos **/
    fclose(src);
    fclose(dest);
}
