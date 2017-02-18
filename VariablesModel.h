#ifndef VARIABLESMODEL_H
#define VARIABLESMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QTableView>

#include "Variable.h"
#include "VariablesModelDelegate.h"

class VariablesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    VariablesModel(QObject* parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    void setupDelegates(QTableView* view);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(const QModelIndex &index, QWidget *editor) const;
    void setDataFromEditor(QWidget *editor, const QModelIndex &index);

private:
    VariablesModelDelegate* delegate;
    QList<Variable> variables;

    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
};

#endif // VARIABLESMODEL_H
