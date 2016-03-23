/********************************************************************************
** Form generated from reading UI file 'initsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INITSETTINGSDIALOG_H
#define UI_INITSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <radartreewidget.h>

QT_BEGIN_NAMESPACE

class Ui_InitSettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    RadarTreeWidget *treeWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *InitSettingsDialog)
    {
        if (InitSettingsDialog->objectName().isEmpty())
            InitSettingsDialog->setObjectName(QStringLiteral("InitSettingsDialog"));
        InitSettingsDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(InitSettingsDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(InitSettingsDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(InitSettingsDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setInputMask(QStringLiteral(""));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        treeWidget = new RadarTreeWidget(InitSettingsDialog);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));

        verticalLayout->addWidget(treeWidget);

        buttonBox = new QDialogButtonBox(InitSettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(InitSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), InitSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), InitSettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(InitSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *InitSettingsDialog)
    {
        InitSettingsDialog->setWindowTitle(QApplication::translate("InitSettingsDialog", "Dialog", 0));
        label->setText(QApplication::translate("InitSettingsDialog", "Zip Code: ", 0));
    } // retranslateUi

};

namespace Ui {
    class InitSettingsDialog: public Ui_InitSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INITSETTINGSDIALOG_H
