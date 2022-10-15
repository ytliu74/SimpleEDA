#include "utils.h"

std::ostream& operator<<(std::ostream& os, const QString& qstr) {
    os << qstr.toStdString();
    return os;
}

const std::string str(const QString qstr) { return qstr.toStdString(); }

/**
 * @brief Find the first matched element in a vector
 *
 * @tparam T
 * @param vec
 * @param element
 * @return int: if is -1, means not exits.
 */
template <typename T>
int findFirstElement(const std::vector<T> vec, T element) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == element)
            return i;
    }
    return -1;
}