#pragma once

#include <QObject>
#include<QSqlDatabase>
#include<QDebug>
#include<QSqlError>
class Databasemanager : public QObject
{
	Q_OBJECT

public:
	static Databasemanager& instance();//单例模式
	void closeDatabase();//关闭数据库
	bool openDatabase(const QString& dbPath);//是否打开成功
	QString getDbPath() const;//获取数据库路径
	void setDbPath(const QString& dbPath);//设置数据库路径
private:
	explicit Databasemanager(QObject* parent = nullptr);
	QSqlDatabase db;//数据库
	QString dbPath = "E:/Project/MyQtProject/TeachSystem/TeachSystem/SQLite/axbTechManSys.db";
	~Databasemanager();
};

