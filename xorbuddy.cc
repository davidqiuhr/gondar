
#include <iostream>
#include <QByteArray>
#include <QString>
#include <random>

void printByteArray(QByteArray in) {
  std::cout << in.toHex().toStdString() << std::endl;
}

QByteArray getRand(int len) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);
  QByteArray output;
  for (int i = 0; i < len; i++) {
    unsigned char cur = dis(gen);
    //std::cout << "cur = " << (unsigned int)cur << std::endl;
    output.append(cur);
  }
  return output;
}
/*
QByteArray getByteArrayFromString(QString in) {
  QByteArray out;
  for (int i = 0; i < in.length(); i+=2) {
    // toLatin1?
    //out.append(QByteArray::fromHex(
  }
}
*/

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("wrong number of args\n");
    return 0;
  }
  QByteArray input(argv[1]);
  QByteArray salt = getRand(input.length());
  QByteArray derived;
  for (int i = 0 ; i < input.length(); i++) {
    derived.append(input.at(i)^salt.at(i));
  }
  std::cout << "input:" << std::endl;
  printByteArray(input);
  std::cout << "salt:" << std::endl;
  printByteArray(salt);
  std::cout << "derived:" << std::endl;
  printByteArray(derived);
  // then we store our data...

  // and extract it later:
  QByteArray output;
  for (int i = 0; i < derived.length(); i++) {
    output.append(derived.at(i)^salt.at(i));
  }
  std::cout << "output:" << std::endl;
  printByteArray(output);
  std::cout << "final output: " << output.toStdString() << std::endl;
}
