#include "ScheduleWidget.h"
#include<QDate>
#include<QHeaderView>
#include<QVBoxLayout>
#include<QSqlQuery>
#include<QMessageBox>
#include<QFormLayout>
#include<QTimeEdit>
#include<QSqlError>
void ScheduleWidget::setupUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* dateLayout = new QHBoxLayout();
	yearComboBox = new QComboBox(this);
	weekComboBox = new QComboBox(this);

	int currentYear = QDate::currentDate().year();
	for (int year = 2020; year <= currentYear + 5; ++year)
		yearComboBox->addItem(QString::number(year), year);

	for (int week = 1; week <= 52; ++week)
		weekComboBox->addItem(QString("第 %1 周").arg(week), week);

	dateRangeLabel = new QLabel(this);
	// 添加周导航按钮
	QPushButton* prevWeekBtn = new QPushButton("上一周", this);
	QPushButton* nextWeekBtn = new QPushButton("下一周", this);
	prevWeekBtn->setFixedWidth(200);
	nextWeekBtn->setFixedWidth(200);

	dateLayout->addWidget(new QLabel("年份：", this));
	dateLayout->addWidget(yearComboBox);
	dateLayout->addWidget(new QLabel("周数：", this));
	dateLayout->addWidget(weekComboBox);
	dateLayout->addWidget(dateRangeLabel);
	dateLayout->addStretch();

	tableWidget = new QTableWidget(this);
	tableWidget->setAlternatingRowColors(true);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	setupTable();

	addButton = new QPushButton("添加课程", this);
	deleteButton = new QPushButton("删除课程", this);
	addButton->setFixedWidth(200);
	deleteButton->setFixedWidth(200);

	connect(yearComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ScheduleWidget::loadSchedule);
	connect(weekComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ScheduleWidget::loadSchedule);
	connect(addButton, &QPushButton::clicked, this, &ScheduleWidget::addCourse);
	connect(deleteButton, &QPushButton::clicked, this, &ScheduleWidget::deleteCourse);
	connect(prevWeekBtn, &QPushButton::clicked, this, &ScheduleWidget::showPreviousWeek);
	connect(nextWeekBtn, &QPushButton::clicked, this, &ScheduleWidget::showNextWeek);
	connect(tableWidget, &QTableWidget::itemChanged, this, &ScheduleWidget::handleItemChanged);
	connect(yearComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScheduleWidget::updateTableDates);
	connect(weekComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ScheduleWidget::updateTableDates);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(prevWeekBtn);
	buttonLayout->addWidget(nextWeekBtn);
	buttonLayout->addWidget(addButton);
	buttonLayout->addWidget(deleteButton);
	buttonLayout->addStretch();
	mainLayout->addLayout(dateLayout);
	mainLayout->addWidget(tableWidget);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}
int ScheduleWidget::customWeekNumber(const QDate& date)
{
	QDate startOfYear(date.year(), 1, 1);//一年的第一天
	int dayOfWeek = startOfYear.dayOfWeek();//一年的第一天是星期几
	int days = startOfYear.daysTo(date);//今天距离一年的第一天有多少天
	int week = (days + dayOfWeek - 1) / 7 + 1;//今天是本年的第几周
	return 0;
}
void ScheduleWidget::setupTable()
{
	times = { "上午1", "上午2", "下午1", "下午2", "晚上1", "晚上2" };
	tableWidget->setRowCount(7);
	tableWidget->setColumnCount(times.count());
	tableWidget->setHorizontalHeaderLabels(times);//设置表头
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置表头宽度自适应
	tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置表头宽度自适应
	tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);//双击单元格可编辑
}

void ScheduleWidget::updateTableDates()
{
	// 重新获取当前年份和周数
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();

	// 重新计算日期范围
	QPair<QDate, QDate> weekRange = getWeekRange(year, week);
	QDate startDate = weekRange.first;

	// 更新垂直表头的日期部分
	QStringList verticalHeaders;
	QStringList days = { "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日" };

	for (int i = 0; i < days.count(); ++i)
	{
		QDate currentDate = startDate.addDays(i);
		verticalHeaders.append(QString("%1\n%2").arg(days[i]).arg(currentDate.toString("MM/dd")));
	}

	// 更新表格的垂直表头
	tableWidget->setVerticalHeaderLabels(verticalHeaders);

	// 加载对应周的课程数据
	loadSchedule();
}

