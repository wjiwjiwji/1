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
	//选择时间
	QComboBox* yearComboBox;
	QComboBox* weekComboBox;
	QLabel* dateRangeLabel;//显示数据范围
	QPushButton* addButton;//添加课程按钮
	QPushButton* deleteButton;//删除课程按钮
	QPushButton* prevWeekButton;//上一周按钮
	QPushButton* nextWeekButton;//下一周按钮
	//课程数据结构：键为(year,week)，值为课程表数据
	QMap <QPair<int, int>, QVector<QVector<QString>>> scheduleData;
	QStringList times;//时间列表

public:
	ScheduleWidget(QWidget* parent = nullptr);
	~ScheduleWidget();

private:
	Ui::ScheduleWidgetClass ui;
};

