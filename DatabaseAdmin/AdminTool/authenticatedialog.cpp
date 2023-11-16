#include "authenticatedialog.h"
#include "ui_authenticatedialog.h"

AuthenticateDialog::AuthenticateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthenticateDialog)
{
    ui->setupUi(this);
}

AuthenticateDialog::~AuthenticateDialog()
{
    delete ui;
}


QString AuthenticateDialog::getLogin()
{
    return ui->loginField->text();
}

QString AuthenticateDialog::getPassword()
{
    return ui->passwordField->text();
}
