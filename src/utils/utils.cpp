#include "utils.h"

std::ostream& operator<<(std::ostream& os, const QString& qstr) {
    os << qstr.toStdString();
    return os;
}

const std::string str(const QString qstr) { return qstr.toStdString(); }

QString qstr(const std::string str) { return QString::fromStdString(str); }