#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "authenticatedialog.h"
#include "QMessageBox"
#include <set>
#include <algorithm>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <QFile>
#include "./ServerCommunication/parametersserialization.h"
#include "./ServerCommunication/authutils.h"
#include "../../Server/build/messages.pb.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->getCollectionsButton, SIGNAL(clicked(bool)), this, SLOT(getCollectionsClicked(bool)));
    connect(ui->getUsersButton, SIGNAL(clicked(bool)), this, SLOT(getUsersClicked(bool)));
    connect(ui->addUserButton, SIGNAL(clicked(bool)), this, SLOT(userAddClicked(bool)));
    connect(ui->deleteUserButton, SIGNAL(clicked(bool)), this, SLOT(userDeleteClicked(bool)));
    connect(ui->saveUserInfoButton, SIGNAL(clicked(bool)), this, SLOT(saveUserInfoButtonClicked(bool)));
    connect(ui->getDatabasesButton, SIGNAL(clicked(bool)), this, SLOT(getDatabasesButtonClicked(bool)));
    connect(ui->authorizeButton, SIGNAL(clicked(bool)), this, SLOT(adminAuthorizeClicked(bool)));
    connect(ui->usersList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listPressed(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::listPressed(QListWidgetItem *item)
{
    if(!item || !pCoordinator)
        return;

    auto userSelected = pCoordinator->findSelectedUser(item->text().toStdString());
    if(!userSelected)
        return;

    fillUserInfo(*userSelected);
    setNickNameEditable(false);
}

void MainWindow::getCollectionsClicked(bool checked)
{
    const auto& text = ui->dbNameCombo->currentText();
    if(text.isEmpty())
    {
        QString message = "Select data base first!";
        showMessage(message);
        return;
    }
     if(!pCoordinator)
         return;
    pCoordinator->sendGetDBCollectionNamesRequest(text.toStdString());
}

void MainWindow::getUsersClicked(bool checked)
{
    const auto& dbName = ui->dbNameCombo->currentText();
    const auto& collection = ui->usersCollectionCombo->currentText();
    if(dbName.isEmpty() || collection.isEmpty())
    {
        QString message = "Select db name or collection first!";
        showMessage(message);
        return;
    }
    pCoordinator->sendGetAllUsersMessage(dbName.toStdString(), collection.toStdString());
}

void MainWindow::userAddClicked(bool checked)
{
    ui->userNickNameEdit->setEnabled(true);
    clearUserDataFields();
}

void MainWindow::userDeleteClicked(bool checked)
{
    const auto& dbName = ui->dbNameCombo->currentText();
    const auto& collection = ui->usersCollectionCombo->currentText();
/*    if(!userSelected)
    {
          QString message = "Select user!";
          showMessage(message);
          return;
    }
*/
    pCoordinator->sendDeleteSelectedUserMessage(dbName.toStdString(), collection.toStdString());
}

bool hasSecureString(const QString& strTest)
{
    if(strTest.length() < 8)
        return false;

    return true;
}

bool MainWindow::isUserDataValid()
{
    if(ui->userNameEdit->text().length() < 3)
    {
        showMessage("name too short!");
        return false;
    }

    if(ui->userSurnameEdit->text().length() < 4)
    {
        showMessage("surname too short!");
        return false;
    }

    if(ui->userEmailEdit->text().length() < 8)
    {
        showMessage("email field too short!");
        return false;
    }

    if(ui->userNickNameEdit->text().length() < 5)
    {
        showMessage("nickname too short!");
        return false;
    }

    if(ui->userPasswordEdit->text().length() < 5)
    {
        showMessage("password field too short!");
        return false;
    }
    return true;
}

Database::userInfo MainWindow::createUserInfo()
{
    Database::userInfo info;
    info.name = ui->userNameEdit->text().toStdString();
    info.surname = ui->userSurnameEdit->text().toStdString();
    info.email = ui->userEmailEdit->text().toStdString();
    info.nickname = ui->userNickNameEdit->text().toStdString();
    info.password = ui->userPasswordEdit->text().toStdString();
    info.deleted = (ui->deleteUserBox->checkState() == Qt::Checked) ? true : false;
    return info;
}

std::map<std::string, std::string> getUpdatedFields(const Database::userInfo& srcInfo, const Database::userInfo& editedInfo)
{
    std::map<std::string, std::string> res;

    if(srcInfo.name != editedInfo.name)
        res[Database::nameField] = editedInfo.name;

    if(srcInfo.surname != editedInfo.surname)
        res[Database::surnameField] = editedInfo.surname;

    if(srcInfo.email != editedInfo.email)
        res[Database::emailField] = editedInfo.email;

    if(srcInfo.nickname != editedInfo.nickname)
        res[Database::nicknameField] = editedInfo.nickname;

    if(srcInfo.password != editedInfo.password)
        res[Database::passwordField] = editedInfo.password;

    if(srcInfo.profilepicture != editedInfo.profilepicture)
        res[Database::photoField] = editedInfo.profilepicture;

    if(srcInfo.deleted != editedInfo.deleted)
        res[Database::deletedField] = editedInfo.deleted ? "true" : "false";

    return res;
}

void MainWindow::saveUserInfoButtonClicked(bool checked)
{
    const auto& dbName = ui->dbNameCombo->currentText();
    const auto& collection = ui->usersCollectionCombo->currentText();
    if(dbName.isEmpty() || collection.isEmpty())
    {
        QString message = "Select db name or collection first!";
        showMessage(message);
        return;
    }

   if(!isUserDataValid())
       return;

    auto info = createUserInfo();
    pCoordinator->sendAddUserMessage(dbName.toStdString(), collection.toStdString(), info);
}

void MainWindow::getDatabasesButtonClicked(bool checked)
{    
     pCoordinator->sendGetDBNamesMessage();
}


void MainWindow::fillUsersList(std::set<std::string>& users)
{
    ui->usersList->clear();
    for(const auto& usr: users)
        ui->usersList->addItem(QString::fromStdString(usr));
}

void MainWindow::adminAuthorizeClicked(bool checked)
{
    const auto& dbName = ui->dbNameCombo->currentText();
    pCoordinator->sendAuthorizeMessage(dbName.toStdString());
}

std::vector<std::string> MainWindow::getElements(const Serialize::StringVector& elements)
{
    std::vector<std::string> res;

    int size = elements.value_size();
    for(int i = 0; i < size; ++i)
        res.push_back(elements.value(i));

    return res;
}

void MainWindow::clearUserDataFields()
{
    std::set<QLineEdit*> vFields{
        ui->userNameEdit, ui->userSurnameEdit, ui->userEmailEdit, ui->userNickNameEdit,ui->userPasswordEdit
    };

    for(auto& field : vFields)
        field->clear();
}

void MainWindow::on_actionLogIn_triggered()
{
    AuthenticateDialog dlg(this);
    if(dlg.exec())
    {
        QString login = dlg.getLogin();
        QString password = dlg.getPassword();
        if(!hasSecureString(login) || !hasSecureString(password))
        {
            QString message = "log o passw is too short";
            showMessage(message);
            return;
        }
/*
        const std::string host = "ws://127.0.0.1";
        unsigned port = 8083;

        std::string json = serializeConnectionDataToString(login.toStdString(), password.toStdString(),
                                                           host, port);
        QString path{"../../Server/data/config.json"};

        saveJsonToFile(path, QString::fromStdString(json));
*/
        QString path{"../../Server/data/config.json"};
        auto content = readJsonFromFile(path);
        Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
        if(!data.has_connectionparameters())
            return;
        const auto& parameters = data.connectionparameters();
        QString host = QString::fromStdString(parameters.host()) + ':' + QString::number(parameters.port());

    }
}

void MainWindow::showMessage(const QString& message)
{
   QMessageBox msgBox(this);
   msgBox.setText(message);
   msgBox.exec();
}

void MainWindow::fillDBNamesList(const std::vector<std::string>& vNames)
{
    ui->dbNameCombo->clear();
    for(const auto& element : vNames)
        ui->dbNameCombo->addItem(QString::fromStdString(element));
}

void MainWindow::fillCollectionsList(const std::vector<std::string>& vCollNames)
{
    ui->usersCollectionCombo->clear();
    for(const auto& element : vCollNames)
        ui->usersCollectionCombo->addItem(QString::fromStdString(element));
}

void MainWindow::fillUsersList(const std::vector<std::string>& vCollNames)
{
    ui->usersList->clear();
    for(const auto& nickname : vCollNames)
        ui->usersList->addItem(QString::fromStdString(nickname));
}

void MainWindow::selectFirstUser()
{
    if(!ui->usersList->count())
        return;
    ui->usersList->setCurrentRow(0);
}

void MainWindow::fillUserInfo(const Database::userInfo& info)
{
    clearUserDataFields();
    ui->userNameEdit->setText(QString::fromStdString(info.name));
    ui->userSurnameEdit->setText(QString::fromStdString(info.surname));
    ui->userEmailEdit->setText(QString::fromStdString(info.email));
    ui->userNickNameEdit->setText(QString::fromStdString(info.nickname));
    ui->userPasswordEdit->setText(QString::fromStdString(info.password));
    ui->deleteUserBox->setCheckState(info.deleted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void MainWindow::setNickNameEditable(bool editable)
{
    ui->userNickNameEdit->setEnabled(editable);
}

void MainWindow::selectUserFromList(int number)
{
    ui->usersList->setCurrentRow(number);
}

//grep serializeGetUsersMessage ./src/*
