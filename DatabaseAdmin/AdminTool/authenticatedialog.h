#ifndef AUTHENTICATEDIALOG_H
#define AUTHENTICATEDIALOG_H

#include <QDialog>

namespace Ui {
class AuthenticateDialog;
}

class AuthenticateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenticateDialog(QWidget *parent = nullptr);
    ~AuthenticateDialog();
    QString getLogin();
    QString getPassword();
private:
    Ui::AuthenticateDialog *ui;
};

#endif // AUTHENTICATEDIALOG_H
