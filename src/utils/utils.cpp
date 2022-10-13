#include "utils.h"

std::ostream& operator<<(std::ostream& os, const QString& qstr) {
    os << qstr.toStdString();
    return os;
}