#include "comboboxdelegate.h"
#include <QDebug>

QVariant ColorModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (!index.isValid() || row >= m_colors.size())
        return QVariant();
    else if (role == Qt::BackgroundColorRole)
        return QColor(m_colors[row]);
    else
        return QVariant();
}

//------------------------ColorDelegate------------------------//
QWidget *ColorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QComboBox *editor = new QComboBox(parent);
    ColorModel *model = new ColorModel(items, parent);
    editor->setModel(model);
    return editor;
}

void ColorDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    comboBox->showPopup();
    auto row = index.data(Qt::UserRole).toInt();
    comboBox->setCurrentIndex(row);
}

void ColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    int row = comboBox->currentIndex();
    model->setData(index, row, Qt::UserRole);
}

void ColorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionComboBox style;
    style.rect = option.rect;
    //
    QVariant value = index.data(Qt::UserRole);
    int row = 0;

    if (value.isValid())
        row  = value.toInt();
    //
    QColor color(items[row]);
    painter->fillRect(style.rect, qvariant_cast<QColor>(color));
    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &style, painter);
}

//------------------------LsDelegate------------------------//
QWidget *ListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QComboBox *editor = new QComboBox(parent);
    editor->addItems(items);
    return editor;
}

void ListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(items.indexOf(index.data().toString()));
}

void ListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    model->setData(index, items[comboBox->currentIndex()]);
}

void ListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void ListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionComboBox style;
    style.rect = option.rect;
    QVariant value = index.data();
    if(value.isValid())
        style.currentText = value.toString();
    else
        style.currentText = QString();
    //
    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &style, painter);
}
