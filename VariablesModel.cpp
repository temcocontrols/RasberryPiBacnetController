#include "VariablesModel.h"

#include <QComboBox>
#include <QLineEdit>

enum VariablesModelColumns
{
    NUM = 0,
    FULL_LABEL = 1,
    AUTO = 2,
    VALUE = 3,
    UNITS = 4,
    LABEL = 5
};

VariablesModel::VariablesModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    this->delegate = new VariablesModelDelegate(this, parent);

    qRegisterMetaType<VariableValue>();
    QMetaType::registerComparators<VariableValue>();
}

int VariablesModel::rowCount(const QModelIndex &parent) const
{
    return this->variables.count();
}

int VariablesModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant VariablesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == NUM)
            {
                return tr("Num");
            }
            if (section == FULL_LABEL)
            {
                return tr("Full Label");
            }
            if (section == AUTO)
            {
                return tr("A/M");
            }
            if (section == VALUE)
            {
                return tr("Value");
            }
            if (section == UNITS)
            {
                return tr("Units");
            }
            if (section == LABEL)
            {
                return tr("Label");
            }
        }
    }

    return QVariant();
}

QVariant VariablesModel::data(const QModelIndex &index, int role) const
{
    auto variable = this->variables[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case NUM:
            return index.row() + 1;

        case FULL_LABEL:
            return variable.fullLabel;

        case AUTO:
            return variable.auto_ ? tr("Auto") : tr("Manual");

        case VALUE:
            if (variable.value.value.type == ANALOG)
            {
                return variable.value.analogValue.value;
            }
            if (variable.value.value.type == DIGITAL)
            {
                if (variable.value.digitalValue.inverted)
                {
                    return variable.value.digitalValue.value ?
                                digitalUnits[variable.value.digitalValue.unit].first :
                                digitalUnits[variable.value.digitalValue.unit].second;
                }
                else
                {
                    return variable.value.digitalValue.value ?
                                digitalUnits[variable.value.digitalValue.unit].second :
                                digitalUnits[variable.value.digitalValue.unit].first;
                }
                return variable.value.digitalValue.value;
            }

        case UNITS:
            if (variable.value.value.type == ANALOG)
            {
                return analogUnits[variable.value.analogValue.unit];
            }
            if (variable.value.value.type == DIGITAL)
            {
                if (variable.value.digitalValue.inverted)
                {
                    return digitalUnits[variable.value.digitalValue.unit].second + "/" +
                            digitalUnits[variable.value.digitalValue.unit].first;
                }
                else
                {
                    return digitalUnits[variable.value.digitalValue.unit].first + "/" +
                            digitalUnits[variable.value.digitalValue.unit].second;
                }
            }

        case LABEL:
            return variable.label;
        }
    }

    return QVariant();
}

bool VariablesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() == FULL_LABEL)
        {
            this->variables[index.row()].fullLabel = value.toString();
        }

        if (index.column() == AUTO)
        {
            this->variables[index.row()].auto_ = value.toBool();
        }

        if (index.column() == VALUE)
        {
            if (this->variables[index.row()].value.value.type == ANALOG)
            {
                this->variables[index.row()].value.analogValue.value = value.toFloat();
            }
            if (this->variables[index.row()].value.value.type == DIGITAL)
            {
                this->variables[index.row()].value.digitalValue.value = value.toBool();
            }
        }

        if (index.column() == UNITS)
        {
            VariableValue update = value.value<VariableValue>();
            VariableValue value = this->variables[index.row()].value;
            bool notify = false;
            if (update.value.type != value.value.type)
            {
                value = update;
                notify = true;
            }
            else
            {
                if (value.value.type == ANALOG)
                {
                    value.analogValue.unit = update.analogValue.unit;
                }
                if (value.value.type == DIGITAL)
                {
                    value.digitalValue.unit = update.digitalValue.unit;
                }
            }
            this->variables[index.row()].value = value;
            if (notify)
            {
                emit dataChanged(this->index(index.row(), 0), this->index(index.row(), this->columnCount()));
            }
        }

        if (index.column() == LABEL)
        {
            this->variables[index.row()].label = value.toString();
        }
    }

    return true;
}

