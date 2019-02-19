
#include <iostream>
#include <QByteArray>
#include <QString>
#include <QList>
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

QByteArray getByteArrayFromString(QString in) {
  QByteArray out;
  out.append(QByteArray::fromHex(in.toLatin1()));
  return out;
}

QList<QByteArray> get_hashes(QString in) {
  QByteArray input = in.toLatin1();
  QByteArray salt = getRand(input.length());
  QByteArray derived;
  for (int i = 0 ; i < input.length(); i++) {
    derived.append(input.at(i)^salt.at(i));
  }
  /*
  std::cout << "input:" << std::endl;
  printByteArray(input);
  std::cout << "salt:" << std::endl;
  printByteArray(salt);
  std::cout << "derived:" << std::endl;
  printByteArray(derived);
  */
  QList<QByteArray> output;
  output.append(salt);
  output.append(derived);
  return output;
}

// assumes arguments come in as strings and need to be converted to regular
// hex
QString get_string(QString hash1str, QString hash2str) {
    QByteArray hash1 = getByteArrayFromString(QString(hash1str));
    QByteArray hash2 = getByteArrayFromString(QString(hash2str));
    QByteArray output;
    for (int i = 0; i < hash1.length(); i++) {
      output.append(hash1.at(i)^hash2.at(i));
    }
    std::cout << "output:" << std::endl;
    printByteArray(output);
    return QString(output);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("not enough args\n");
    return 1;
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
  QList<QByteArray> hashes = get_hashes(QString(argv[1]));
  std::cout << "salt:" << std::endl;
  printByteArray(hashes[0]);
  std::cout << "derived:" << std::endl;
  printByteArray(hashes[1]);
}
