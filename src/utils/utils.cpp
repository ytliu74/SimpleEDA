#include "utils.h"

/**
 * @brief Easy way to print a QString.
 *
 * @param comment The comment ahead of the QString.
 * @param qstring The target QString.
 */
void printQString(const std::string comment, const QString qstring) {
    std::cout << comment << qstring.toStdString().data() << std::endl;
}