void ScheduleWidget::loadSchedule()
{
	tableWidget->blockSignals(false);
	tableWidget->clearContents();

	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate> weekRange = getWeekRange(year, week);
	QDate startDate = weekRange.first;
	QDate endDate = weekRange.second;
	dateRangeLabel->setText(startDate.toString("yyyy-MM-dd") + "到" + endDate.toString("yyyy-MM-dd"));

	QVector<QVector<QString>> courses(7, QVector<QString>(times.count(), ""));

	QSqlQuery query;
	query.prepare("SELECT date, time, course_name FROM schedule WHERE date BETWEEN ? AND ?");
	query.addBindValue(startDate.toString("yyyy-MM-dd"));
	query.addBindValue(endDate.toString("yyyy-MM-dd"));
	if (query.exec())
	{
		while (query.next())
		{
			QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");//第0项日期
			QString time = query.value(1).toString();
			int dayIndex = startDate.daysTo(date);
			int timeIndex = times.indexOf(time);//返回索引
			if (dayIndex >= 0 && dayIndex < 7 && timeIndex != -1)
			{
				//如果是本周的课程，则添加到课程表中
				courses[dayIndex][timeIndex] = query.value(2).toString();
			}
		}
	}

	for (int day = 0; day < 7; ++day)
	{
		for (int time = 0; time < times.count(); ++time)
		{
			QTableWidgetItem* item = new QTableWidgetItem(courses[day][time]);//创建单元格
			item->setTextAlignment(Qt::AlignCenter);//居中显示
			tableWidget->setItem(day, time, item);
		}
	}

	tableWidget->blockSignals(false);
}

void ScheduleWidget::addCourse() {
	int dayIndex = tableWidget->currentRow();
	int timeIndex = tableWidget->currentColumn();
	if (dayIndex == -1 || timeIndex == -1) { // 验证选中位置有效性
		QMessageBox::warning(this, "错误", "请先选择一个时间段！");
		return;
	}
	// 检查时间段是否被占用
	if (!tableWidget->item(dayIndex, timeIndex)->text().isEmpty()) {
		QMessageBox::warning(this, "错误", "该时间段已被占用！");
		return;
	}
	QDialog dialog(this);// 创建自定义对话框
	dialog.setWindowTitle("添加课程");
	QFormLayout layout(&dialog);
	QComboBox nameCombo;// 学生姓名下拉框
	QSqlQuery nameQuery("SELECT name FROM studentInfo");
	while (nameQuery.next()) nameCombo.addItem(nameQuery.value(0).toString());
	// 时间映射表：列索引 -> 默认时间
	QMap<int, QTime> timePresets = {// 上午1到晚上2
		{0, QTime(9, 0)},{1, QTime(11, 0)}, {2, QTime(14, 0)},
		{3, QTime(16, 0)},{4, QTime(19, 0)},{5, QTime(21, 0)}
	};
	QTimeEdit timeEdit;// 时间选择控件
	timeEdit.setDisplayFormat("HH:mm");
	timeEdit.setTime(timePresets.value(timeIndex)); // 设置列对应默认时间
	layout.addRow("学生姓名:", &nameCombo);
	layout.addRow("课程时间:", &timeEdit);
	QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttons.button(QDialogButtonBox::Ok)->setText("确定");
	buttons.button(QDialogButtonBox::Cancel)->setText("取消");
	layout.addRow(&buttons);
	connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	if (dialog.exec() != QDialog::Accepted) return;
	// 生成组合字符串
	QString courseName = QString("%1,%2").arg(nameCombo.currentText())
		.arg(timeEdit.time().toString("HH:mm"));
	// 获取日期信息
	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate> weekRange = getWeekRange(year, week);
	QDate currentDate = weekRange.first.addDays(dayIndex);
	QString timeSlot = times[timeIndex]; // 原始时间段标识（如"上午1"）
	// 数据库操作
	QSqlQuery query;
	query.prepare("INSERT INTO schedule (date, time, course_name) VALUES (?, ?, ?)");
	query.addBindValue(currentDate.toString("yyyy-MM-dd"));
	query.addBindValue(timeSlot); // 存储原始时间段标识
	query.addBindValue(courseName); // 存储"姓名,HH:mm"格式
	if (!query.exec())
		QMessageBox::critical(this, "错误", "添加失败：" + query.lastError().text());
	else loadSchedule(); // 刷新显示
}

