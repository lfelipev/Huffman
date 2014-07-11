#include "codificar.h"
#include "ui_codificar.h"
#include "QFileDialog"
#include "QMessageBox"
#include "stdio.h"
#include "stdlib.h"
#include "QProgressDialog"
#include <QDebug>
#include<string>

Codificar::Codificar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Codificar) {
    ui->setupUi(this);

    /** Botões para selecionar **/
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseInputFile())); //Botão de busca
    connect(ui->encodeButton, SIGNAL(clicked()), this, SLOT(compressFile())); //Botão de comprimir

    /** Texto de Error **/
    ui->warningText->setVisible(false); //Mensagem de erro inicialmente invisível
}

Codificar::~Codificar() { //Criado automaticamente
    delete ui;
}

void Codificar::browseInputFile() { //Função de buscar o arquivo
    /** Caixa para selecionar o arquivo **/
    inputFileName = QFileDialog::getOpenFileName(this, tr("Selecione o arquivo"),"../Huffman/", //Pasta inicial
                                                 tr("All files (*.*)"), //Extensão do arquivo a buscar
                                                 0, QFileDialog::DontUseNativeDialog);
    ui->inputFile->setText(inputFileName);
}

void Codificar::outputFilePath(const char *path, char *outputPath, const char *fileExtension) {
    int i;
    const int pathLength = strlen(path); //Variável para o tamanho do Path

    for(i=0; i<pathLength-4; i++) {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension); //Concatena o NOME com .huff (Fica NOME.huff)
}

void Codificar::compressFile() {
    if (inputFileName == "") {
        /** ERROR MSG **/
        ui->warningText->setVisible(false);
        showDoneMessage("Por favor selecione um arquivo."); //Mensagem de erro caso nada seja selecionado
    }
    else {
        ui->warningText->setVisible(false);

        QByteArray byteArray1 = inputFileName.toUtf8(); //
        const char* inputFile = byteArray1.constData(); //Conversão do path de QSTring para "const char*"

        huffmanEncode(inputFile);
    }
}

void Codificar::showDoneMessage(const char *msg) {
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setWindowTitle("Compressão");  //Título da janela
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

unsigned int Codificar::getFileSize(FILE *src) {
    unsigned int fileSize = 0;
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) { // Função fgetc para buscar caracteres diferentes de EOF
        fileSize++;
    }
    rewind(src); // Retorna o cursor(c) ao começo do arquivo

    return fileSize;
}

void Codificar::calcCharFreq(FILE *src, unsigned int *freqList) {
    unsigned int c; // Cursor

    while((c = fgetc(src)) != EOF) { //Lista os caracteres até o fim do arquivo
        freqList[c]++;
    }
    rewind(src);
}

unsigned int Codificar::calcNumOfFreq(unsigned int *freqList) {
    unsigned int i;
    unsigned int numOfFreq = 0;

    for(i=0; i<256; i++) {
        if(freqList[i] > 0) { //Conta cada vez que um caractere aparece
            numOfFreq++;
        }
    }

    return numOfFreq;
}

void Codificar::buildNodeList(HuffNode **nodeList, unsigned int *freqList) {
    unsigned int i;
    HuffNode *newNode;

    for(i=0; i<256; i++) {
        if(freqList[i] > 0) { //Um nó será criado sempre que um caractere dentre os 256 aparecer
            newNode = (HuffNode *)calloc(1, sizeof(HuffNode)); //Aloca um espaço na memória para o nó do caractere
            newNode->charCode = i;  //Caractere em questão
            newNode->freq = freqList[i]; //Quantidade em que o caractere em questão aparece
            newNode->next = NULL; //(Lista) Próximo nó da lista
            newNode->left = NULL; //(Árvore) Filho da esquerda
            newNode->right = NULL; //(Árvore) Filho da Direita
            newNode->leaf = true; //Cada novo nó que entra na LISTA será considerado uma folha

            addToNodeList(nodeList, newNode);
        }
    }
}

void Codificar::addToNodeList(HuffNode **nodeList, HuffNode *newNode) {
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
    else { //Loops restantes
        prevNode->next = newNode; //Faz o auxiliar apontar para o novo nó
    }
}

void Codificar::buildHuffTree(HuffNode **nodeList) {
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
        newNode->next = NULL; //Os nós de uma arvore não tem um "Next" por isso é NUL
        newNode->left = leftNode; //Filho da Esquerda (10)
        newNode->right = rightNode; //Filho da Direita (13)
        newNode->leaf = false; //Como o nó resultante(23) tem 2 filhos (10 e 13), não é folha.

        addToNodeList(nodeList, newNode); //Adciona o nó (23) na Lista Ligada
    }
}

