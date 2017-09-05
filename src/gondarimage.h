
#ifndef GONDARIMAGE_H
#define GONDARIMAGE_H

class GondarImage {
  public:
    GondarImage() {
        this->product = QString("");
        this->imageName = QString("");
        this->url = QUrl();
    }
    GondarImage(QString productIn, QString imageNameIn, QUrl urlIn) {
        this->product = productIn;
        this->imageName = imageNameIn;
        this->url = urlIn;
    }
    QString getCompositeName() {
        return product + " " + imageName;
    }
    bool is32Bit() {
        return imageName.contains("32-bit", Qt::CaseInsensitive);
    }
    bool isDeployable() {
        return imageName.contains("deployable", Qt::CaseInsensitive);
    }
    QString product;
    QString imageName;
    QUrl url;
};

#endif
