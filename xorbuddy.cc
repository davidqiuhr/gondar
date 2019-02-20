
#include "xor.h"

#include <QByteArray>
#include <QString>
#include <iostream>

int main(int argc, char *argv[]) {

  auto use_str = "USE:\nxorbuddy random thequickbrownfox\nxorbuddy xor thequickbrownfox 12345abcdef\nxorbuddy dexor 12345abcdef 232323dfdfdf\n";

  // rand generation mode
  if (argc < 3) {
    std::cout << use_str;
    return 1;
  }
  if (std::string(argv[1]) == std::string("random")) {
  // we need to pass in the string, so we generate a rand of the correct length
    if (argc != 3) {
      std::cout << use_str;
      return 1;
    }
    QString secret = argv[2];
    QByteArray rand = getRand(secret.length());
    printByteArray(rand);
    return 0; 
  } else if (std::string(argv[1]) == std::string("dexor")) {
    if (argc != 4) {
      std::cout << use_str;
      return 1;
    }
    // then we're in dexor mode
    QString out = get_string(QString(argv[2]), QString(argv[3])); 
    std::cout << out.toStdString() << std::endl;
    return 0;
  } else if (std::string(argv[1]) == std::string("xor")) {
    if (argc != 4) {
      return 1;
    }
    QByteArray randhash = getByteArrayFromString(QString(argv[3])); 
    QByteArray hash = get_hash(QString(argv[2]), randhash);
    printByteArray(hash);
    return 0;
  } else {
    std::cout << use_str;
  }
}
