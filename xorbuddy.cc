
#include <QByteArray>
#include <iostream>
#include <random>

QByteArray getRand(int len) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);
  QByteArray output;
  for (int i = 0; i < len; i++) {
    unsigned char cur = dis(gen);
    std::cout << "cur = " << (unsigned int)cur << std::endl;
    output.append(cur);
  }
  bool ok;
  return output;
}

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
  // then we store our data...

  // and extract it later:
  QByteArray output;
  for (int i = 0; i < derived.length(); i++) {
    output.append(derived.at(i)^salt.at(i));
  }
  std::cout << "final output: " << output.toStdString() << std::endl;
}
