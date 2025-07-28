#include "StudentInfoWidget.h"
#include<QSqlQuery>
#include<QDialog>
#include<QMessageBox>
#include<QFormLayout>
#include<QLineEdit>
#include<QComboBox>
#include<QDateEdit>
#include<QLabel>
#include<QBuffer>
#include<QFileDialog>
#include<QSqlError>
StudentInfoWidget::StudentInfoWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(100);
	// 性别列代理
	ComboBoxDelegate* genderDelegate = new ComboBoxDelegate(this);
	genderDelegate->setItems(QStringList() << "男" << "女");
	ui.tableWidget->setItemDelegateForColumn(2, genderDelegate);
	// 进度列代理
	ComboBoxDelegate* progressDelegate = new ComboBoxDelegate(this);
	progressDelegate->setItems(QStringList() << "0%" << "20%" << "40%" << "60%" << "80%" << "100%");
	ui.tableWidget->setItemDelegateForColumn(6, progressDelegate);
	// 日期列代理
	ui.tableWidget->setItemDelegateForColumn(3, new DateEditDelegate(this));
	ui.tableWidget->setItemDelegateForColumn(4, new DateEditDelegate(this));
	// 图片列代理
	ui.tableWidget->setItemDelegateForColumn(7, new ImageDelegate(this));
	refreshTable();
}

StudentInfoWidget::~StudentInfoWidget()
{

}

void StudentInfoWidget::on_btnAdd_clicked()
{
	QDialog dialog(this);//创建对话框
	dialog.setWindowTitle(tr("添加学生信息"));
	dialog.resize(600, 400);
	//初始化布局
	QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
	QHBoxLayout* hlayout = new QHBoxLayout();
	mainLayout->addLayout(hlayout);
	//添加表单(左)和照片(右)区域
	QGroupBox* formGroup = creatFromGroup();
	QGroupBox* photoGroup = creatPhotoGroup();
	hlayout->addWidget(formGroup, 1);//数字控制比例
	hlayout->addWidget(photoGroup, 1);
	//添加按钮
	QHBoxLayout* btnLayout = new QHBoxLayout();
	QPushButton* btnConfirm = new QPushButton(tr("确定"));
	QPushButton* btnCancel = new QPushButton(tr("取消"));
	//配置按钮
	btnConfirm->setFixedWidth(150);
	btnCancel->setFixedWidth(150);
	//添加按钮到布局
	btnLayout->addStretch();//弹簧
	btnLayout->addWidget(btnConfirm);
	btnLayout->addWidget(btnCancel);
	btnLayout->addStretch();//弹簧
	mainLayout->addLayout(btnLayout);
	//连接信号槽
	connect(btnConfirm, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
	if (dialog.exec() == QDialog::Accepted) handleDialogAccept(formGroup, photoGroup);
}

void StudentInfoWidget::on_btnDeleteItem_clicked()
{
	auto selected = ui.tableWidget->selectedItems();
	if (selected.isEmpty())
	{
		QMessageBox::warning(this, "警告", "请先选择要删除的单元格！");
		return;
	}
	QSqlDatabase::database().transaction();//开启事务
	foreach(QTableWidgetItem * item, selected)
	{
		int row = item->row();
		int col = item->column();
		QString id = ui.tableWidget->item(row, 0)->text();

		const QStringList columns = { "id", "name", "gender", "birthday",
									 "join_date", "study_goal", "progress", "photo" };
		QSqlQuery query;
		query.prepare(QString("UPDATE studentInfo SET %1 = ? WHERE id = ?").arg(columns[col]));

		query.addBindValue("");//设置为空
		query.addBindValue(id);//设置id

		if (!query.exec()) {
			QSqlDatabase::database().rollback();
			QMessageBox::critical(this, "错误", "更新失败：" + query.lastError().text());
			return;
		}
	}
	QSqlDatabase::database().commit();
	refreshTable();
}

void StudentInfoWidget::on_btnDeleteLine_clicked()
{
	auto selected = ui.tableWidget->selectionModel()->selectedRows();
	if (selected.empty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请先选择要删除的行！"));
		return;
	}
	QSqlDatabase::database().transaction();//开启事务
	foreach(const QModelIndex & index, selected)//拿出每一项
	{
		QString id = ui.tableWidget->item(index.row(), 0)->text();//0是第0列，也就是id
		QSqlQuery query;
		query.prepare("DELETE FROM studentInfo WHERE id = ?");
		query.addBindValue(id);
		if (!query.exec())
		{
			QSqlDatabase::database().rollback();//回滚事务
			QMessageBox::warning(this, tr("警告"), tr("删除失败！") + query.lastError().text());
			return;
		}
	}
	QSqlDatabase::database().commit();//提交事务
	QMessageBox::information(this, tr("提示"), tr("删除成功！"));
	refreshTable();
}

void StudentInfoWidget::refreshTable()
{
	ui.tableWidget->blockSignals(true);//加载数据的时候阻止信号
	ui.tableWidget->setRowCount(0);

	QSqlQuery query("SELECT * FROM studentInfo");
	while (query.next())
	{
		int row = ui.tableWidget->rowCount();//获取当前现有行数
		ui.tableWidget->insertRow(row);//在现有行后插入新的行
		for (int col = 0; col < ui.tableWidget->columnCount(); ++col)
		{
			//获取列数后，根据列数创建QTableWidgetItem对象
			QTableWidgetItem* item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);//设置居中对齐

			if (col == ui.tableWidget->columnCount() - 1) // 处理最后一列
			{ // 处理照片列
				QByteArray photoData = query.value(col).toByteArray();//获取照片数据转化成数组
				if (!photoData.isEmpty())
				{
					QPixmap photo;
					photo.loadFromData(photoData);
					item->setData(Qt::DecorationRole, photo.scaled(100, 100, Qt::KeepAspectRatio));//设置照片显示
					item->setData(Qt::UserRole, photoData);
				}
			}
			else
			{
				item->setText(query.value(col).toString());//查找对应列的值并设置到QTableWidgetItem对象中
			}

			ui.tableWidget->setItem(row, col, item);
		}
	}
	ui.tableWidget->blockSignals(false);//加载数据完成后解除信号阻塞
}

