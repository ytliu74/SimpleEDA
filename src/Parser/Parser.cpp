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
    Vsrc_name_vec.clear();
    Res_name_vec.clear();
    Cap_name_vec.clear();
    Ind_name_vec.clear();

    printVariable_vec.clear();

    command_OP = false;
    command_END = false;
    analysisType = NONE;
}

Parser::~Parser() {
    Vsrc_vec.clear();
    Res_vec.clear();
    Cap_vec.clear();
    Ind_vec.clear();

    printVariable_vec.clear();
}

void Parser::deviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    std::string deviceName = elements[0].toStdString().data();

    // Process Voltage Source
    if (line.startsWith("V") | line.startsWith("v")) {
        if (num_elements != 4) {
            parseError("Failed to parse " + deviceName, lineNum);
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Vsrc vsrc = { deviceName, value, node_1, node_2 };
            Vsrc_vec.push_back(vsrc);
            Vsrc_name_vec.push_back(deviceName);

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
            parseError("Failed to parse " + deviceName, lineNum);
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Res res = { deviceName, value, node_1, node_2 };
            Res_vec.push_back(res);
            Res_name_vec.push_back(deviceName);

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
            parseError("Failed to parse " + deviceName, lineNum);
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Cap cap = { deviceName, value, node_1, node_2 };
            Cap_vec.push_back(cap);
            Cap_name_vec.push_back(deviceName);

            std::cout << "Parsed Device Type: Capacitor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
    // Process Inductor
    else if (line.startsWith("L") | line.startsWith("l")) {
        if (num_elements != 4) {
            parseError("Failed to parse " + deviceName, lineNum);
        }
        else {
            double value = parseValue(elements[3]);
            std::string node_1 = elements[1].toStdString().data();
            std::string node_2 = elements[2].toStdString().data();
            Ind ind = { deviceName, value, node_1, node_2 };
            Ind_vec.push_back(ind);
            Ind_name_vec.push_back(deviceName);

            std::cout << "Parsed Device Type: Inductor ("
                << "Name: " << deviceName << "; "
                << "Value: " << value << "; "
                << "Node1: " << node_1 << "; "
                << "Node2: " << node_2 << " )"
                << std::endl;
        }
    }
}

/**
 * @brief Parser for command
 *
 * @param line a const QString
 * @param lineNum
 */
void Parser::commandParser(const QString line, const int lineNum) {

    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    QString command = elements[0];

    // .OP
    if (command == ".op") {
        if (num_elements != 1)
            parseError("Failed to parse .op", lineNum);
        else {
            command_OP = true;
            analysisType = DC;
            std::cout << "Parsed Analysis Command .OP Token" << std::endl;
        }
    }
    // .END
    else if (command == ".end") {
        if (num_elements != 1)
            parseError("Failed to parse .end", lineNum);
        else {
            command_END = true;
            std::cout << "Parsed .END Token" << std::endl;
        }
    }
    // .PRINT
    else if (command == ".print") {
        if (num_elements == 1)
            parseError("Failed to parse .print, .print need parameters", lineNum);
        else if (elements[1].startsWith("i") | elements[1].startsWith("v")) {// .print V(node)
            if (analysisType == NONE)
                parseError("Failed to parse .print. Analysis type is not determined", lineNum);
            else {
                elements.removeFirst(); // remove the first one
                printParser(elements);
            }
        }
        // .print dc V(..)
        else {
            if (elements[1] == "dc")
                analysisType = DC;
            else if (elements[1] == "ac")
                analysisType = AC;
            else if (elements[1] == "tran")
                analysisType = TRAN;
            else parseError("Failed to parse .print. Invalid analysis type", lineNum);

            elements.removeFirst();
            elements.removeFirst(); // remove the first two
            printParser(elements);
        }
        //TODO: .DC
    }
}

// TODO: This method is far from complete.
void Parser::printParser(const QStringList elements) {
    print_T printType;
    analysisVariable_T analysisVariableType;
    std::string node_1;
    std::string node_2;

    QRegularExpression bracket_re("(?<=\\().*(?=\\))");

    for (QString e : elements) {
        if (e.startsWith("v")) {
            printType = V;

            QRegularExpressionMatch match = bracket_re.match(e);
            if (match.hasMatch())
                node_1 = match.captured().toStdString();

            analysisVariableType = MAG;
            node_2 = "0";
            printVariable printVar{ printType, analysisVariableType, node_1, node_2 };
            std::cout << "Parsed Output Coomand Print (Type: " << analysis_T_lookup[analysisType]
                << "; " << "Type: " << "Voltage " << node_1 << ")" << std::endl;
        }
    }
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

    scaledUnit f = { "f", 1e-15 };
    scaledUnit p = { "p", 1e-12 };
    scaledUnit n = { "n", 1e-9 };
    scaledUnit u = { "u", 1e-6 };
    scaledUnit m = { "m", 1e-3 };
    scaledUnit k = { "k", 1e3 };
    scaledUnit meg = { "meg", 1e6 };
    scaledUnit g = { "g", 1e9 };
    scaledUnit t = { "t", 1e12 };

    std::vector<scaledUnit> scaledUnit_vec = { f, p, n, u, m, k, meg, g, t };

    for (auto scaledUnit : scaledUnit_vec) {
        if (value_in_str.endsWith(scaledUnit.unit)) {
            value *= scaledUnit.scaledValue;
            break;
        }
    }
    return value;
}

/**
 * @brief Print an error message.
 *
 * @param error_msg
 * @param lineNum
 */
void Parser::parseError(const std::string error_msg, const int lineNum) {
    std::cout << "Error: line " << lineNum << ": " << error_msg << std::endl;
}
