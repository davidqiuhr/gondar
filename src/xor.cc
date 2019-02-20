
#include "xor.h"

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

//QList<QByteArray> get_hashes(QString in) {
QByteArray get_hash(QString in, QByteArray hash1) {
  QByteArray input = in.toLatin1();
  QByteArray salt = hash1;
  QByteArray derived;
  for (int i = 0 ; i < input.length(); i++) {
    derived.append(input.at(i)^salt.at(i));
  }
  return derived;
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
    return QString(output);
}
