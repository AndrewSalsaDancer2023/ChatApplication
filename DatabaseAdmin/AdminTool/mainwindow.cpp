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
#include "parametersserialization.h"
#include "messageserialization.h"
#include "../../Server/src/common_types.h"
#include "../../Server/build/messages.pb.h"
//#include "../../Server/src/database/DatabaseTypes.h"

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
//    connect(ui->usersList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listClicked(QListWidgetItem*)));
    connect(ui->usersList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listPressed(QListWidgetItem*)));
//    connect(ui->deleteUserBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &MainWindow::disconnected);

 //   m_webSocket.open(QUrl(QStringLiteral("ws://127.0.0.1:8083")));
    QString host  = getHost();
    m_webSocket.open(QUrl(host));

  //  ui->deleteUserBox->setCheckState(Qt::Checked);
  //ui->infoLayout->setEnabled(false); //userInfoLayout
/*    for(auto&& child: ui->infoLayout->findChildren<QWidget *>()){
        child->setVisible(false);
    }*/
}

MainWindow::~MainWindow()
{
    delete ui;
}


std::optional<Database::userInfo> MainWindow::findSelectedUser(const std::string& nick)
{
    auto it = std::find_if(users.begin(), users.end(), [&nick](const auto& user){
       return user.nickname == nick;
    });

    if(it != users.end())
        return *it;

    return std::nullopt;
}

void MainWindow::listPressed(QListWidgetItem *item)
{
    if(!item)
        return;
    userSelected = findSelectedUser(item->text().toStdString());
    if(!userSelected)
        return;

    fillUserInfo(*userSelected);
    ui->userNickNameEdit->setEnabled(false);
}

void MainWindow::showMessage(const QString& message)
{
   QMessageBox msgBox(this);
   msgBox.setText(message);
   msgBox.exec();
}

void MainWindow::showMessage(QString&& message)
{
   QMessageBox msgBox(this);
   msgBox.setText(std::move(message));
   msgBox.exec();
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

     std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES, text.toStdString());
     if(res.empty())
     {
         QString message = "Unable to encode message SERVER_GET_DB_NAMES";
         showMessage(message);
         return;
     }

     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
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

    std::string res = createGetAllUsersMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE, dbName.toStdString(), collection.toStdString());
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
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
    if(!userSelected)
    {
          QString message = "Select user!";
          showMessage(message);
          return;
    }
    std::string res = createDeleteUserMessage(dbName.toStdString(), collection.toStdString(), (*userSelected).nickname);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
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

    if(ui->userSurnameEdit->text().length() < 5)
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

/*
Database::userInfo clearEqualFields(const Database::userInfo& srcInfo, const Database::userInfo& editedInfo)
{
    Database::userInfo res = editedInfo;

    if(srcInfo.name == editedInfo.name)
        res.name.clear();

    if(srcInfo.surname == editedInfo.surname)
        res.surname.clear();

    if(srcInfo.email == editedInfo.email)
        res.email.clear();

    if(srcInfo.nickname == editedInfo.nickname)
        res.nickname.clear();

    if(srcInfo.password == editedInfo.password)
        res.password.clear();

    if(srcInfo.profilepicture == editedInfo.profilepicture)
        res.profilepicture.clear();

    return res;
}
*/

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

    auto itFind = std::find_if(users.begin(), users.end(),[&info](const auto& user){
        return user.nickname == info.nickname;
    });

    userSelected = info;
    std::string res;
    if(itFind == users.end())
        res = createAddUserMessage(dbName.toStdString(), collection.toStdString(), info);
    else
        res = createUpdateUserMessage(dbName.toStdString(), collection.toStdString(), info);

    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void MainWindow::getDatabasesButtonClicked(bool checked)
{
    std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_NAMES);
     if(res.empty())
     {
         QString message = "Unable to encode message SERVER_GET_DB_NAMES";
         showMessage(message);
         return;
     }
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void MainWindow::fillUserInfo(const Database::userInfo& info)
{
    ui->userNameEdit->setText(QString::fromStdString(info.name));
    ui->userSurnameEdit->setText(QString::fromStdString(info.surname));
    ui->userEmailEdit->setText(QString::fromStdString(info.email));
    ui->userNickNameEdit->setText(QString::fromStdString(info.nickname));
    ui->userPasswordEdit->setText(QString::fromStdString(info.password));
    ui->deleteUserBox->setCheckState(info.deleted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void MainWindow::fillUsersList(std::set<std::string>& users)
{
    for(const auto& usr: users)
        ui->usersList->addItem(QString::fromStdString(usr));
}

void MainWindow::onConnected()
 {
    state = commState::Connected;
     connect(&m_webSocket, &QWebSocket::textMessageReceived,
             this, &MainWindow::onTextMessageReceived);

     connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
             this, &MainWindow::onBinaryMessageReceived);
 }

void MainWindow::handleAuthenticationMessage(const Serialize::ChatMessage& msg)
{
    if((state == commState::Connected) &&
       (static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED) == msg.payload_type_id()))
       state = commState::Authenticated;
    else
    {
        QString message = "Authentication error!";
        showMessage(message);
    }
}

