#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QtWebSockets/QWebSocket>
#include <QListWidgetItem>
#include <set>
#include <utility>
#include <optional>
#include "../../Server/src/database/DatabaseTypes.h"
/*
namespace Database {
    struct userInfo;
}
*/
enum class commState
{
    Disconnected,
    Connected,
    Authenticated
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace Serialize {
class ChatMessage;
class StringVector;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
     void getCollectionsClicked(bool checked = false);
     void getUsersClicked(bool checked = false);
     void userAddClicked(bool checked = false);
     void saveUserInfoButtonClicked(bool checked = false);
     void getDatabasesButtonClicked(bool checked = false);
     void userDeleteClicked(bool checked = false);
     void adminAuthorizeClicked(bool checked = false);
     void disconnected();
//     void listClicked(QListWidgetItem *item);
     void listPressed(QListWidgetItem *item);
 private Q_SLOTS:
     void onConnected();
     void onTextMessageReceived(const QString& message);
     void onBinaryMessageReceived(const QByteArray &message);

     void on_actionLogIn_triggered();
//     void stateChanged(int state);
private:
    void clearUserDataFields();
    void fillUserInfo(const Database::userInfo& info);
    void fillUsersList(std::set<std::string>& users);
    void showMessage(const QString& message);
    void showMessage(QString&& message);
    std::optional<Database::userInfo> findSelectedUser(const std::string& nick);
    Database::userInfo createUserInfo();

    std::vector<std::string> getElements(const Serialize::StringVector& elements);
    void handleAuthenticationMessage(const Serialize::ChatMessage& msg);
    void handleGetDatabaseNames(Serialize::ChatMessage& msg);
    void handleGetCollectionsNames(Serialize::ChatMessage& msg);
    void handleGetUsersInfo(Serialize::ChatMessage& msg);
    void handleDeleteUser(Serialize::ChatMessage& msg);
    void handleDeleteUserError(Serialize::ChatMessage& msg);
    void handleModifyUserInfo(Serialize::ChatMessage& msg);
    void handleModifyUserInfoError(Serialize::ChatMessage& msg);
    void handleAddUserInfo(Serialize::ChatMessage& msg);
    void handleAddUserInfoError(Serialize::ChatMessage& msg);
    bool isUserDataValid();

    QString getHost();
    std::pair<std::string, std::string> getLoginPassword();
    Ui::MainWindow *ui;
    QWebSocket m_webSocket;
    commState state{commState::Disconnected};
    std::vector<Database::userInfo> users;
    std::optional<Database::userInfo> userSelected;
};
#endif // MAINWINDOW_H
