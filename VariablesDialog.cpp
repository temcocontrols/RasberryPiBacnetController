#include "VariablesDialog.h"
#include "ui_VariablesDialog.h"
#include <QFileDialog>
#include "PRGReader.h"

VariablesDialog::VariablesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VariablesDialog)
{
    ui->setupUi(this);

    this->model = new VariablesModel(this);
    ui->tableView->setModel(this->model);
    this->model->setupDelegates(ui->tableView);

    ui->tableView->setColumnWidth(0, 30);
    ui->tableView->setColumnWidth(1, 180);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->setColumnWidth(3, 100);
    ui->tableView->setColumnWidth(4, 100);
    ui->tableView->setColumnWidth(5, 100);

    connect(ui->addRowButton, SIGNAL(clicked(bool)), this, SLOT(addRow()));
    connect(ui->deleteRowButton, SIGNAL(clicked(bool)), this, SLOT(deleteRow()));
    connect(ui->saveFileButton, SIGNAL(clicked(bool)), this, SLOT(saveFileAs()));
}

VariablesDialog::~VariablesDialog()
{
    delete ui;
}

void VariablesDialog::addRowWithData(char *full_label, bool is_auto, int16_t digital_analog, int16_t range, unsigned int data, char *label)
{
    auto index = this->model->rowCount();
    this->model->insertRow(index);
    //this->ui->tableView->edit(this->model->index(index, 1));

    QModelIndex model_index = this->model->index (index, FULL_LABEL);
    QString qfull_label = full_label;
    this->model->setData(model_index, QVariant (qfull_label));

    bool isauto = is_auto;
    model_index = this->model->index (index, AUTO);
    this->model->setData(model_index, QVariant (isauto));

    QString qlabel = label;
    model_index = this->model->index (index, LABEL);
    this->model->setData(model_index, QVariant (qlabel));

    QVariant qvalue;
    VariableValue value;
    model_index = this->model->index (index, UNITS);


    if (digital_analog)
    {
        value.value.type = ANALOG;
        value.analogValue.unit = range;
        value.analogValue.value = data / 1000;
    }
    else
    {
        value.value.type = DIGITAL;
        value.digitalValue.unit = range - 101;
        value.digitalValue.inverted = false;
        value.digitalValue.value = data;
    }
    qvalue.setValue (value);
    this->model->setData(model_index, qvalue);

}

void VariablesDialog::addEmptyRow()
{
    addRow();
}

void VariablesDialog::addRow()
{
    auto index = this->model->rowCount();
    this->model->insertRow(index);
    this->ui->tableView->edit(this->model->index(index, 1));
}

#include <QDebug>
void VariablesDialog::deleteRow()
{
    auto index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        this->model->removeRow(index.row(), QModelIndex());
    }
}

void VariablesDialog::saveFileAs()
{
    QString path;
    QString filter = "T3000 PRG File (*.prg)";
    path = QFileDialog::getSaveFileName(this, "Select a file...", QDir::homePath(), filter);
    char *str;

    Panel_info1 panel_info;
    panel_info.panel_number = 1;

    

    for (int i = 0; i < this->model->rowCount(); i ++)
    {
        Str_variable_point *current_var = &m_reader->vars[i];

        QModelIndex model_index = this->model->index (i, FULL_LABEL);

        Variable variable = this->model->getRowVariable(i);

        QString value = variable.fullLabel;
        QByteArray ba = value.toLatin1();
        str = (char *) ba.data();
        strncpy (current_var->description, str, 21);

        value = variable.label;
        ba = value.toLatin1();
        str = (char *) ba.data();
        strncpy (current_var->label, str, 9);

        current_var->auto_manual = variable.auto_ ? 0 : 1;

        current_var->digital_analog = variable.value.value.type;

        if (variable.value.value.type == ANALOG)
        {
            current_var->range = variable.value.analogValue.unit;

            int analog_value = (int) (variable.value.analogValue.value * 1000.0);

            current_var->value = analog_value;
        }
        else
        {
            current_var->range = variable.value.analogValue.unit + 101;
        }


    }

	QByteArray ba = path.toLatin1();
    str = (char *)ba.data();

	m_reader->savefile(str, &panel_info);
}


