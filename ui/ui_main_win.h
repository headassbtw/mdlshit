/********************************************************************************
** Form generated from reading UI file 'main_winupvOEu.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAIN_WINUPVOEU_H
#define MAIN_WINUPVOEU_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionAbout_Program;
    QAction *actionMdlBrowse;
    QAction *actionFUCK;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_5;
    QFrame *mdlFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *mdl;
    QPlainTextEdit *mdlTextBox;
    QPushButton *mdlBrowseButton;
    QFrame *vtxFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *vtx;
    QPlainTextEdit *vtxTextBox;
    QPushButton *vtxBrowseButton;
    QFrame *vvdFrame;
    QHBoxLayout *horizontalLayout_3;
    QLabel *vvd;
    QPlainTextEdit *vvdTextBox;
    QPushButton *vvdBrowseButton;
    QFrame *phyFrame;
    QHBoxLayout *horizontalLayout_4;
    QLabel *phy;
    QPlainTextEdit *phyTextBox;
    QPushButton *phyBrowseButton;
    QFrame *line_2;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_6;
    QFrame *outFrame;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QPlainTextEdit *outTextBox;
    QPushButton *outBrowseButton;
    QHBoxLayout *horizontalLayout_5;
    QFrame *frame_3;
    QCheckBox *animDisableCheck;
    QLabel *label_8;
    QFrame *line;
    QPushButton *convertButton;
    QMenuBar *menubar;
    QMenu *menuHe;
    QMenu *menuAbout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(786, 530);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(640, 530));
        MainWindow->setMaximumSize(QSize(16777215, 530));
        MainWindow->setSizeIncrement(QSize(1, 1));
        MainWindow->setBaseSize(QSize(900, 700));
        QIcon icon;
        icon.addFile(QString::fromUtf8("../mdlshit/product.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setAutoFillBackground(false);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionAbout_Program = new QAction(MainWindow);
        actionAbout_Program->setObjectName(QString::fromUtf8("actionAbout_Program"));
        actionMdlBrowse = new QAction(MainWindow);
        actionMdlBrowse->setObjectName(QString::fromUtf8("actionMdlBrowse"));
        actionFUCK = new QAction(MainWindow);
        actionFUCK->setObjectName(QString::fromUtf8("actionFUCK"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(89);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        centralwidget->setMinimumSize(QSize(0, 0));
        centralwidget->setSizeIncrement(QSize(1, 1));
        centralwidget->setBaseSize(QSize(800, 600));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(6, 6, 6, 6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy2);
        frame->setMinimumSize(QSize(270, 0));
        frame->setMaximumSize(QSize(16777215, 270));
        frame->setContextMenuPolicy(Qt::NoContextMenu);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(2);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(6, 6, 6, 6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(0, 30));
        label_5->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("Squares Bold"));
        label_5->setFont(font);
        label_5->setLineWidth(1);
        label_5->setTextFormat(Qt::MarkdownText);
        label_5->setScaledContents(false);
        label_5->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout_3->addWidget(label_5);

        mdlFrame = new QFrame(frame);
        mdlFrame->setObjectName(QString::fromUtf8("mdlFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(mdlFrame->sizePolicy().hasHeightForWidth());
        mdlFrame->setSizePolicy(sizePolicy3);
        mdlFrame->setMaximumSize(QSize(16777215, 50));
        mdlFrame->setFrameShape(QFrame::StyledPanel);
        mdlFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(mdlFrame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(6, 6, 6, 6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mdl = new QLabel(mdlFrame);
        mdl->setObjectName(QString::fromUtf8("mdl"));
        sizePolicy3.setHeightForWidth(mdl->sizePolicy().hasHeightForWidth());
        mdl->setSizePolicy(sizePolicy3);
        mdl->setMinimumSize(QSize(40, 30));
        mdl->setMaximumSize(QSize(40, 30));
        mdl->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(mdl);

        mdlTextBox = new QPlainTextEdit(mdlFrame);
        mdlTextBox->setObjectName(QString::fromUtf8("mdlTextBox"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(mdlTextBox->sizePolicy().hasHeightForWidth());
        mdlTextBox->setSizePolicy(sizePolicy4);
        mdlTextBox->setInputMethodHints(Qt::ImhNone);
        mdlTextBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mdlTextBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout->addWidget(mdlTextBox);

        mdlBrowseButton = new QPushButton(mdlFrame);
        mdlBrowseButton->setObjectName(QString::fromUtf8("mdlBrowseButton"));
        mdlBrowseButton->setMinimumSize(QSize(90, 30));
        mdlBrowseButton->setMaximumSize(QSize(90, 30));

        horizontalLayout->addWidget(mdlBrowseButton);


        verticalLayout_3->addWidget(mdlFrame);

        vtxFrame = new QFrame(frame);
        vtxFrame->setObjectName(QString::fromUtf8("vtxFrame"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(vtxFrame->sizePolicy().hasHeightForWidth());
        vtxFrame->setSizePolicy(sizePolicy5);
        vtxFrame->setMaximumSize(QSize(16777215, 50));
        vtxFrame->setFrameShape(QFrame::StyledPanel);
        vtxFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(vtxFrame);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        vtx = new QLabel(vtxFrame);
        vtx->setObjectName(QString::fromUtf8("vtx"));
        sizePolicy3.setHeightForWidth(vtx->sizePolicy().hasHeightForWidth());
        vtx->setSizePolicy(sizePolicy3);
        vtx->setMinimumSize(QSize(40, 0));
        vtx->setMaximumSize(QSize(40, 16777215));
        vtx->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(vtx);

        vtxTextBox = new QPlainTextEdit(vtxFrame);
        vtxTextBox->setObjectName(QString::fromUtf8("vtxTextBox"));
        sizePolicy4.setHeightForWidth(vtxTextBox->sizePolicy().hasHeightForWidth());
        vtxTextBox->setSizePolicy(sizePolicy4);
        vtxTextBox->setInputMethodHints(Qt::ImhNone);
        vtxTextBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        vtxTextBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout_2->addWidget(vtxTextBox);

        vtxBrowseButton = new QPushButton(vtxFrame);
        vtxBrowseButton->setObjectName(QString::fromUtf8("vtxBrowseButton"));
        vtxBrowseButton->setMinimumSize(QSize(90, 30));
        vtxBrowseButton->setMaximumSize(QSize(90, 30));

        horizontalLayout_2->addWidget(vtxBrowseButton);


        verticalLayout_3->addWidget(vtxFrame);

        vvdFrame = new QFrame(frame);
        vvdFrame->setObjectName(QString::fromUtf8("vvdFrame"));
        sizePolicy3.setHeightForWidth(vvdFrame->sizePolicy().hasHeightForWidth());
        vvdFrame->setSizePolicy(sizePolicy3);
        vvdFrame->setMaximumSize(QSize(16777215, 50));
        vvdFrame->setFrameShape(QFrame::StyledPanel);
        vvdFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(vvdFrame);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        vvd = new QLabel(vvdFrame);
        vvd->setObjectName(QString::fromUtf8("vvd"));
        sizePolicy3.setHeightForWidth(vvd->sizePolicy().hasHeightForWidth());
        vvd->setSizePolicy(sizePolicy3);
        vvd->setMinimumSize(QSize(40, 30));
        vvd->setMaximumSize(QSize(40, 30));
        vvd->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(vvd);

        vvdTextBox = new QPlainTextEdit(vvdFrame);
        vvdTextBox->setObjectName(QString::fromUtf8("vvdTextBox"));
        sizePolicy4.setHeightForWidth(vvdTextBox->sizePolicy().hasHeightForWidth());
        vvdTextBox->setSizePolicy(sizePolicy4);
        vvdTextBox->setInputMethodHints(Qt::ImhNone);
        vvdTextBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        vvdTextBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout_3->addWidget(vvdTextBox);

        vvdBrowseButton = new QPushButton(vvdFrame);
        vvdBrowseButton->setObjectName(QString::fromUtf8("vvdBrowseButton"));
        vvdBrowseButton->setMinimumSize(QSize(90, 30));
        vvdBrowseButton->setMaximumSize(QSize(90, 30));

        horizontalLayout_3->addWidget(vvdBrowseButton);


        verticalLayout_3->addWidget(vvdFrame);

        phyFrame = new QFrame(frame);
        phyFrame->setObjectName(QString::fromUtf8("phyFrame"));
        sizePolicy3.setHeightForWidth(phyFrame->sizePolicy().hasHeightForWidth());
        phyFrame->setSizePolicy(sizePolicy3);
        phyFrame->setMaximumSize(QSize(16777215, 50));
        phyFrame->setFrameShape(QFrame::StyledPanel);
        phyFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(phyFrame);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        phy = new QLabel(phyFrame);
        phy->setObjectName(QString::fromUtf8("phy"));
        sizePolicy3.setHeightForWidth(phy->sizePolicy().hasHeightForWidth());
        phy->setSizePolicy(sizePolicy3);
        phy->setMinimumSize(QSize(40, 30));
        phy->setMaximumSize(QSize(40, 30));
        phy->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(phy);

        phyTextBox = new QPlainTextEdit(phyFrame);
        phyTextBox->setObjectName(QString::fromUtf8("phyTextBox"));
        sizePolicy4.setHeightForWidth(phyTextBox->sizePolicy().hasHeightForWidth());
        phyTextBox->setSizePolicy(sizePolicy4);
        phyTextBox->setInputMethodHints(Qt::ImhNone);
        phyTextBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        phyTextBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout_4->addWidget(phyTextBox);

        phyBrowseButton = new QPushButton(phyFrame);
        phyBrowseButton->setObjectName(QString::fromUtf8("phyBrowseButton"));
        phyBrowseButton->setMinimumSize(QSize(90, 30));
        phyBrowseButton->setMaximumSize(QSize(90, 30));

        horizontalLayout_4->addWidget(phyBrowseButton);


        verticalLayout_3->addWidget(phyFrame);


        verticalLayout_2->addWidget(frame);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setMinimumSize(QSize(0, 9));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_2);

        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(16777215, 200));
        frame_2->setStyleSheet(QString::fromUtf8(""));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        frame_2->setLineWidth(2);
        verticalLayout_4 = new QVBoxLayout(frame_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(6, 6, 6, 6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(0, 40));
        label_6->setMaximumSize(QSize(16777215, 40));
        label_6->setFont(font);
        label_6->setTextFormat(Qt::MarkdownText);

        verticalLayout_4->addWidget(label_6);

        outFrame = new QFrame(frame_2);
        outFrame->setObjectName(QString::fromUtf8("outFrame"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(outFrame->sizePolicy().hasHeightForWidth());
        outFrame->setSizePolicy(sizePolicy6);
        outFrame->setMaximumSize(QSize(16777215, 50));
        outFrame->setAcceptDrops(true);
        outFrame->setAutoFillBackground(true);
        outFrame->setStyleSheet(QString::fromUtf8(""));
        outFrame->setFrameShape(QFrame::StyledPanel);
        outFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(outFrame);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_7 = new QLabel(outFrame);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMinimumSize(QSize(40, 30));
        label_7->setMaximumSize(QSize(40, 30));
        label_7->setScaledContents(false);
        label_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_7);

        outTextBox = new QPlainTextEdit(outFrame);
        outTextBox->setObjectName(QString::fromUtf8("outTextBox"));
        outTextBox->setMinimumSize(QSize(0, 30));
        outTextBox->setMaximumSize(QSize(16777215, 30));
        outTextBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        outTextBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        outTextBox->setLineWrapMode(QPlainTextEdit::NoWrap);
        outTextBox->setReadOnly(false);
        outTextBox->setTextInteractionFlags(Qt::TextEditorInteraction);

        horizontalLayout_6->addWidget(outTextBox);

        outBrowseButton = new QPushButton(outFrame);
        outBrowseButton->setObjectName(QString::fromUtf8("outBrowseButton"));
        outBrowseButton->setMinimumSize(QSize(90, 30));
        outBrowseButton->setMaximumSize(QSize(90, 30));
        outBrowseButton->setStyleSheet(QString::fromUtf8("color:white;"));

        horizontalLayout_6->addWidget(outBrowseButton);


        verticalLayout_4->addWidget(outFrame);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMinimumSize(QSize(0, 80));
        frame_3->setMaximumSize(QSize(16777215, 80));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        animDisableCheck = new QCheckBox(frame_3);
        animDisableCheck->setObjectName(QString::fromUtf8("animDisableCheck"));
        animDisableCheck->setGeometry(QRect(10, 40, 270, 22));
        label_8 = new QLabel(frame_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 10, 270, 20));
        label_8->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(frame_3);

        line = new QFrame(frame_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShadow(QFrame::Raised);
        line->setFrameShape(QFrame::VLine);

        horizontalLayout_5->addWidget(line);

        convertButton = new QPushButton(frame_2);
        convertButton->setObjectName(QString::fromUtf8("convertButton"));
        convertButton->setMinimumSize(QSize(0, 80));
        convertButton->setMaximumSize(QSize(16777215, 80));
        convertButton->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_5->addWidget(convertButton);


        verticalLayout_4->addLayout(horizontalLayout_5);


        verticalLayout_2->addWidget(frame_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 786, 29));
        sizePolicy4.setHeightForWidth(menubar->sizePolicy().hasHeightForWidth());
        menubar->setSizePolicy(sizePolicy4);
        menuHe = new QMenu(menubar);
        menuHe->setObjectName(QString::fromUtf8("menuHe"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuHe->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menuHe->addAction(actionQuit);
        menuHe->addAction(actionFUCK);
        menuAbout->addAction(actionAbout_Program);

        retranslateUi(MainWindow);

        outBrowseButton->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
        actionAbout_Program->setText(QCoreApplication::translate("MainWindow", "About Program", nullptr));
        actionMdlBrowse->setText(QCoreApplication::translate("MainWindow", "MdlBrowse", nullptr));
        actionFUCK->setText(QCoreApplication::translate("MainWindow", "FUCK", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "## Input Files", nullptr));
        mdl->setText(QCoreApplication::translate("MainWindow", "MDL", nullptr));
        mdlBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        vtx->setText(QCoreApplication::translate("MainWindow", "VTX", nullptr));
        vtxBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        vvd->setText(QCoreApplication::translate("MainWindow", "MDL", nullptr));
        vvdBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        phy->setText(QCoreApplication::translate("MainWindow", "PHY", nullptr));
        phyBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "## Output", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Output", nullptr));
        outTextBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "Output File", nullptr));
        outBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        animDisableCheck->setText(QCoreApplication::translate("MainWindow", "Disable Animations", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Options", nullptr));
        convertButton->setText(QCoreApplication::translate("MainWindow", "CONVERT", nullptr));
        menuHe->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuAbout->setTitle(QCoreApplication::translate("MainWindow", "About", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAIN_WINUPVOEU_H
