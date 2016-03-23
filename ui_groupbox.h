/********************************************************************************
** Form generated from reading UI file 'groupbox.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GROUPBOX_H
#define UI_GROUPBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GroupBox
{
public:
    QVBoxLayout *verticalLayout;
    QRadioButton *radioN0R;
    QRadioButton *radioN0Z;
    QRadioButton *radioN0S;
    QRadioButton *radioN0V;
    QRadioButton *radioN1P;
    QRadioButton *radioNCR;
    QRadioButton *radioNTP;

    void setupUi(QGroupBox *GroupBox)
    {
        if (GroupBox->objectName().isEmpty())
            GroupBox->setObjectName(QStringLiteral("GroupBox"));
        GroupBox->resize(257, 251);
        verticalLayout = new QVBoxLayout(GroupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        radioN0R = new QRadioButton(GroupBox);
        radioN0R->setObjectName(QStringLiteral("radioN0R"));
        QFont font;
        font.setPointSize(9);
        radioN0R->setFont(font);

        verticalLayout->addWidget(radioN0R);

        radioN0Z = new QRadioButton(GroupBox);
        radioN0Z->setObjectName(QStringLiteral("radioN0Z"));
        radioN0Z->setFont(font);

        verticalLayout->addWidget(radioN0Z);

        radioN0S = new QRadioButton(GroupBox);
        radioN0S->setObjectName(QStringLiteral("radioN0S"));
        radioN0S->setFont(font);

        verticalLayout->addWidget(radioN0S);

        radioN0V = new QRadioButton(GroupBox);
        radioN0V->setObjectName(QStringLiteral("radioN0V"));
        radioN0V->setFont(font);

        verticalLayout->addWidget(radioN0V);

        radioN1P = new QRadioButton(GroupBox);
        radioN1P->setObjectName(QStringLiteral("radioN1P"));
        radioN1P->setFont(font);

        verticalLayout->addWidget(radioN1P);

        radioNCR = new QRadioButton(GroupBox);
        radioNCR->setObjectName(QStringLiteral("radioNCR"));
        radioNCR->setFont(font);

        verticalLayout->addWidget(radioNCR);

        radioNTP = new QRadioButton(GroupBox);
        radioNTP->setObjectName(QStringLiteral("radioNTP"));
        radioNTP->setFont(font);

        verticalLayout->addWidget(radioNTP);

        radioN0R->raise();
        radioN0Z->raise();
        radioN0S->raise();
        radioN0V->raise();
        radioN1P->raise();
        radioN1P->raise();
        radioNCR->raise();
        radioNTP->raise();

        retranslateUi(GroupBox);

        QMetaObject::connectSlotsByName(GroupBox);
    } // setupUi

    void retranslateUi(QGroupBox *GroupBox)
    {
        GroupBox->setWindowTitle(QApplication::translate("GroupBox", "GroupBox", 0));
        GroupBox->setTitle(QApplication::translate("GroupBox", "Radar Type", 0));
        radioN0R->setText(QApplication::translate("GroupBox", "Base Reflectivity N0R", 0));
        radioN0Z->setText(QApplication::translate("GroupBox", "Base Reflectivity Long Range N0Z", 0));
        radioN0S->setText(QApplication::translate("GroupBox", "Storm Relative Motion N0S", 0));
        radioN0V->setText(QApplication::translate("GroupBox", "Base Velocity N0V", 0));
        radioN1P->setText(QApplication::translate("GroupBox", "One-Hour Precipitation N1P", 0));
        radioNCR->setText(QApplication::translate("GroupBox", "Composite Reflectivity NCR", 0));
        radioNTP->setText(QApplication::translate("GroupBox", "Storm Total Precipitation NTP", 0));
    } // retranslateUi

};

namespace Ui {
    class GroupBox: public Ui_GroupBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPBOX_H
