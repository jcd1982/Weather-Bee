#ifndef INITSETTINGSDIALOG_H
#define INITSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class InitSettingsDialog;
}

class InitSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitSettingsDialog(QWidget *parent = 0);
    ~InitSettingsDialog();

    virtual void accept();

private:
    Ui::InitSettingsDialog *ui;

private slots:

    void slotRejected();
};

#endif // INITSETTINGSDIALOG_H