Qt::ItemFlags VariablesModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractTableModel::flags(index);

    if (index.column() > 0)
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

void VariablesModel::setupDelegates(QTableView* view)
{
    view->setItemDelegateForColumn(2, this->delegate);
    view->setItemDelegateForColumn(3, this->delegate);
    view->setItemDelegateForColumn(4, this->delegate);
}

QWidget* VariablesModel::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == AUTO)
    {
        QComboBox* editor = new QComboBox(parent);
        editor->addItem(tr("Auto"), true);
        editor->addItem(tr("Manual"), false);
        return editor;
    }

    if (index.column() == VALUE)
    {
        if (this->variables[index.row()].value.value.type == ANALOG)
        {
            QLineEdit* editor = new QLineEdit(parent);
            editor->setValidator(new QDoubleValidator(-1e9, 1e9, 2, editor));
            return editor;
        }

        if (this->variables[index.row()].value.value.type == DIGITAL)
        {
            QComboBox* editor = new QComboBox(parent);
            editor->addItem(digitalUnits[this->variables[index.row()].value.digitalValue.unit].first, false);
            editor->addItem(digitalUnits[this->variables[index.row()].value.digitalValue.unit].second, true);
            return editor;
        }
    }

    if (index.column() == UNITS)
    {
        QComboBox* editor = new QComboBox(parent);

        for (int i = 0; i < analogUnits.count(); i++)
        {
            VariableValue v;
            v.value.type = ANALOG;
            v.analogValue.unit = i;
            v.analogValue.value = 0;
            QVariant variant;
            variant.setValue(v);
            editor->addItem(analogUnits[i], variant);
        }
        for (int i = 0; i < digitalUnits.count(); i++)
        {
            VariableValue v;
            v.value.type = DIGITAL;
            v.digitalValue.unit = i;
            v.digitalValue.inverted = false;
            v.digitalValue.value = false;
            editor->addItem(digitalUnits[i].first + "/" + digitalUnits[i].second, QVariant::fromValue(v));
        }
        for (int i = 0; i < digitalUnits.count(); i++)
        {
            VariableValue v;
            v.value.type = DIGITAL;
            v.digitalValue.unit = i;
            v.digitalValue.inverted = true;
            v.digitalValue.value = false;
            editor->addItem(digitalUnits[i].second + "/" + digitalUnits[i].first, QVariant::fromValue(v));
        }

        return editor;
    }

    return nullptr;
}

void VariablesModel::setEditorData(const QModelIndex &index, QWidget *editor) const
{
    if (index.column() == AUTO)
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findData(this->variables[index.row()].auto_));
    }

    if (index.column() == VALUE)
    {
        if (this->variables[index.row()].value.value.type == ANALOG)
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            lineEdit->setText(QString::number(this->variables[index.row()].value.analogValue.value));
        }

        if (this->variables[index.row()].value.value.type == DIGITAL)
        {
            QComboBox* comboBox = static_cast<QComboBox*>(editor);
            comboBox->setCurrentIndex(comboBox->findData(this->variables[index.row()].value.digitalValue.value));
        }
    }

    if (index.column() == UNITS)
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findData(QVariant::fromValue(this->variables[index.row()].value)));
    }
}

void VariablesModel::setDataFromEditor(QWidget *editor, const QModelIndex &index)
{
    if (index.column() == AUTO)
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        this->setData(index, comboBox->itemData(comboBox->currentIndex()));
    }

    if (index.column() == VALUE)
    {
        if (this->variables[index.row()].value.value.type == ANALOG)
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            this->setData(index, lineEdit->text().toFloat());
        }

        if (this->variables[index.row()].value.value.type == DIGITAL)
        {
            QComboBox* comboBox = static_cast<QComboBox*>(editor);
            this->setData(index, comboBox->itemData(comboBox->currentIndex()));
        }
    }

    if (index.column() == UNITS)
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        this->setData(index, comboBox->itemData(comboBox->currentIndex()));
    }
}

bool VariablesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    this->beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        Variable v;
        this->variables.insert(row + i, v);
    }

    this->endInsertRows();
    return true;
}

#include <QDebug>
bool VariablesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << row;
    this->beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        this->variables.removeAt(row);
    }

    this->endRemoveRows();
    return true;
}
