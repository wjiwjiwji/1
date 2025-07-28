#pragma once

#include <QWidget>
#include "ui_StudentInfoWidget.h"
#include<QGroupBox>
#include<QByteArray>
#include"tabledelegates.h"
class StudentInfoWidget : public QWidget
{
	Q_OBJECT

public:
	StudentInfoWidget(QWidget* parent = nullptr);
	~StudentInfoWidget();
private slots:
	void on_btnAdd_clicked();
	void on_btnDeleteItem_clicked();
	void on_btnDeleteLine_clicked();
private:
	void refreshTable();
	QGroupBox* creatFromGroup();
	QGroupBox* creatPhotoGroup();
	void handleDialogAccept(QGroupBox* formGroup, QGroupBox* photoGroup);
	Ui::StudentInfoWidgetClass ui;
	QByteArray photoData;
};

