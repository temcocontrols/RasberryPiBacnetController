#ifndef VARIABLESMODELDELEGATE_H
#define VARIABLESMODELDELEGATE_H

#include <QStyledItemDelegate>

class VariablesModel;

class VariablesModelDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    VariablesModelDelegate(VariablesModel* model, QObject* parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    VariablesModel* model;
};

#endif // VARIABLESMODELDELEGATE_H
