
#include "xor.h"

#include <QByteArray>
#include <QString>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("not enough args\n");
    return 1;
  }
  // rand generation mode
  if (std::string(argv[1]) == std::string("r")) {
  // we need to pass in the string, so we generate a rand of the correct length
    if (argc != 3) {
      printf("wrong # of args\n");
      return 1;
    }
    QString secret = argv[2];
    QByteArray rand = getRand(secret.length());
    printByteArray(rand);
    return 0; 
  }
  if (std::string(argv[1]) == std::string("d")) {
    if (argc != 4) {
      printf("not enough args\n");
      return 1;
    }
    // then we're in dexor mode
    QString out = get_string(QString(argv[2]), QString(argv[3])); 
    std::cout << "final output: " << out.toStdString() << std::endl;
    return 0;
  }
  // implicit else
  QByteArray randhash = getByteArrayFromString(QString(argv[2])); 
  QByteArray hash = get_hash(QString(argv[1]), randhash);
  printByteArray(hash);
}
