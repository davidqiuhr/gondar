
#ifndef GONDARWIZARD_H
#define GONDARWIZARD_H

#include <QWizard>
#include <QRadioButton>

#include <QString>

#include "downloader.h"

extern "C" {
    #include "deviceguy.h"
}

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;

class GondarButton : public QRadioButton {
    Q_OBJECT

public:
    GondarButton(const QString & text, 
                 unsigned int device_num,
                 QWidget *parent = 0);
    unsigned int index = 0;
};

class GondarWizard : public QWizard
{
    Q_OBJECT

public:
    GondarWizard(QWidget *parent = 0);

};

class AdminCheckPage : public QWizardPage
{
    Q_OBJECT

public:
    AdminCheckPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    bool isComplete() const;

public slots:
    void getIsAdmin();
    void showIsAdmin();
    void showIsNotAdmin();
signals:
    void isAdminRequested();
    void isAdminReady();
    void isNotAdminReady();
private:
    QLabel *label;
    QTimer *tim;
    bool is_admin;
};

class ImageSelectPage : public QWizardPage
{
    Q_OBJECT

public:
    ImageSelectPage(QWidget *parent = 0);

protected:
    void initializePage() override;
private:
    QLabel *label;
    QLineEdit *urlLineEdit;
};

class DownloadProgressPage : public QWizardPage
{
    Q_OBJECT

public:
    DownloadProgressPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    bool isComplete() const;

public slots:
    void markComplete();

private:
    DownloadManager manager;
    bool download_finished;
    QLabel * label;
};

class UsbInsertPage : public QWizardPage
{
    Q_OBJECT

public:
    UsbInsertPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    bool isComplete() const;

private:
    QLabel *label;
    QTimer *tim;

public slots:
    void getDriveList();
    void showDriveList();
signals:
    void driveListRequested();
    void driveListReady();
};

class DeviceSelectPage : public QWizardPage
{
    Q_OBJECT

public:
    DeviceSelectPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    bool validatePage() override;

private:
    QLabel *drivesLabel;
    QGroupBox *drivesBox;
    QButtonGroup * radioGroup;
};

class KewlPage: public QWizardPage {
    Q_OBJECT

public:
    KewlPage(QWidget *parent = 0);

protected:
    void initializePage() override;
    bool validatePage() override;

public slots:
    void writeToDrive();
signals:
    void writeDriveRequested();
};

#endif /* GONDARWIZARD */
