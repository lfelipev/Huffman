#include "Select.h"
#include "codificar.h"
#include "decodificar.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Select w;
    w.show(); 
    // Codificar g = Codificar("in.txt");
    // decodificar f;
     //argv[0] = huffman
     //argv[1] = -c ou arquivo.huff
     /*if(argv[1] == "-c") {
         //argv[2] = arquivo.x
         //argv[3] = -o ou nada
         Codificar c = Codificar(argv[2]);
         if(argv[3] == "-o") {
             c.compressFile();
             //argv[4] = arquivo_de_saida.huff
         }
         else {
             c.compressFile();
         }
     }
     else {
         //argv[2] = -d ou nada
         if(argv[2] == "-d") {
             //argv[3] /home/user/destino
         }
         else {
             f.decompressFile();
         }
     }*/
     /*switch (atoi(argv[1])) {
     case '-c':
         g.compressFile();
         break;
     case '-d':
        f.decompressFile();
        break;
     }*/
     //g.compressFile(); //teste
     //f.decompressFile();

    return app.exec();
}
