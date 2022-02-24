//
// Created by headass on 2022-02-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_main_win.h" resolved

#include "main_win.h"
//#include "ui_main_win.h"


main_win::main_win(QWidget *parent) :
        QMainWindow(parent), ui(new Ui_MainWindow) {
    ui->setupUi(this);
}

main_win::~main_win() {
    delete ui;
}

