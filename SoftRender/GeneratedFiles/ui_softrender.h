/********************************************************************************
** Form generated from reading UI file 'softrender.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOFTRENDER_H
#define UI_SOFTRENDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "RenderView.h"

QT_BEGIN_NAMESPACE

class Ui_SoftRenderClass
{
public:
    QWidget *centralWidget;
    RenderView *render_widget;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *DefineButton;
    QLineEdit *posEditx;
    QLineEdit *posEdity;
    QLineEdit *posEditz;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *tarEditz;
    QLineEdit *tarEditx;
    QLineEdit *tarEdity;
    QLineEdit *fovEdit;
    QLineEdit *nearEdit;
    QLineEdit *farEdit;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QCheckBox *BackFaceCheck;
    QCheckBox *WireFrameCheck;
    QCheckBox *VertexCheck;
    QGroupBox *groupBox_2;
    QRadioButton *FlatshadingCheck;
    QRadioButton *GroundshadingCheck;
    QCheckBox *TextureCheck;
    QLabel *TimeRecord;
    QGroupBox *groupBox_3;
    QRadioButton *MeshCube;
    QRadioButton *MeshSphere;
    QRadioButton *MeshTank;
    QRadioButton *MeshViolin;
    QRadioButton *MeshRec;
    QRadioButton *MeshTeapot;
    QPushButton *minusButton;
    QPushButton *plusButton;
    QLabel *label_14;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SoftRenderClass)
    {
        if (SoftRenderClass->objectName().isEmpty())
            SoftRenderClass->setObjectName(QStringLiteral("SoftRenderClass"));
        SoftRenderClass->resize(1079, 752);
        centralWidget = new QWidget(SoftRenderClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        render_widget = new RenderView(centralWidget);
        render_widget->setObjectName(QStringLiteral("render_widget"));
        render_widget->setGeometry(QRect(20, 10, 800, 600));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(830, 10, 231, 361));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 31, 16));
        QFont font;
        font.setPointSize(9);
        label->setFont(font);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 60, 51, 16));
        label_2->setFont(font);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 90, 51, 16));
        label_3->setFont(font);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 120, 51, 16));
        label_4->setFont(font);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 150, 51, 16));
        label_5->setFont(font);
        DefineButton = new QPushButton(groupBox);
        DefineButton->setObjectName(QStringLiteral("DefineButton"));
        DefineButton->setGeometry(QRect(60, 270, 93, 28));
        posEditx = new QLineEdit(groupBox);
        posEditx->setObjectName(QStringLiteral("posEditx"));
        posEditx->setGeometry(QRect(80, 30, 31, 16));
        posEdity = new QLineEdit(groupBox);
        posEdity->setObjectName(QStringLiteral("posEdity"));
        posEdity->setGeometry(QRect(130, 30, 31, 16));
        posEditz = new QLineEdit(groupBox);
        posEditz->setObjectName(QStringLiteral("posEditz"));
        posEditz->setGeometry(QRect(180, 30, 31, 16));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(110, 30, 21, 16));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(160, 30, 21, 16));
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(110, 60, 21, 16));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(160, 60, 21, 16));
        tarEditz = new QLineEdit(groupBox);
        tarEditz->setObjectName(QStringLiteral("tarEditz"));
        tarEditz->setGeometry(QRect(180, 60, 31, 16));
        tarEditx = new QLineEdit(groupBox);
        tarEditx->setObjectName(QStringLiteral("tarEditx"));
        tarEditx->setGeometry(QRect(80, 60, 31, 16));
        tarEdity = new QLineEdit(groupBox);
        tarEdity->setObjectName(QStringLiteral("tarEdity"));
        tarEdity->setGeometry(QRect(130, 60, 31, 16));
        fovEdit = new QLineEdit(groupBox);
        fovEdit->setObjectName(QStringLiteral("fovEdit"));
        fovEdit->setGeometry(QRect(70, 90, 51, 21));
        nearEdit = new QLineEdit(groupBox);
        nearEdit->setObjectName(QStringLiteral("nearEdit"));
        nearEdit->setGeometry(QRect(70, 120, 51, 21));
        farEdit = new QLineEdit(groupBox);
        farEdit->setObjectName(QStringLiteral("farEdit"));
        farEdit->setGeometry(QRect(70, 150, 51, 21));
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(60, 30, 16, 16));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_10->setFont(font1);
        label_11 = new QLabel(groupBox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(220, 30, 21, 16));
        label_11->setFont(font1);
        label_12 = new QLabel(groupBox);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(60, 60, 20, 20));
        label_12->setFont(font1);
        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(220, 60, 21, 16));
        label_13->setFont(font1);
        BackFaceCheck = new QCheckBox(groupBox);
        BackFaceCheck->setObjectName(QStringLiteral("BackFaceCheck"));
        BackFaceCheck->setGeometry(QRect(10, 180, 131, 19));
        BackFaceCheck->setChecked(true);
        WireFrameCheck = new QCheckBox(groupBox);
        WireFrameCheck->setObjectName(QStringLiteral("WireFrameCheck"));
        WireFrameCheck->setGeometry(QRect(10, 210, 101, 19));
        VertexCheck = new QCheckBox(groupBox);
        VertexCheck->setObjectName(QStringLiteral("VertexCheck"));
        VertexCheck->setGeometry(QRect(10, 240, 101, 19));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(850, 540, 191, 121));
        FlatshadingCheck = new QRadioButton(groupBox_2);
        FlatshadingCheck->setObjectName(QStringLiteral("FlatshadingCheck"));
        FlatshadingCheck->setGeometry(QRect(20, 30, 131, 19));
        FlatshadingCheck->setChecked(true);
        GroundshadingCheck = new QRadioButton(groupBox_2);
        GroundshadingCheck->setObjectName(QStringLiteral("GroundshadingCheck"));
        GroundshadingCheck->setGeometry(QRect(20, 60, 141, 19));
        TextureCheck = new QCheckBox(groupBox_2);
        TextureCheck->setObjectName(QStringLiteral("TextureCheck"));
        TextureCheck->setGeometry(QRect(20, 90, 91, 19));
        TimeRecord = new QLabel(centralWidget);
        TimeRecord->setObjectName(QStringLiteral("TimeRecord"));
        TimeRecord->setGeometry(QRect(30, 630, 201, 16));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        TimeRecord->setFont(font2);
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(850, 380, 191, 151));
        MeshCube = new QRadioButton(groupBox_3);
        MeshCube->setObjectName(QStringLiteral("MeshCube"));
        MeshCube->setGeometry(QRect(10, 20, 115, 19));
        MeshCube->setChecked(true);
        MeshSphere = new QRadioButton(groupBox_3);
        MeshSphere->setObjectName(QStringLiteral("MeshSphere"));
        MeshSphere->setGeometry(QRect(10, 40, 115, 19));
        MeshTank = new QRadioButton(groupBox_3);
        MeshTank->setObjectName(QStringLiteral("MeshTank"));
        MeshTank->setGeometry(QRect(10, 60, 115, 19));
        MeshViolin = new QRadioButton(groupBox_3);
        MeshViolin->setObjectName(QStringLiteral("MeshViolin"));
        MeshViolin->setGeometry(QRect(10, 100, 115, 19));
        MeshRec = new QRadioButton(groupBox_3);
        MeshRec->setObjectName(QStringLiteral("MeshRec"));
        MeshRec->setGeometry(QRect(10, 120, 161, 19));
        MeshTeapot = new QRadioButton(groupBox_3);
        MeshTeapot->setObjectName(QStringLiteral("MeshTeapot"));
        MeshTeapot->setGeometry(QRect(10, 80, 115, 19));
        minusButton = new QPushButton(centralWidget);
        minusButton->setObjectName(QStringLiteral("minusButton"));
        minusButton->setGeometry(QRect(520, 640, 31, 28));
        plusButton = new QPushButton(centralWidget);
        plusButton->setObjectName(QStringLiteral("plusButton"));
        plusButton->setGeometry(QRect(570, 640, 31, 28));
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(450, 640, 41, 16));
        SoftRenderClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SoftRenderClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1079, 26));
        SoftRenderClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SoftRenderClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        SoftRenderClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SoftRenderClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        SoftRenderClass->setStatusBar(statusBar);

        retranslateUi(SoftRenderClass);

        QMetaObject::connectSlotsByName(SoftRenderClass);
    } // setupUi

    void retranslateUi(QMainWindow *SoftRenderClass)
    {
        SoftRenderClass->setWindowTitle(QApplication::translate("SoftRenderClass", "SoftRender", 0));
        groupBox->setTitle(QApplication::translate("SoftRenderClass", "CameraAttrib", 0));
        label->setText(QApplication::translate("SoftRenderClass", "Pos", 0));
        label_2->setText(QApplication::translate("SoftRenderClass", "Target ", 0));
        label_3->setText(QApplication::translate("SoftRenderClass", "Fov", 0));
        label_4->setText(QApplication::translate("SoftRenderClass", "Near", 0));
        label_5->setText(QApplication::translate("SoftRenderClass", "Far", 0));
        DefineButton->setText(QApplication::translate("SoftRenderClass", "Define", 0));
        posEditx->setText(QApplication::translate("SoftRenderClass", "0", 0));
        posEdity->setText(QApplication::translate("SoftRenderClass", "0", 0));
        posEditz->setText(QApplication::translate("SoftRenderClass", "3", 0));
        label_6->setText(QApplication::translate("SoftRenderClass", " ,", 0));
        label_7->setText(QApplication::translate("SoftRenderClass", " ,", 0));
        label_8->setText(QApplication::translate("SoftRenderClass", " ,", 0));
        label_9->setText(QApplication::translate("SoftRenderClass", " ,", 0));
        tarEditz->setText(QApplication::translate("SoftRenderClass", "0", 0));
        tarEditx->setText(QApplication::translate("SoftRenderClass", "0", 0));
        tarEdity->setText(QApplication::translate("SoftRenderClass", "0", 0));
        fovEdit->setText(QApplication::translate("SoftRenderClass", "45", 0));
        nearEdit->setText(QApplication::translate("SoftRenderClass", "0.1", 0));
        farEdit->setText(QApplication::translate("SoftRenderClass", "1000", 0));
        label_10->setText(QApplication::translate("SoftRenderClass", "\357\274\210", 0));
        label_11->setText(QApplication::translate("SoftRenderClass", "\357\274\211", 0));
        label_12->setText(QApplication::translate("SoftRenderClass", "\357\274\210", 0));
        label_13->setText(QApplication::translate("SoftRenderClass", "\357\274\211", 0));
        BackFaceCheck->setText(QApplication::translate("SoftRenderClass", "BackFace cull", 0));
        WireFrameCheck->setText(QApplication::translate("SoftRenderClass", "WireFrame", 0));
        VertexCheck->setText(QApplication::translate("SoftRenderClass", "Vertices", 0));
        groupBox_2->setTitle(QApplication::translate("SoftRenderClass", "Shading", 0));
        FlatshadingCheck->setText(QApplication::translate("SoftRenderClass", "Flat Shading", 0));
        GroundshadingCheck->setText(QApplication::translate("SoftRenderClass", "Ground Shading", 0));
        TextureCheck->setText(QApplication::translate("SoftRenderClass", "Texture", 0));
        TimeRecord->setText(QApplication::translate("SoftRenderClass", "Time per frame:", 0));
        groupBox_3->setTitle(QApplication::translate("SoftRenderClass", "Mesh", 0));
        MeshCube->setText(QApplication::translate("SoftRenderClass", "Cube", 0));
        MeshSphere->setText(QApplication::translate("SoftRenderClass", "Sphere", 0));
        MeshTank->setText(QApplication::translate("SoftRenderClass", "Tank", 0));
        MeshViolin->setText(QApplication::translate("SoftRenderClass", "Violin Box", 0));
        MeshRec->setText(QApplication::translate("SoftRenderClass", "Texture Rectangle", 0));
        MeshTeapot->setText(QApplication::translate("SoftRenderClass", "Teapot", 0));
        minusButton->setText(QApplication::translate("SoftRenderClass", "-", 0));
        plusButton->setText(QApplication::translate("SoftRenderClass", "+", 0));
        label_14->setText(QApplication::translate("SoftRenderClass", "-1", 0));
    } // retranslateUi

};

namespace Ui {
    class SoftRenderClass: public Ui_SoftRenderClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOFTRENDER_H
