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
#include "MainWindowInterface.h"
#include "coordinator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace Serialize {
class ChatMessage;
class StringVector;
}

class NetworkCoordinatorInterface;

class MainWindow : public QMainWindow, public MainWindowInterface
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setCoordinator(NetworkCoordinatorInterface* coord) { pCoordinator = coord; }
public:
    void showMessage(const QString& message) override;
    void fillDBNamesList(const std::vector<std::string>& vNames) override;
    void fillCollectionsList(const std::vector<std::string>& vCollNames) override;
    void fillUsersList(const std::vector<std::string>& vCollNames) override;
    void selectFirstUser() override;
    void fillUserInfo(const Database::userInfo& info) override;
    void setNickNameEditable(bool editable) override;
    void selectUserFromList(int number) override;
public slots:
     void getCollectionsClicked(bool checked = false);
     void getUsersClicked(bool checked = false);
     void userAddClicked(bool checked = false);
     void saveUserInfoButtonClicked(bool checked = false);
     void getDatabasesButtonClicked(bool checked = false);
     void userDeleteClicked(bool checked = false);
     void adminAuthorizeClicked(bool checked = false);
//     void disconnected();
//     void listClicked(QListWidgetItem *item);
     void listPressed(QListWidgetItem *item);
 private Q_SLOTS:
//     void onConnected();
//     void onTextMessageReceived(const QString& message);
//     void onBinaryMessageReceived(const QByteArray &message);

     void on_actionLogIn_triggered();
//     void stateChanged(int state);
private:
    void clearUserDataFields();
    void fillUsersList(std::set<std::string>& users);

//    std::optional<Database::userInfo> findSelectedUser(const std::string& nick);
    Database::userInfo createUserInfo();

    std::vector<std::string> getElements(const Serialize::StringVector& elements);
    bool isUserDataValid();
    Ui::MainWindow *ui;

    NetworkCoordinatorInterface* pCoordinator{};
};
#endif // MAINWINDOW_H