void ScheduleWidget::handleItemChanged(QTableWidgetItem* item)
{
	int day = item->row();
	int timeSlot = item->column();
	QString newCourse = item->text().trimmed();

	int year = yearComboBox->currentData().toInt();
	int week = weekComboBox->currentData().toInt();
	QPair<QDate, QDate> weekRange = getWeekRange(year, week);
	QDate date = weekRange.first.addDays(day);
	QString time = times[timeSlot];

	QSqlQuery query;
	if (newCourse.isEmpty())
	{
		// 删除课程
		query.prepare("DELETE FROM schedule WHERE date = ? AND time = ?");
		query.addBindValue(date.toString("yyyy-MM-dd"));
		query.addBindValue(time);
	}
	else {
		// 使用REPLACE语句更新或插入
		query.prepare("INSERT OR REPLACE INTO schedule (date, time, course_name) VALUES (?, ?, ?)");
		query.addBindValue(date.toString("yyyy-MM-dd"));
		query.addBindValue(time);
		query.addBindValue(newCourse);
	}

	if (!query.exec()) {
		QMessageBox::critical(this, "错误", "操作失败：" + query.lastError().text());
		loadSchedule(); // 恢复数据
	}
}

void ScheduleWidget::deleteCourse()
{
	// 确认删除操作
	QMessageBox confirmBox(this);
	confirmBox.setWindowTitle("确认删除");
	confirmBox.setText("确定要删除该记录吗？");

	// 设置按钮为中文
	QPushButton* yesButton = confirmBox.addButton("确定", QMessageBox::YesRole);
	QPushButton* noButton = confirmBox.addButton("取消", QMessageBox::NoRole);

	// 设置默认按钮
	confirmBox.setDefaultButton(noButton);

	// 显示对话框并等待用户选择
	confirmBox.exec();

	if (confirmBox.clickedButton() == yesButton) {
		int dayIndex = tableWidget->currentRow();
		int timeIndex = tableWidget->currentColumn();

		if (dayIndex == -1 || timeIndex == -1) {
			QMessageBox::warning(this, "错误", "请先选择一个时间段！");
			return;
		}

		QTableWidgetItem* item = tableWidget->item(dayIndex, timeIndex);
		if (!item || item->text().isEmpty()) {
			QMessageBox::warning(this, "错误", "该时间段没有课程！");
			return;
		}

		int year = yearComboBox->currentData().toInt();
		int week = weekComboBox->currentData().toInt();
		QPair<QDate, QDate> weekRange = getWeekRange(year, week);
		QDate currentDate = weekRange.first.addDays(dayIndex);
		QString time = times[timeIndex];

		QSqlQuery query;
		query.prepare("DELETE FROM schedule WHERE date = ? AND time = ?");
		query.addBindValue(currentDate.toString("yyyy-MM-dd"));
		query.addBindValue(time);

		if (!query.exec()) {
			QMessageBox::critical(this, "错误", "删除失败：" + query.lastError().text());
		}
		else {
			loadSchedule();
		}
	}
}

QPair<QDate, QDate> ScheduleWidget::getWeekRange(int year, int week)
{
	QDate startDate(year, 1, 1);
	int daysToSubtract = startDate.dayOfWeek() - Qt::Monday;
	if (daysToSubtract > 0) startDate = startDate.addDays(-daysToSubtract);
	QDate weekStart = startDate.addDays((week - 1) * 7);
	QDate weekEnd = weekStart.addDays(6);
	return qMakePair(weekStart, weekEnd);
}
ScheduleWidget::ScheduleWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setupUI();

	int currentYear = QDate::currentDate().year();//初始化为当前年份
	int currentWeek = customWeekNumber(QDate::currentDate());//初始化为当前周数
	yearComboBox->setCurrentText(QString::number(currentYear));
	weekComboBox->setCurrentText(QString("第%1周").arg(currentWeek));

	loadSchedule();
}

ScheduleWidget::~ScheduleWidget()
{
}

void ScheduleWidget::showPreviousWeek()
{
	int currentWeek = weekComboBox->currentIndex();
	int currentYear = yearComboBox->currentIndex();

	if (currentWeek > 0) {
		weekComboBox->setCurrentIndex(currentWeek - 1);
	}
	else {
		if (yearComboBox->currentIndex() > 0) {
			yearComboBox->setCurrentIndex(currentYear - 1);
			// 跳转到上一年最后一周（第52周）
			weekComboBox->setCurrentIndex(51);
		}
	}
}
void ScheduleWidget::showNextWeek()
{
	int currentWeek = weekComboBox->currentIndex();
	int currentYear = yearComboBox->currentIndex();

	if (currentWeek < 51) {
		weekComboBox->setCurrentIndex(currentWeek + 1);
	}
	else {
		if (yearComboBox->currentIndex() < yearComboBox->count() - 1) {
			yearComboBox->setCurrentIndex(currentYear + 1);
			weekComboBox->setCurrentIndex(0);
		}
	}
}