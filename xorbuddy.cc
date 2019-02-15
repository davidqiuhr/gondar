
#include <QByteArray>
#include <iostream>

int main(int argc, char ** argv) {
  QByteArray input("my dog has fleas");
  unsigned int rando = 92;
  QByteArray stored;
  for (QByteArray::iterator it = input.begin(); it != input.end(); it++) {
    stored.append(it[0]^rando);
  }
  // then we store our data...

  // and extract it later:
  QByteArray output;
  for (QByteArray::iterator it = stored.begin(); it != stored.end(); it++) {
    output.append(it[0]^rando);
  }
  std::cout << output.toStdString() << std::endl;
}
