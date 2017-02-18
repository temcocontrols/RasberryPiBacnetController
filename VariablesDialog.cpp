#include "VariablesDialog.h"
#include "ui_VariablesDialog.h"

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
}

VariablesDialog::~VariablesDialog()
{
    delete ui;
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
