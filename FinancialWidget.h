#pragma once

#include <QWidget>
#include "ui_FinancialWidget.h"
class QChartView;
#include<QTableWidget>
#include<QComboBox>
#include<QPushButton>
#include<QDateEdit>
class FinancialWidget : public QWidget
{
	Q_OBJECT

public:
	FinancialWidget(QWidget* parent = nullptr);
	~FinancialWidget();

private:
	void setupUI();
	void loadFinancialRecords();
	void populateStudentComboBox();
	void addRecord();
	void updateChart();
	void updatePieChart();
	void editRecord();
	void deleteRecord();
	QChartView* pieChartView;
	QTableWidget* tableWidget;
	QComboBox* studentComboBox;
	QChartView* chartView;
	QDateEdit* startDateEdit;
	QDateEdit* endDateEdit;
	Ui::FinancialWidgetClass ui;
};

