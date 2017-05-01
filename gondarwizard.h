
#ifndef GONDARWIZARD_H
#define GONDARWIZARD_H

#include <QWizard>

#include "downloader.h"

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;

class GondarWizard : public QWizard
{
    Q_OBJECT

public:
    GondarWizard(QWidget *parent = 0);
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

private:
    QLabel *label;
    QLineEdit *urlLineEdit;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);

protected:
    void initializePage() override;

private:
    QLabel *label;
    DownloadManager manager;
};

#endif /* GONDARWIZARD */
