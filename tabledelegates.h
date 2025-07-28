#pragma once
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QBuffer>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QFileDialog>
class ComboBoxDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ComboBoxDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
	// ���������˵���ѡ��
	void setItems(const QStringList& items)
	{
		m_items = items; // ���������˵���ѡ��
	}
	// �����༭��
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		QComboBox* editor = new QComboBox(parent);
		editor->addItems(m_items); // ���ѡ������˵�
		return editor;
	}
private:
	QStringList m_items; // �洢�����˵���ѡ��
};

class DateEditDelegate : public QStyledItemDelegate {
public:
	explicit DateEditDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override {
		QDateEdit* editor = new QDateEdit(parent);
		editor->setDisplayFormat("yyyy-MM-dd"); // �������ڸ�ʽ
		editor->setCalendarPopup(true);        // ������������
		return editor;
	}
};

class ImageDelegate : public QStyledItemDelegate {
public:
	explicit ImageDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override {
		Q_UNUSED(option); Q_UNUSED(index);
		QLabel* editor = new QLabel(parent);
		return editor;
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
		QLabel* label = qobject_cast<QLabel*>(editor);
		if (label) {
			QByteArray imageData;
			QPixmap pixmap = label->pixmap(); // ֱ�ӻ�ȡ QPixmap ����
			if (!pixmap.isNull()) {
				QBuffer buffer(&imageData);
				buffer.open(QIODevice::WriteOnly);
				pixmap.save(&buffer, "PNG"); // ��ͼƬ����Ϊ PNG ��ʽ�Ķ���������
			}
			model->setData(index, imageData, Qt::UserRole);// �����������ݱ��浽ģ����
		}
	}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QByteArray imageData = index.data(Qt::UserRole).toByteArray();
		if (imageData.isEmpty()) {
			QStyledItemDelegate::paint(painter, option, index);
			return;
		}
		QPixmap pixmap;
		pixmap.loadFromData(imageData);
		if (pixmap.isNull()) {
			QStyledItemDelegate::paint(painter, option, index);
			return;
		}
		// ����ͼƬ
		QRect rect = option.rect;
		QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio); // ����ͼƬ
		painter->drawPixmap(rect, scaledPixmap);
	}
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
		const QModelIndex& index) override {
		if (event->type() == QEvent::MouseButtonDblClick) {
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton) {
				// �����ļ��Ի���ѡ����ͼƬ
				QString imagePath = QFileDialog::getOpenFileName(
					nullptr, "ѡ��ͼƬ", "", "ͼƬ�ļ� (*.png *.jpg *.bmp)"
				);
				if (!imagePath.isEmpty()) {// ����û�ѡ������Ч��ͼƬ·��
					// ��ͼƬ�ļ�����Ϊ����������
					QFile file(imagePath);
					if (file.open(QIODevice::ReadOnly)) {
						QByteArray imageData = file.readAll();
						file.close();
						model->setData(index, imageData, Qt::UserRole);// �����������ݱ��浽ģ����
					}
				}
				// ����û�ȡ��ѡ�񣬱���ԭ����ͼƬ
				return true; // �¼��Ѵ���
			}
		}
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}
};