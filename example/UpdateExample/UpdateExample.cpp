#include "UpdateExample.h"
#include "ui_UpdateExample.h"

UpdateExample::UpdateExample(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateExample)
{
    ui->setupUi(this);
}

UpdateExample::~UpdateExample()
{
    delete ui;
}
