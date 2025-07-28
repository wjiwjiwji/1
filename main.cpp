#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include"databasemanager.h"
#include<QFile>
#include"logindialog.h"
int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	Databasemanager::instance();
	QFile styleFile(":/new/style/style/style.qss");
	if (styleFile.open(QFile::ReadOnly))
	{
		QString styleSheet = QString(styleFile.readAll());
		app.setStyleSheet(styleSheet);
		styleFile.close();
	}
	else
	{
		qWarning() << "Failed to load style sheet:" << styleFile.errorString();
	}
	LoginDialog loginDlg;
	if (loginDlg.exec() == QDialog::Accepted)
	{
		MainWindow window;
		window.show();
		return app.exec();
	}
	return 0;
}