void MainWindow::adminAuthorizeClicked(bool checked)
{
    const auto&[login, password] = getLoginPassword();
    std::string res = createAuthorizationMessage(login, password);
    if(res.empty())
        return;

     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

std::vector<std::string> MainWindow::getElements(const Serialize::StringVector& elements)
{
    std::vector<std::string> res;

    int size = elements.value_size();
    for(int i = 0; i < size; ++i)
        res.push_back(elements.value(i));

    return res;
}

void MainWindow::handleGetDatabaseNames(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_NAMES) == msg.payload_type_id())
    {
        if(!msg.has_payload())
              return;

      Serialize::StringVector elements;
      msg.mutable_payload()->UnpackTo(&elements);

       ui->dbNameCombo->clear();
       for(const auto& element : getElements(elements))
           ui->dbNameCombo->addItem(QString::fromStdString(element));
    }
}

void MainWindow::handleGetCollectionsNames(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES) == msg.payload_type_id())
    {
        if(!msg.has_payload())
            return;

        Serialize::StringVector elements;
        msg.mutable_payload()->UnpackTo(&elements);

        ui->usersCollectionCombo->clear();
        for(const auto& element : getElements(elements))
            ui->usersCollectionCombo->addItem(QString::fromStdString(element));
    }
}

void MainWindow::handleGetUsersInfo(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE) == msg.payload_type_id())
    {
        if(!msg.has_payload())
            return;

        Serialize::UserInfoVector elements;
        msg.mutable_payload()->UnpackTo(&elements);

        for(int i = 0; i < elements.users_size(); ++i)
        {
            const Serialize::UserInfo& usrInfo = elements.users(i);

            Database::userInfo user;
            user.name = std::move(usrInfo.name());
            user.surname = std::move(usrInfo.surname());
            user.email = std::move(usrInfo.email());
            user.nickname = std::move(usrInfo.nickname());
            user.password = std::move(usrInfo.password());
            user.profilepicture = std::move(usrInfo.profilepicture());
            user.deleted = usrInfo.deleted();

            users.push_back(user);
        }

//        fillUsersList(uss);
        ui->usersList->clear();
        for(const auto& user : users)
            ui->usersList->addItem(QString::fromStdString(user.nickname));

        if(users.size())
        {
            clearUserDataFields();
            userSelected = users[0];
            fillUserInfo(*userSelected);
            ui->userNickNameEdit->setEnabled(false);
            ui->usersList->setCurrentRow(0);
        }
    }
}

void MainWindow::handleDeleteUserError(Serialize::ChatMessage& msg)
{
   const std::string& nickName = msg.sender();
   std::string message = "Unable delete user:" + nickName;
    QString outmsg = QString::fromStdString(message);
    showMessage(outmsg);
}

void MainWindow::handleDeleteUser(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
        });
    if(itFind == users.end())
        return;
    itFind->deleted = true;
}

