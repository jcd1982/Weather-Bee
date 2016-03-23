#include <QMessageBox>
#include <QSettings>

#include "initsettingsdialog.h"
#include "ui_initsettingsdialog.h"

InitSettingsDialog::InitSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitSettingsDialog)
{
    ui->setupUi(this);

    ui->lineEdit->setInputMask("99999");
    QFontMetrics metrics(QApplication::font());
    ui->lineEdit->setFixedSize(metrics.width("88888")+14,ui->lineEdit->height()+4);

    QMessageBox *infoBox = new QMessageBox(this);
    infoBox->setText("In order to use Weather Bee, you need to enter your zip code and choose a radar station.");
    infoBox->exec();

    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(slotRejected()));
}

void InitSettingsDialog::accept(){
    qDebug() << "InitSettingsDialog::accept";

    QSettings settings;

    if(ui->treeWidget->currentItem() != nullptr &&
       ui->treeWidget->currentItem()->parent() != nullptr &&
       ui->lineEdit->text().length() == 5){

        QString radarStation = ui->treeWidget->currentItem()->text(ui->treeWidget->currentColumn());

        QString radarStationID = radarStation.split(" - ").at(0).toUpper();
        qDebug() << "radarStation" << radarStation;
        qDebug() << "radarStationID" << radarStationID;
        settings.setValue("radarStation", radarStation);
        settings.setValue("radarStationID", radarStationID);
        settings.setValue("zipCode", ui->lineEdit->text());

        settings.setValue("init", "true");

        done(QDialog::Accepted);
    }else{
        QMessageBox *infoBox = new QMessageBox(this);
        infoBox->setText("Please enter your zip code and choose a radar station.");
        infoBox->exec();
    }

    return;
}

void InitSettingsDialog::slotRejected(){
    exit(0);
    return;
}

InitSettingsDialog::~InitSettingsDialog()
{
    delete ui;
}
