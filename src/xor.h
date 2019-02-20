
#ifndef _XOR_H_
#define _XOR_H_

#include <QByteArray>
#include <QString>

void printByteArray(QByteArray in);
QByteArray getRand(int len);
QByteArray getByteArrayFromString(QString in);
QByteArray get_hash(QString in, QByteArray hash1);
QString get_string(QString hash1str, QString hash2str);

#endif  //  _XOR_H_
