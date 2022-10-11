/**
 * @file Parser.cpp
 * @author Yaotian Liu
 * @brief Parser implementation
 * @date 2022-10-11
 */

#include "Parser.h"
#include "utils/utils.h"

Parser::Parser() {
    Vsrc_vec.clear();
    Res_vec.clear();
    Cap_vec.clear();
    Ind_vec.clear();
}

Parser::~Parser() {
    Vsrc_vec.clear();
    Res_vec.clear();
    Cap_vec.clear();
    Ind_vec.clear();
}

void Parser::deviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    std::string deviceName = elements[0].toStdString().data();

    // Process Voltage Source
    if (line.startsWith("V") | line.startsWith("v")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Vsrc vsrc = { deviceName, value, node_1, node_2 };
            Vsrc_vec.push_back(vsrc);

            std::cout << "Parsed Device Type: Voltage Source ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // Process Resistor
    else if (line.startsWith("R") | line.startsWith("r")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Res res = { deviceName, value, node_1, node_2 };
            Res_vec.push_back(res);

            std::cout << "Parsed Device Type: Resistor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // Process Capacitor
    else if (line.startsWith("C") | line.startsWith("c")) {
        if (num_elements != 4) {
            std::cout << "Error: line " << lineNum
                << ": Fail to parse " << deviceName << std::endl;
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Cap cap = { deviceName, value, node_1, node_2 };
            Cap_vec.push_back(cap);

            std::cout << "Parsed Device Type: Capacitor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // TODO: Inducer
}

void Parser::commandParser(const QString line, const int lineNum) {
    std::cout << "Enter Command Parser" << std::endl;
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    if (num_elements == 1) {
        if (elements[0] == ".op")
            std::cout << "Parsed Analysis Command .OP Token" << std::endl;
        else if (elements[0] == ".end")
            std::cout << "Parsed .END Token" << std::endl;
    } else {}
}

/**
 * @brief To parse the value correctly
 *
 * @param value_in_str
 * @return double
 */
double Parser::parseValue(const QString value_in_str) {
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