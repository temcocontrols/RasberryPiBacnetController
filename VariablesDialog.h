#ifndef VARIABLESDIALOG_H
#define VARIABLESDIALOG_H

#include <QDialog>

#include "VariablesModel.h"

namespace Ui {
class VariablesDialog;
}

class VariablesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VariablesDialog(QWidget *parent = 0);
    ~VariablesDialog();

private:
    Ui::VariablesDialog *ui;
    VariablesModel* model;

private slots:
    void addRow();
    void deleteRow();
};

#endif // VARIABLESDIALOG_H
