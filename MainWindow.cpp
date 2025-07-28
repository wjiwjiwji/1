#include "MainWindow.h"
#include<QButtonGroup>//按钮组
MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QButtonGroup* btnGp = new QButtonGroup(this);
	//ToolBtn都设为一组
	btnGp->addButton(ui.btnStudentInfo, 0);
	btnGp->addButton(ui.btnSchedule, 1);
	btnGp->addButton(ui.btnFinance, 2);
	btnGp->addButton(ui.btnLesson, 3);
	btnGp->addButton(ui.btnSystemSetting, 4);
	//信号槽连接
	connect(btnGp, &QButtonGroup::idClicked, ui.stackedWidget, &QStackedWidget::setCurrentIndex);
	//默认选中第0，stackedWidget的页索引为0
	btnGp->button(0)->setChecked(true);
	ui.stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
}

