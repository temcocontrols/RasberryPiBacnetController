#include "VariablesModelDelegate.h"

#include "VariablesModel.h"

VariablesModelDelegate::VariablesModelDelegate(VariablesModel* model, QObject* parent)
    : QStyledItemDelegate(parent)
{
    this->model = model;
}

QWidget* VariablesModelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return this->model->createEditor(parent, option, index);
}

void VariablesModelDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return this->model->setEditorData(index, editor);
}

void VariablesModelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    return this->model->setDataFromEditor(editor, index);
}

void VariablesModelDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
