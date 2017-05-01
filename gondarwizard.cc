
#include <QtWidgets>

#include "gondarwizard.h"
#include "downloader.h"

GondarWizard::GondarWizard(QWidget *parent)
    : QWizard(parent)
{
    addPage(new IntroPage);
    addPage(new ConclusionPage);
    setWindowTitle(tr("Wyzzerd"));
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Select Image"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/frogmariachis.png"));

    label = new QLabel("Target image url:");

    urlLineEdit = new QLineEdit;

    registerField("imageurl", urlLineEdit);

    // Line up widgets horizontally
    // use QVBoxLayout for vertically
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(urlLineEdit);
    setLayout(layout);
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Conclusion"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/frogmariachis.png"));

    label = new QLabel;
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

void ConclusionPage::initializePage()
{
    label->setText("ok");
    qDebug() << "contents: " << field("imageurl").toString();
    QString url_str = field("imageurl").toString();
    QUrl url = QUrl::fromUserInput(url_str);
    manager.url = url;
    //TODO(kendall): this should allow 'next' to be clicked when the download
    // is complete
    QTimer::singleShot(0, &manager, SLOT(execute()));
    qDebug() << "we reach the end";
}