void MainWindow::handleModifyUserInfoError(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    std::string message = "Unable modify user:" + nickName;
    QString outmsg = QString::fromStdString(message);
    showMessage(outmsg);
}

void MainWindow::handleModifyUserInfo(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
    });

    if((itFind == users.end()) || !userSelected)
        return;

    if((*userSelected).nickname != nickName)
        return;

    *itFind = *userSelected;
}

void MainWindow::handleAddUserInfo(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
    });

    if((itFind != users.end()) || !userSelected)
        return;

    if((*userSelected).nickname != nickName)
        return;

    users.push_back(*userSelected);
}

void MainWindow::handleAddUserInfoError(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    std::string message = "Unable add user:" + nickName;
    QString outmsg = QString::fromStdString(message);
    showMessage(outmsg);
}

void MainWindow::onBinaryMessageReceived(const QByteArray &message)
{
     QString msg{message};
     auto curMsg = decodeMessageFromString(message.toStdString());

     if(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED) == curMsg.payload_type_id())
         handleAuthenticationMessage(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_NAMES) == curMsg.payload_type_id())
         handleGetDatabaseNames(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES) == curMsg.payload_type_id())
        handleGetCollectionsNames(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE) == curMsg.payload_type_id())
        handleGetUsersInfo(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_DELETION_SUCCESS) == curMsg.payload_type_id())
        handleDeleteUser(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_DELETION_ERROR) == curMsg.payload_type_id())
        handleDeleteUserError(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO_SUCCESS) == curMsg.payload_type_id())
        handleModifyUserInfo(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO_ERROR) == curMsg.payload_type_id())
        handleModifyUserInfoError(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_DATABASE_SUCCESS) == curMsg.payload_type_id())
        handleAddUserInfo(curMsg);

     if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_ADD_TO_DATABASE_ERROR) == curMsg.payload_type_id())
        handleAddUserInfoError(curMsg);
}


 void MainWindow::onTextMessageReceived(const QString& message)
 {
  int k = 10;
  //     m_webSocket.close();
 }

void MainWindow::disconnected()
 {
    QString message = "Socket disconnected!";
    showMessage(message);
 }

void MainWindow::clearUserDataFields()
{
    std::set<QLineEdit*> vFields{
        ui->userNameEdit, ui->userSurnameEdit, ui->userEmailEdit, ui->userNickNameEdit,ui->userPasswordEdit
    };

    for(auto& field : vFields)
        field->clear();
}

void saveAuthInfo(const QString& login, const QString& password, const QString& path)
{

}

std::string getStringFromFile( const std::string & filename, bool binary = false)
{
  std::ifstream f( filename, std::ios::in);
  if (!f) throw std::runtime_error( "file_to_string: " + filename );

  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

void saveStringToFile(const std::string & filename, const std::string& text)
{
   //std::ofstream myfile(filename, std::ios::out);
   std::ofstream myfile(filename, std::ios_base::binary);
 //if(!myfile.is_open())
 //    return;
//   myfile.open(filename);

   myfile << text;
   myfile.close();
 }

void saveJsonToFile(const QString& path, const QString& fileContent)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << fileContent;

    file.close();
}

QString readJsonFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&file);
    QString content = in.readLine();
    QString current = content;
    while (!current.isNull()) {
            current = in.readLine();
            content += current;
        }

    return content;
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

QString MainWindow::getHost()
{
    QString path{"../../Server/data/config.json"};
    auto content = readJsonFromFile(path);
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_connectionparameters())
        return {};
    const auto& parameters = data.connectionparameters();
    return QString::fromStdString(parameters.host()) + ':' + QString::number(parameters.port());
}

std::pair<std::string, std::string> MainWindow::getLoginPassword()
{
    QString path{"../../Server/data/config.json"};
    auto content = readJsonFromFile(path);
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_authentication())
        return {};
    const auto& auth = data.authentication();
    return { auth.login(),  auth.password()};
}

//grep serializeGetUsersMessage ./src/*

