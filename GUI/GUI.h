#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GUI.h"
#include "MyThread.h"

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI(QWidget *parent = Q_NULLPTR);
	void addPoint(double x, double y, double y1);
	void plot();
	MyThread *mThread;

private:
	Ui::GUIClass ui;
	QVector<double> qv_x, qv_y, qv_y1, qv_y2, qv1_x, qv1_y, qv1_y1, qv1_y2;

private slots:
	void on_btn_start_clicked();
	void on_btn_stop_clicked();
	void on_btn_set_params_clicked();

public slots:
	void onMpcIteration(double,double,double);
};