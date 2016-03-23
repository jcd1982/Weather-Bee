#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "pages.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    // TODO: below destructor investigate whether or not needs to be here
    //~SettingsDialog();

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private slots:
    void slotSaveSettings();
private:
    void createIcons();
    ConfigurationPage *configurationPage;
    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;

};

#endif // SETTINGSDIALOG_H
