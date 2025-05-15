#ifndef ACCOUNTTYPE_H
#define ACCOUNTTYPE_H

enum class AccountType {
    Invalid,
    User,
    Owner,
    Employee
};

inline AccountType stringToAccountType(const QString &str) {
    if (str == "Пользователь") return AccountType::User;
    if (str == "Владелец склада") return AccountType::Owner;
    if (str == "Сотрудник") return AccountType::Employee;
    return AccountType::Invalid;
}

inline QStringList getAccountTypeStrings() {
    return {"Выберите тип учетной записи", "Пользователь", "Владелец склада", "Сотрудник"};
}

inline bool checkAccountPassword(const QString &password, const AccountType& type){
    if ((type == AccountType::User && password == "userPass") || (type == AccountType::Owner && password == "ownerPass")
        || (type == AccountType::Employee && password == "emplPass")) return true;
    else return false;
}

#endif // ACCOUNTTYPE_H