QGroupBox* StudentInfoWidget::creatFromGroup()
{
	QGroupBox* formGroup = new QGroupBox("基本信息");
	QFormLayout* formLayout = new QFormLayout(formGroup);//表单布局
	//初始化控件
	QLineEdit* idEdit = new QLineEdit();
	idEdit->setObjectName("idEdit");
	QLineEdit* nameEdit = new QLineEdit();
	nameEdit->setObjectName("nameEdit");
	QComboBox* genderCombo = new QComboBox();
	genderCombo->setObjectName("genderCombo");
	QDateEdit* birthdayEdit = new QDateEdit(QDate::currentDate());
	birthdayEdit->setObjectName("birthdayEdit");
	QDateEdit* joinDateEdit = new QDateEdit(QDate::currentDate());
	joinDateEdit->setObjectName("joinDateEdit");
	QLineEdit* goalEdit = new QLineEdit();
	goalEdit->setObjectName("goalEdit");
	QComboBox* progressCombo = new QComboBox();
	progressCombo->setObjectName("progressCombo");
	//配置控件
	genderCombo->addItems({ tr("男"), tr("女") });
	progressCombo->addItems({ tr("0%"), tr("20%"), tr("40%"), tr("60%"), tr("80%"), tr("100%") });
	birthdayEdit->setDisplayFormat("yyyy-MM-dd");
	joinDateEdit->setDisplayFormat("yyyy-MM-dd");
	birthdayEdit->setCalendarPopup(true);//设置日期控件弹出日历
	joinDateEdit->setCalendarPopup(true);//设置日期控件弹出日历
	//添加控件到表单布局
	formLayout->addRow(tr("学号"), idEdit);
	formLayout->addRow(tr("姓名"), nameEdit);
	formLayout->addRow(tr("性别"), genderCombo);
	formLayout->addRow(tr("出生日期"), birthdayEdit);
	formLayout->addRow(tr("入学日期"), joinDateEdit);
	formLayout->addRow(tr("学习目标"), goalEdit);
	formLayout->addRow(tr("学习进度"), progressCombo);
	return formGroup;
}

