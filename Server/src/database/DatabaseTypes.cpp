
#include "DatabaseTypes.h"
namespace Database {
   const std::string nameField{"name"};
   const std::string surnameField{"surame"};
   const std::string emailField{"email"};
   const std::string nicknameField{"nickname"};
   const std::string passwordField{"password"};
   const std::string photoField{"photo"};
   const std::string deletedField{"deleted"};

    std::set<std::string> getuserInfoFieldTitles()
    {

        std::set<std::string> res{ nameField, surnameField, emailField, nicknameField,
                                    passwordField, photoField, deletedField};
        return res;
    }
}
