#pragma once

#include <QWidget>
#include "ui_ScheduleWidget.h"
#include<QTableWidget>
#include<QLabel>
#include<QComboBox>
#include<QPushButton>
#include<QTableWidgetItem>

class ScheduleWidget : public QWidget
{
	Q_OBJECT
private:
	void setupUI();
	int customWeekNumber(const QDate& date);
	void setupTable();
	void loadSchedule();
	void addCourse();
	void handleItemChanged(QTableWidgetItem* item);
	void deleteCourse();
	void showPreviousWeek();
	void showNextWeek();
	void updateTableDates();
	QPair<QDate, QDate> getWeekRange(int year, int week);

	QTableWidget* tableWidget;
	//ѡ��ʱ��
	QComboBox* yearComboBox;
	QComboBox* weekComboBox;
	QLabel* dateRangeLabel;//��ʾ���ݷ�Χ
	QPushButton* addButton;//��ӿγ̰�ť
	QPushButton* deleteButton;//ɾ���γ̰�ť
	QPushButton* prevWeekButton;//��һ�ܰ�ť
	QPushButton* nextWeekButton;//��һ�ܰ�ť
	//�γ����ݽṹ����Ϊ(year,week)��ֵΪ�γ̱�����
	QMap <QPair<int, int>, QVector<QVector<QString>>> scheduleData;
	QStringList times;//ʱ���б�

public:
	ScheduleWidget(QWidget* parent = nullptr);
	~ScheduleWidget();

private:
	Ui::ScheduleWidgetClass ui;
};