QGroupBox* StudentInfoWidget::creatPhotoGroup()
{
	QGroupBox* photoGroup = new QGroupBox("照片上传");
	QVBoxLayout* photoLayout = new QVBoxLayout(photoGroup);
	//初始化控件
	QLabel* photoLabel = new QLabel();
	QPushButton* photoBtn = new QPushButton(tr("选择照片"));
	//配置控件
	photoLabel->setAlignment(Qt::AlignCenter);
	photoBtn->setMinimumSize(200, 200);
	photoBtn->setFixedSize(100, 40);
	//添加控件到布局
	photoLayout->addWidget(photoLabel);
	photoLayout->addWidget(photoBtn, 0, Qt::AlignHCenter);
	//连接信号槽
	connect(photoBtn, &QPushButton::clicked, [this, photoLabel]() {
		QString fileName = QFileDialog::getOpenFileName(this, tr("选择图片"), "", tr("图片文件 (*.jpg *.png)"));
		if (!fileName.isEmpty())
		{
			QPixmap pixmap(fileName);
			if (!pixmap.isNull())
			{
				pixmap = pixmap.scaled(photoLabel->width() - 30, photoLabel->height() - 30, Qt::KeepAspectRatio);
				photoLabel->setPixmap(pixmap);
				QBuffer buffer(&photoData);//缓存为字节数组
				buffer.open(QIODevice::WriteOnly);
				pixmap.save(&buffer, "PNG");//存到photoData，方便存储到数据库
			}
			else
			{
				QMessageBox::warning(this, tr("警告"), tr("请选择正确的图片文件！"));
			}
		}
		});
	return photoGroup;
}

void StudentInfoWidget::handleDialogAccept(QGroupBox* formGroup, QGroupBox* photoGroup)
{
	//写到数据库
	//获取表单数据
	QLineEdit* idEdit = formGroup->findChild<QLineEdit*>("idEdit");
	QLineEdit* nameEdit = formGroup->findChild<QLineEdit*>("nameEdit");
	QComboBox* genderCombo = formGroup->findChild<QComboBox*>("genderCombo");
	QDateEdit* birthdayEdit = formGroup->findChild<QDateEdit*>("birthdayEdit");
	QDateEdit* joinDateEdit = formGroup->findChild<QDateEdit*>("joinDateEdit");
	QLineEdit* goalEdit = formGroup->findChild<QLineEdit*>("goalEdit");
	QComboBox* progressCombo = formGroup->findChild<QComboBox*>("progressCombo");
	//数据校验
	if (idEdit->text().isEmpty() || nameEdit->text().isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("学号和姓名不能为空！"));
		return;
	}
	//检查学号唯一性
	QSqlQuery query("SELECT * FROM studentinfo WHERE id = ?");
	query.addBindValue(idEdit->text());//绑定参数
	if (query.exec() && query.next())
	{
		QMessageBox::warning(this, tr("警告"), tr("学号已存在！"));
		return;
	}
	//插入数据
	QSqlDatabase::database().transaction();
	QSqlQuery insertQuery;
	insertQuery.prepare(
		"INSERT INTO studentinfo(id, name, gender, birthday, join_date, study_goal, progress, photo) VALUES(?,?,?,?,?,?,?,?)"
	);
	//绑定参数
	insertQuery.addBindValue(idEdit->text());
	insertQuery.addBindValue(nameEdit->text());
	insertQuery.addBindValue(genderCombo->currentText());
	insertQuery.addBindValue(birthdayEdit->date().toString("yyyy-MM-dd"));
	insertQuery.addBindValue(joinDateEdit->date().toString("yyyy-MM-dd"));
	insertQuery.addBindValue(goalEdit->text());
	insertQuery.addBindValue(progressCombo->currentText());
	insertQuery.addBindValue(photoData.isEmpty() ? QVariant() : photoData);
	if (!insertQuery.exec())
	{
		QSqlDatabase::database().rollback();//回滚事务
		QMessageBox::warning(this, tr("警告"), tr("添加失败！") + insertQuery.lastError().text());
	}
	else
	{
		QSqlDatabase::database().commit();//提交事务
		QMessageBox::information(this, tr("提示"), tr("添加学生%1成功！").arg(idEdit->text()));
		refreshTable();
	}
}

