/********************************************************************************
** Form generated from reading UI file 'GLWidget.ui'
**
** Created: Mon Sep 10 17:36:27 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLWIDGET_H
#define UI_GLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GLWidgetClass
{
public:

    void setupUi(QWidget *GLWidgetClass)
    {
        if (GLWidgetClass->objectName().isEmpty())
            GLWidgetClass->setObjectName(QString::fromUtf8("GLWidgetClass"));
        GLWidgetClass->resize(600, 400);

        retranslateUi(GLWidgetClass);

        QMetaObject::connectSlotsByName(GLWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *GLWidgetClass)
    {
        GLWidgetClass->setWindowTitle(QApplication::translate("GLWidgetClass", "GLWidget", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GLWidgetClass: public Ui_GLWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLWIDGET_H
