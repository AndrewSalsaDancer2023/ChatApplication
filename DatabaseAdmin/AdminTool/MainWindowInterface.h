#pragma once
#include <QString>
#include <string>
#include <vector>
#include "../../Server/src/database/DatabaseTypes.h"

class MainWindowInterface
{
public:
    virtual void showMessage(const QString& message) = 0;
    virtual void fillDBNamesList(const std::vector<std::string>& vNames) = 0;
    virtual void fillCollectionsList(const std::vector<std::string>& vCollNames) = 0;
    virtual void fillUsersList(const std::vector<std::string>& vCollNames) = 0;
    virtual void selectFirstUser() = 0;
    virtual void fillUserInfo(const Database::userInfo& info) = 0;
    virtual void setNickNameEditable(bool editable)  = 0;
    virtual void selectUserFromList(int number) = 0;
};
