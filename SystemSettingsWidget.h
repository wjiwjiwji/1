#pragma once

#include <QWidget>
#include "ui_SystemSettingsWidget.h"
#include<QLineEdit>
#include<QPushButton>
#include<QTextEdit>
#include<QCheckBox>
#include<QGridLayout>
class SystemSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SystemSettingsWidget(QWidget* parent = nullptr);
	~SystemSettingsWidget();

private:
	void createUI();
	void browseDatabasePath();
	void loadSettings();
	bool validatePasswordChange();
	void updatePassword();
	void saveSettings();
	QLineEdit* dbPathEdit;
	QPushButton* browseBtn;
	QLineEdit* oldPwdEdit;
	QLineEdit* newPwdEdit;
	QLineEdit* confirmPwdEdit;
	QCheckBox* cacheCheckBox;
	QPushButton* saveBtn;
	QTextEdit* versionInfoEdit;
	QGridLayout* mainLayout;
private:
	Ui::SystemSettingsWidgetClass ui;
};

