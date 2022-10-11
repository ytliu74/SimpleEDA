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

/**
 * @brief To parse the value correctly
 *
 * @param value_in_str
 * @return double
 */
double parseValue(const QString value_in_str) {
    double value;

    QRegExp numberWithE("\\d+(\\.\\d+)?e-?\\d+");
    if (numberWithE.exactMatch(value_in_str)) {
        value = value_in_str.toDouble();
        return value;
    }

    QRegExp number("\\d+(\\.\\d+)?");
    int pos = number.indexIn(value_in_str);
    if (pos > -1)
        value = number.cap().toDouble();

    if (value_in_str.endsWith("db")) {
        value = 20 * log10(value);
        return value;
    }

    scaledUnit f = {"f", 1e-15};
    scaledUnit p = {"p", 1e-12};
    scaledUnit n = {"n", 1e-9};
    scaledUnit u = {"u", 1e-6};
    scaledUnit m = {"m", 1e-3};
    scaledUnit k = {"k", 1e3};
    scaledUnit meg = {"meg", 1e6};
    scaledUnit g = {"g", 1e9};
    scaledUnit t = {"t", 1e12};

    std::vector<scaledUnit> scaledUnit_vec ={ f, p, n, u, m, k, meg, g, t };

    for (auto scaledUnit : scaledUnit_vec) {
        if (value_in_str.endsWith(scaledUnit.unit)) {
            value *= scaledUnit.scaledValue;
            break;
        }
    }



    return value;
}