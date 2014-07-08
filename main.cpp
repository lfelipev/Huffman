#include "codificar.h"
#include "decodificar.h"

int main(int argc, char *argv[]) {
    Codificar g;
    decodificar f;
    /*switch (atoi(argv[1])) {
    case '-c':
        g.compressFile();
        break;
    case '-d':
       f.decompressFile();
       break;
    }*/
    //g.compressFile(); //teste
    f.decompressFile();
    return 0;
}
