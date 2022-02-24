//
// Created by headass on 2022-02-23.
//

#ifndef QT_TEST_MAIN_WIN_H
#define QT_TEST_MAIN_WIN_H
#pragma once
#include <QMainWindow>
#include <ui_main_win.h>


QT_BEGIN_NAMESPACE
namespace Ui { class main_win; }
QT_END_NAMESPACE

class main_win : public QMainWindow {
Q_OBJECT

public:
    explicit main_win(QWidget *parent = nullptr);


    ~main_win() override;
    Ui_MainWindow *ui;
};


#endif //QT_TEST_MAIN_WIN_H
