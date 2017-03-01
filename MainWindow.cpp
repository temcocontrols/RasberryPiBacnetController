#include <QFileDialog>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PRGReader.h"

#include "VariablesDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fileOpen()
{
    QString path;
    QString filter = "T3000 PRG File (*.prg)";
    path = QFileDialog::getOpenFileName(this, "Select a file...", QDir::homePath(), filter);
	
    QByteArray ba = path.toLatin1();
    const char *str = ba.data();

    Panel_info1 panel_info;
    PRGReader *reader = new PRGReader();

    reader->loadprg((char *) str, &panel_info);

    VariablesDialog *d = new VariablesDialog();
    d->show();

    d->m_reader = reader;

    for (int i = 0; i < MAX_VARS; i++)
    {
        Str_variable_point *current_var = &reader->vars[i];

        if (!current_var->description[0])
        {
            d->addEmptyRow();
        }
        else
        {
            printf("---------------------------\n");
            printf("Description: %s\n", current_var->description);
            printf("Label: %s\n", current_var->label);
            printf("Value: %d\n", current_var->value);
            printf("AUTO/MANUAL: %s\n", current_var->auto_manual ? "MANUAL" : "AUTO");

            d->addRowWithData((char *) current_var->description,
                            current_var->auto_manual,
                            current_var->digital_analog,
                            current_var->range,
                            current_var->value,
                            current_var->label);
        }

    }
}
