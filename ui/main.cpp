#include <QApplication>
#include "./../src/main.hpp"
#include "main.hpp"
#include "main_win.h"
#include "main.moc"
#include <QObject>
#include <fstream>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <QObject>
#include <qaction.h>
#include <qpushbutton.h>

std::string mdlp;
std::string vtxp;
std::string vvdp;
std::string phyp;
std::string outp;
bool anims;


main_win* win;

bool IsFileGood(const char* path){
    
    std::ifstream tstream = std::ifstream(path);
    if(tstream.good()){
        tstream.close();
        return true;
    }
    else{
        return false;
    }
}


void test(){
    mdlp = win->ui->mdlTextBox->toPlainText().toStdString();
    vtxp = win->ui->vtxTextBox->toPlainText().toStdString();
    vvdp = win->ui->vvdTextBox->toPlainText().toStdString();
    phyp = win->ui->phyTextBox->toPlainText().toStdString();
    outp = win->ui->outTextBox->toPlainText().toStdString();

    bool mdlg = IsFileGood(mdlp.c_str());
    bool vtxg = IsFileGood(vtxp.c_str());
    bool vvdg = IsFileGood(vvdp.c_str());
    bool phyg = IsFileGood(phyp.c_str());
    bool outg = IsFileGood(outp.c_str());


    if(mdlg)
        win->ui->mdlFrame->setStyleSheet(QString::fromUtf8("color:green;"));
    else
        win->ui->mdlFrame->setStyleSheet(QString::fromUtf8("color:red;"));

    if(vtxg)
        win->ui->vtxFrame->setStyleSheet(QString::fromUtf8("color:green;"));
    else
        win->ui->vtxFrame->setStyleSheet(QString::fromUtf8("color:red;"));

    if(vvdg)
        win->ui->vvdFrame->setStyleSheet(QString::fromUtf8("color:green;"));
    else
        win->ui->vvdFrame->setStyleSheet(QString::fromUtf8("color:red;"));

    if(phyg)
        win->ui->phyFrame->setStyleSheet(QString::fromUtf8("color:green;"));
    else
        win->ui->phyFrame->setStyleSheet(QString::fromUtf8("color:red;"));

    if(!outp.empty())
        win->ui->outFrame->setStyleSheet(QString::fromUtf8("color:green;"));
    else
        win->ui->outFrame->setStyleSheet(QString::fromUtf8("color:red;"));
    if(outg)
        win->ui->outFrame->setStyleSheet(QString::fromUtf8("color:yellow;"));
    

    win->ui->convertButton->setEnabled(mdlg && vtxg && vvdg && phyg && (!outp.empty()));
}
QMessageBox* completediag;
void FullSend(){
    int ass = ReadHeader(mdlp.c_str(), vtxp.c_str(), vvdp.c_str(), phyp.c_str(), outp.c_str());
    
    if(ass == 0){ //success
        completediag->setText("Conversion completed!\n\n\n“Where is the texture index?” -BobTheBob");
        completediag->open();
    }
    else if(ass == 1){ //fail
        completediag->setText("Conversion failed!\n\ncheck the log, i guess lol");
        completediag->open();
    }
    else{ //what
        completediag->setText("what");
    }
    
}



int UI::main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    win = new main_win();
    completediag = new QMessageBox(win);
    win->setWindowTitle("fucking bitchass model converter i spent way too goddamn long on this piece of shit goddamn software my grades are suffering");
    a.connect(win->ui->actionQuit, SIGNAL(triggered()),SLOT(quit()));



    QFileDialog mdl_diag = QFileDialog(win->window());
    mdl_diag.setNameFilter("Source engine model (*.mdl)");
    mdl_diag.connect(win->ui->mdlBrowseButton,SIGNAL(pressed()),SLOT(open()));
    win->ui->mdlTextBox->connect(&mdl_diag,SIGNAL(fileSelected(QString)),SLOT(setPlainText(QString)));
    QObject::connect(&mdl_diag,&QFileDialog::fileSelected,test);


    QObject::connect(win->ui->convertButton,&QPushButton::clicked,FullSend);
    

    QFileDialog vtx_diag = QFileDialog(win->window());
    vtx_diag.setNameFilter("Source engine model vertices (*.vtx *.dx80.vtx *.sw.vtx *.dx90.vtx *.xbox.vtx)");
    vtx_diag.connect(win->ui->vtxBrowseButton,SIGNAL(clicked()),SLOT(open()));
    win->ui->vtxTextBox->connect(&vtx_diag,SIGNAL(fileSelected(QString)),SLOT(setPlainText(QString)));
    QObject::connect(&vtx_diag,&QFileDialog::fileSelected,test);

    QFileDialog vvd_diag = QFileDialog(win->window());
    vvd_diag.setNameFilter("Source engine model vertex data (*.vvd)");
    vvd_diag.connect(win->ui->vvdBrowseButton,SIGNAL(clicked()),SLOT(open()));
    win->ui->vvdTextBox->connect(&vvd_diag,SIGNAL(fileSelected(QString)),SLOT(setPlainText(QString)));
    QObject::connect(&vvd_diag,&QFileDialog::fileSelected,test);


    QFileDialog phy_diag = QFileDialog(win->window());
    phy_diag.setNameFilter("Source engine model collision data (*.phy)");
    phy_diag.connect(win->ui->phyBrowseButton,SIGNAL(clicked()),SLOT(open()));
    win->ui->phyTextBox->connect(&phy_diag,SIGNAL(fileSelected(QString)),SLOT(setPlainText(QString)));
    QObject::connect(&phy_diag,&QFileDialog::fileSelected,test);

    QFileDialog out_diag = QFileDialog(win->window());
    out_diag.setNameFilter("Source engine model (*.mdl)");
    out_diag.connect(win->ui->outBrowseButton,SIGNAL(clicked()),SLOT(open()));
    win->ui->outTextBox->connect(&out_diag,SIGNAL(fileSelected(QString)),SLOT(setPlainText(QString)));
    QObject::connect(&out_diag,&QFileDialog::fileSelected,test);
    
    test();
    
    win->show();
    return QApplication::exec();
}
