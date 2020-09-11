#ifndef ColorDelegate_H
#define ColorDelegate_H

#include <QApplication>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QStringListModel>
#include <QPainter>

#include "base.h"

class ColorModel  : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ColorModel(const QVector<Qt::GlobalColor>& color, QObject *parent = nullptr)
        : QAbstractListModel(parent),
          m_colors(color) {}
    virtual ~ColorModel() {}

public slots:
    int rowCount(const QModelIndex &parent = QModelIndex()) const { return m_colors.size(); }
    QVariant data(const QModelIndex &index, int role = Qt::BackgroundRole) const;
    //virtual Qt::ItemFlags flags(const QModelIndex &index) const  override;

protected:
    QVector<Qt::GlobalColor> m_colors;
};


//------------------------ColorDelegate------------------------//
class ColorDelegate :  public QStyledItemDelegate
{
    using ColorsList = QVector<Qt::GlobalColor>;
    Q_OBJECT
public:
    ColorDelegate(const ColorsList& list = defaultColors, QObject *parent = 0)
        : QStyledItemDelegate(parent) { items = list; }

public slots:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setItems(const ColorsList &list) {  items = list; }

protected:
    ColorsList items;
};

//------------------------LsDelegate------------------------//
class ListDelegate :  public QStyledItemDelegate
{
    Q_OBJECT
public:
    ListDelegate(const QStringList& list, QObject *parent = 0)
        : QStyledItemDelegate(parent) { items = list; }
    //
    ListDelegate(const QList<QString> & list, QObject *parent = 0)
        : QStyledItemDelegate(parent) { items = list; }
public slots:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setItems(const QStringList& list) {  items = list; }

protected:
    QStringList items;
};


#endif // ColorDelegate_H
