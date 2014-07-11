#include "Decodificar.h"
#include "ui_decodificar.h"
#include "QFileDialog"
#include "QMessageBox"
#include "stdio.h"
#include "stdlib.h"
#include "QDebug"
#include "QProgressDialog"
#include "select.h"

Decodificar::Decodificar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Decodificar) {
    ui->setupUi(this);

    /** Botões para selecionar **/
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseInputFile())); //Botão de busca
    connect(ui->decodeButton, SIGNAL(clicked()), this, SLOT(decompressFile())); //Botão de descomprimir

    /** Texto de Error **/
    ui->warningText->setVisible(false);
}

Decodificar::~Decodificar() {
    delete ui;
}

void Decodificar::browseInputFile() {
    /** Caixa para selecionar o arquivo **/
    inputFileName = QFileDialog::getOpenFileName(this, tr("Selecione o arquivo"),"../Huffman/",
                                                 tr("Huffman File (*.huff)")); //Seleciona arquivo .huff

    ui->inputFile->setText(inputFileName);
}

QString Decodificar::getInputFileName() {
    return inputFileName;
}

void Decodificar::outputFilePath(const char *path, char *outputPath, const char *fileExtension) {
    /** Mesma função da codificar.h **/
    int i;
    const int pathLength = strlen(path);

    for(i=0; i<pathLength-4; i++) {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension);
}

void Decodificar::decompressFile() {
    if (inputFileName == "") {
        /** ERROR MSG **/
        ui->warningText->setVisible(false);
        showDoneMessage("Por favor selecione um arquivo."); //Caso nada for selecionado
    }
    else {
        ui->warningText->setVisible(false);

        QByteArray byteArray1 = inputFileName.toUtf8();
        const char* inputFile = byteArray1.constData(); //Conversão para const char *
        huffmanDecode(inputFile);
    }
}

void Decodificar::showDoneMessage(const char *msg) {
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setWindowTitle("Descompressão");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void Decodificar::buildNodeList(HuffNode **nodeList, HuffFreq *hFreq, unsigned int numOfFreq) {
    /** Mesma função da codificar.h **/
    unsigned int i;
    HuffNode *newNode;

    for (i = 0; i < numOfFreq; i++) {
        newNode = (HuffNode *) malloc(sizeof(HuffNode));
        newNode->charCode = hFreq[i].charCode;
        newNode->freq = hFreq[i].freq;
        newNode->next = NULL;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->leaf = true;

        addToNodeList(nodeList, newNode);
    }
}

void Decodificar::addToNodeList(HuffNode **nodeList, HuffNode *newNode) {
    /** Mesma função da codificar.h **/
    HuffNode * prevNode = NULL;
    HuffNode * currNode = *nodeList;

    while ((currNode != NULL && currNode->freq < newNode->freq)) {
        prevNode = currNode;
        currNode = prevNode->next;
    }

    newNode->next = currNode;

    if (prevNode == NULL) {
        *nodeList = newNode;
    }
    else {
        prevNode->next = newNode;
    }
}

void Decodificar::buildHuffTree(HuffNode **nodeList) {
    /** Mesma função da codificar.h **/
    HuffNode * newNode, * leftNode, * rightNode;

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

void Decodificar::writeDecodedData(FILE *src, FILE *dest, HuffNode *rootTree, unsigned int fileSize) {
    /** Variáveis principais **/
    int bit = -1;
    unsigned int c;
    unsigned int bytesWritten = 0;
    HuffNode *currNode = rootTree;

    /** Variáveis da interface gráfica **/
    bool cancel = false;
    unsigned int interval = fileSize/100;
    int progress = 1;

    /** Interface gráfica **/
    QProgressDialog progressDialog("Descomprimindo...", "Cancelar", 0, fileSize, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setValue(0);

    /** Função principal **/
    while(bytesWritten < fileSize) { //Recupera o fileSize do .huff e entra na repetição
        if(bit < 0) { //bit = -1
            c = fgetc(src);

            if(c == EOF) { //Termina a repetição se o .huff estiver vazio
                break;
            }

            bit = 7; //Se não iguala bit = 7
        }

        if((c >> bit) & 1) { //Verifica deslocando 7 bits para a direita e pega os caracteres 1
            currNode = currNode->right;
        }
        else { //Pega os caracteres 0
            currNode = currNode->left;
        }

        if(currNode->leaf) { //Caso para folhas
            fputc(currNode->charCode, dest);
            bytesWritten++; //Incrementa quandos um byte é lido
            currNode = rootTree;
        }

        bit--; //Decrementa um bits

        /** Interface gráfica **/
        if(bytesWritten > interval*progress) {
            progressDialog.setValue(interval*progress);
            progress++;
        }

        if (progressDialog.wasCanceled()) {
            cancel = true;
            showDoneMessage("Interrompido");
            break;
        }
    }

    /** Interface gráfica **/
    progressDialog.setValue(fileSize);

    if(!cancel) {
        showDoneMessage("A descompressão foi um sucesso!");
    }
}

void Decodificar::freeHuffTree(HuffNode *treeRoot) {
    if(treeRoot) {
        freeHuffTree(treeRoot->left);
        freeHuffTree(treeRoot->right);

        free(treeRoot);
    }
}

void Decodificar::huffmanDecode(const char *inputFile) {
    /** Ler o arquivo source **/
    FILE *src = fopen(inputFile, "rb");

    /** Lê o Cabeçalho de Huffman **/
    HuffHeader hHeader;
    fread(&hHeader, sizeof(hHeader), 1, src);

    qDebug() << hHeader.fileExtension;

     /** Abre o destino do arquivo **/
    char outputPath[1000];
    const char *fileExtension = ".jpg";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE *dest = fopen(outputPath, "wb");

    /** Verificação da existência do arquivo **/
    if (src == NULL) {
        fprintf(stderr, "O arquivo não pôde ser aberto");
        exit(EXIT_FAILURE);
    }

    /** Lê o Cabeçalho de Huffman **/
    /*HuffHeader hHeader;
    fread(&hHeader, sizeof(hHeader), 1, src); */

    /** Lê as frequências **/
    HuffFreq *hFreq = (HuffFreq *)calloc(hHeader.numOfFreq, sizeof(HuffFreq));
    fread(hFreq, sizeof(HuffFreq), hHeader.numOfFreq, src);

    /** Cria a lista ligada com os caracteres e as frequências **/
    HuffNode * nodeList = NULL;
    buildNodeList(&nodeList, hFreq, hHeader.numOfFreq);

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

