#include "databasemanager.h"
#include<QMessageBox>
Databasemanager& Databasemanager::instance()
{
	static Databasemanager instance;//只能在内部使用
	return instance;
}

void Databasemanager::closeDatabase()
{
	if (db.isOpen())
	{
		db.close();
	}
}

bool Databasemanager::openDatabase(const QString& dbPath)
{
	db.setDatabaseName(dbPath);
	if (db.open())
	{
		return true;

	}
	else
	{
		qDebug() << "无法打开数据库";
		return false;
	}
}

QString Databasemanager::getDbPath() const
{
	return dbPath;
}

void Databasemanager::setDbPath(const QString& Path)
{
	if (Path != dbPath)
	{
		//如果路径发生变化，则关闭数据库，重新打开
		dbPath = Path;
		closeDatabase();
		openDatabase(dbPath);
	}
}

Databasemanager::Databasemanager(QObject* parent)
	: QObject(parent)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	openDatabase(dbPath);
}

Databasemanager::~Databasemanager()
{
	closeDatabase();
}

