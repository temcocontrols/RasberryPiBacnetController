#ifndef VARIABLESDIALOG_H
#define VARIABLESDIALOG_H

#include <QDialog>

#include "VariablesModel.h"

namespace Ui {
class VariablesDialog;
}

class PRGReader;

class VariablesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VariablesDialog(QWidget *parent = 0);
    ~VariablesDialog();

    void addRowWithData(char *full_label, bool is_auto, int16_t digital_analog, int16_t range, unsigned int data, char *label);
    void addEmptyRow();

    PRGReader *m_reader;

private:
    Ui::VariablesDialog *ui;
    VariablesModel* model;


private slots:
    void addRow();
    void deleteRow();
    void saveFileAs();
};

#endif // VARIABLESDIALOG_H
