#include "logindialog.h"
#include "ui_logindialog.h"
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include "Setting.h"
#include <QMessageBox>
LoginDialog::LoginDialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::logindialogClass)
{
	ui->setupUi(this);
	checkAndCreateInitialUser(); // 检查数据库是否为空，若为空则插入初始用户
	setWindowTitle("教学管理系统");    // 设置窗口标题
	setWindowIcon(QIcon(":/MainWindow/images/TeachSystem.png"));
	setFixedSize(260, 180);
	// 创建控件
	QLabel* usernameLabel = new QLabel("用户名:", this);
	QLabel* passwordLabel = new QLabel("密   码:", this);
	usernameLineEdit = new QLineEdit(this);
	passwordLineEdit = new QLineEdit(this);
	passwordLineEdit->setEchoMode(QLineEdit::Password); // 密码输入模式
	loginButton = new QPushButton("登录", this);
	cancelButton = new QPushButton("取消", this);
	// 布局
	QGridLayout* mainLayout = new QGridLayout(this);
	mainLayout->addWidget(usernameLabel, 0, 0);
	mainLayout->addWidget(usernameLineEdit, 0, 1);
	mainLayout->addWidget(passwordLabel, 1, 0);
	mainLayout->addWidget(passwordLineEdit, 1, 1);
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(loginButton);
	buttonLayout->addWidget(cancelButton);
	mainLayout->addLayout(buttonLayout, 2, 0, 1, 2); // 跨两列
	setLayout(mainLayout);
	// 连接信号和槽
	connect(loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);
	connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
	// 尝试加载缓存的登录信息
	QString cachedUsername, cachedPassword;
	if (Settings::instance().getCacheEnabled())
	{
		if (loadCredentials(cachedUsername, cachedPassword))
		{
			usernameLineEdit->setText(cachedUsername);
			passwordLineEdit->setText(cachedPassword);
		}
	}
}

LoginDialog::~LoginDialog()
{
	delete ui;
}
void LoginDialog::checkAndCreateInitialUser()
{
	const QString initialUsername = "wjiwjiwji";// 初始用户名和密码
	const QString initialPassword = "124867539";
	QSqlQuery query;
	query.exec("SELECT COUNT(*) FROM users");
	if (query.next() && query.value(0).toInt() == 0)
	{// 检查 users 表是否为空,表为空，插入初始用户
		QString hashedInitialPassword = hashPassword(initialPassword);
		query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
		query.bindValue(":username", initialUsername);
		query.bindValue(":password", hashedInitialPassword);
		if (!query.exec()) qDebug() << "插入初始用户失败:" << query.lastError().text();
	}
}

QString LoginDialog::hashPassword(const QString& password)
{
	// 哈希加密
	QByteArray passwordBytes = password.toUtf8();
	QByteArray hashBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
	return QString(hashBytes.toHex());
}

void LoginDialog::on_loginButton_clicked()
{
	QString username = usernameLineEdit->text();
	QString password = passwordLineEdit->text();

	if (validateUser(username, password)) {
		// 登录成功，保存登录信息
		saveCredentials(username, password);
		// 将当前登录的用户名保存到 Settings 中
		Settings::instance().setLastUser(username);
		accept(); // 关闭对话框
	}
	else  QMessageBox::warning(this, "登录失败", "用户名或密码错误。");
}

bool LoginDialog::validateUser(const QString& username, const QString& password)
{
	QString hashedPassword = hashPassword(password);
	QSqlQuery query;
	query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
	query.bindValue(":username", username);
	query.bindValue(":password", hashedPassword);
	if (!query.exec())
	{
		qDebug() << "错误:" << query.lastError().text();
		return false;
	}
	return query.next();
}

void LoginDialog::saveCredentials(const QString& username, const QString& password)
{
	// 存储用户名和加密后的密码
	Settings::instance().getQSettings().setValue("username", username);
	QString encryptedPassword = encryptPassword(password);
	Settings::instance().getQSettings().setValue("password", encryptedPassword);
}

bool LoginDialog::loadCredentials(QString& username, QString& password)
{
	username = Settings::instance().getQSettings().value("username").toString();
	QString encryptedPassword = Settings::instance().getQSettings().value("password").toString();
	// 如果用户名和加密后的密码不为空，则解密密码
	if (!username.isEmpty() && !encryptedPassword.isEmpty())
	{
		password = decryptPassword(encryptedPassword);
		return true;
	}
	return false;
}
// 加密和解密的密钥
const QByteArray encryptionKey = "your_encryption_key";
// 加密函数
QString LoginDialog::encryptPassword(const QString& password)
{
	QByteArray passwordBytes = password.toUtf8();
	QByteArray encryptedBytes;
	for (int i = 0; i < passwordBytes.size(); ++i)
		encryptedBytes.append(passwordBytes[i] ^ encryptionKey[i % encryptionKey.size()]);
	return encryptedBytes.toBase64();
}
// 解密函数
QString LoginDialog::decryptPassword(const QString& encryptedPassword)
{
	QByteArray encryptedBytes = QByteArray::fromBase64(encryptedPassword.toUtf8());
	QByteArray decryptedBytes;
	for (int i = 0; i < encryptedBytes.size(); ++i)
		decryptedBytes.append(encryptedBytes[i] ^ encryptionKey[i % encryptionKey.size()]);
	return QString::fromUtf8(decryptedBytes);
}