bool Codificar::buildHuffCode(HuffNode *treeRoot, HuffCode *hCode, unsigned char goalChar) {
    if(treeRoot->charCode == goalChar && treeRoot->leaf) { //TreeRoot 0: 11 hCode = newCode(char + length)
        return true; //Nesse caso é quando a árvore tem apenas 1 caractere
    }

    if(treeRoot->left) { //Filho da Esquerda = 6
        hCode->code[hCode->length] = '0'; //Para cada filho da esquerda: 0
        hCode->length++; //Incrementa a largura da árvore

        if(hCode->length == 32) {
            printf("O códico atingiu seu tamanho limite! (32)");
            return false;
        }

        if(buildHuffCode(treeRoot->left, hCode, goalChar)) {
            hCode->code[hCode->length] = 0; //Caso o caractere do filho da esquerda é igual ao goalChar
            return true;
        }
        else {
            hCode->length--;
            hCode->code[hCode->length] = 0;
        }
    }

    if(treeRoot->right) {
        hCode->code[hCode->length] = '1';  //Para cada filho na direita: 1
        hCode->length++; //Incrementa a largura da árvore

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

void Codificar::writeHeader(FILE *dest, HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize) {
    QByteArray byteArray1 = inputFileName.toUtf8();
    const char *fileName = byteArray1.constData();
    hHeader.fileExtension = strrchr(fileName, '.');

    hHeader.numOfFreq = numOfFreq; //Escreve quantos caracteres repetem
    hHeader.fileSize = fileSize; //Escreve o tamanho do arquivo

    fwrite(&hHeader, sizeof(hHeader), 1, dest);
}

void Codificar::writeFreq(FILE *dest, unsigned int *freqList, HuffFreq hFreq) {
    unsigned int i;

    for(i=0; i<256; i++) { //Escreve o caractere com a frequência
        if(freqList[i] > 0) {
            hFreq.charCode = i; //caractere
            hFreq.freq = freqList[i]; //frequencia

            fwrite(&hFreq, sizeof(HuffFreq), 1, dest); //escreve nó contendo as informações
        }
    }
}

void Codificar::writeEncodedData(FILE *src, FILE *dest, HuffCode *huffCode, unsigned int fileSize) {
    /** Variáveis principais **/
    unsigned int i, c;
    unsigned int bits = 0;
    unsigned int bytes = 0;
    char currChar = 0;
    HuffCode currCode;

    /** Variáveis p/ Interface gráfica **/
    bool cancel = false;
    unsigned int interval = fileSize/100;
    int progress = 1;

    /** Interface gráfica **/
    QProgressDialog progressDialog("Comprimindo...", "Cancelar", 0, fileSize, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setValue(0);

    /** Programa principal **/
    while((c = fgetc(src)) != EOF) { //Até o fim do arquivo
        bytes++; //Incrementa os bytes para cada caractere
        currCode = huffCode[c]; //iguala os códigos dos caracteres

        for(i=0; i<currCode.length; i++) { //de 0 até o tamanho do código do caractere
            currChar = (currChar << 1) + (currCode.code[i] == '1' ? 1 : 0); //Desloca os bits para listar os caracteres
            bits++;

            if(bits == 8) { //Ao atingir os 8 bits
                fputc(currChar, dest); //Escreve os bytes no outputFile
                currChar = 0;
                bits = 0;
            }
        }

        /** Interface gráfica **/
        if(bytes > interval*progress) {
            progressDialog.setValue(progress);
            progress++;
        }

        if (progressDialog.wasCanceled()) {
            cancel = true;
            showDoneMessage("Interrompido!");
            break;
        }
    }

    /** Função principal **/
    if(bits > 0) { //Se sobrar lixo
        currChar = currChar << (8 - bits);
        fputc(currChar, dest);
    }

    progressDialog.setValue(fileSize);

    if(!cancel) {
        showDoneMessage("A compressão foi um sucesso!");
    }
}

void Codificar::freeHuffTree(HuffNode * treeRoot) {
    if(treeRoot) {
        freeHuffTree(treeRoot->left);
        freeHuffTree(treeRoot->right);

        free(treeRoot);
    }
}

void Codificar::huffmanEncode(const char* inputFile) {
    /** Ler o arquivo source **/
    FILE *src = fopen(inputFile, "rb");

    /** Abre o destino do arquivo **/
    char outputPath[1000];
    const char *fileExtension = ".huff";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE *dest = fopen(outputPath, "wb");

    /** Verificação da existência do arquivo **/
    if (src == NULL || dest == NULL) {
        printf("Arquivo de entrada inexistente!");
        exit(EXIT_FAILURE);
    }

    /** Pega o tamanho do Arquivo Source **/
    unsigned int fileSize;
    fileSize = getFileSize(src);

    /** Faz uma lista de ocorrência de caracteres **/
    unsigned int *freqList;
    freqList = (unsigned int *)calloc(256, sizeof(unsigned int));
    calcCharFreq(src, freqList);

    /** Calcula número de ocorrências dos caracteres listados **/
    unsigned int numOfFreq;
    numOfFreq = calcNumOfFreq(freqList);

    /** Cria uma lista dos Nós de Huffman em ordem crescente **/
    HuffNode *nodeList = NULL;
    buildNodeList(&nodeList, freqList);

    /** Cria a Árvode de Huffman **/
    buildHuffTree(&nodeList);
    HuffNode *treeRoot = nodeList;

    /** Codificação da árvore **/
    unsigned int i;
    HuffCode newCode;
    HuffCode *huffCode;
    huffCode = (HuffCode *)calloc(256, sizeof(HuffCode));
    for(i=0; i<256; i++) {
        if(freqList[i] > 0) { //Para cada caractere da lista
            newCode.length = 0;
            buildHuffCode(treeRoot, &newCode, i);
            huffCode[i] = newCode;
        }
    }

    /** Escreve o cabeçalho no arquivo destino **/
    HuffHeader hHeader;
    writeHeader(dest, hHeader, numOfFreq, fileSize);

    /** Escreve as frequências no arquivo destino **/
    HuffFreq hFreq;
    writeFreq(dest, freqList, hFreq);

    /** Escreve o código no arquivo destino **/
    writeEncodedData(src, dest, huffCode, fileSize);

    /** Libera a memória **/
    freeHuffTree(treeRoot);
    treeRoot = NULL;
    free(huffCode);
    free(freqList);

    /** Fecha os arquivo **/
    fclose(src);
    fclose(dest);
}


