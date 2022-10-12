/**
 * @file Parser.cpp
 * @author Yaotian Liu
 * @brief Parser implementation
 * @date 2022-10-11
 */

#include "Parser.h"

#include "utils/utils.h"

Parser::Parser() {
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

    DeviceName deviceName = elements[0].toStdString().data();

    // Process Voltage Source
    // TODO: Update Vsrc grammer
    if (line.startsWith("V") | line.startsWith("v")) {
        if (checkNameRepetition<Vsrc>(Vsrc_vec, deviceName)) {
            parseError("Failed to parse " + deviceName + ", which already exits.",
                       lineNum);
            return;
        }

        switch (num_elements) {
                // Vx 1 0 10
            case 4: {
                double value = parseValue(elements[3]);
                NodeName node_1 = elements[1].toStdString();
                NodeName node_2 = elements[2].toStdString();
                Vsrc vsrc = {deviceName, value, node_1, node_2};
                Vsrc_vec.push_back(vsrc);

                std::cout << "Parsed Device Type: Voltage Source ("
                          << "Name: " << deviceName << "; "
                          << "Value: " << value << "; "
                          << "Node1: " << node_1 << "; "
                          << "Node2: " << node_2 << " )" << std::endl;

                break;
            }
                // Vx 1 0 dc 10
                // TODO: Wrong need to be fixed
            case 5: {
                double value = parseValue(elements[4]);
                NodeName node_1 = elements[1].toStdString();
                NodeName node_2 = elements[2].toStdString();
                Vsrc vsrc = {deviceName, value, node_1, node_2};
                Vsrc_vec.push_back(vsrc);

                std::cout << "Parsed Device Type: Voltage Source ("
                          << "Name: " << deviceName << "; "
                          << "Value: " << value << "; "
                          << "Node1: " << node_1 << "; "
                          << "Node2: " << node_2 << "; "
                          << "Type: " << elements[3].toStdString() << " )" << std::endl;
                break;
            }

            default: break;
        }
    }
    // Process Resistor
    else if (line.startsWith("R") | line.startsWith("r")) {
        if (num_elements != 4) {
            parseError("Failed to parse " + deviceName, lineNum);
        } else {
            if (checkNameRepetition<Res>(Res_vec, deviceName)) {
                parseError("Failed to parse " + deviceName + ", which already exits.",
                           lineNum);
                return;
            }

            double value = parseValue(elements[3]);
            NodeName node_1 = elements[1].toStdString();
            NodeName node_2 = elements[2].toStdString();
            Res res = {deviceName, value, node_1, node_2};
            Res_vec.push_back(res);

            std::cout << "Parsed Device Type: Resistor ("
                      << "Name: " << deviceName << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }
    // Process Capacitor
    else if (line.startsWith("C") | line.startsWith("c")) {
        if (num_elements != 4) {
            parseError("Failed to parse " + deviceName, lineNum);
        } else {
            if (checkNameRepetition<Cap>(Cap_vec, deviceName)) {
                parseError("Failed to parse " + deviceName + ", which already exits.",
                           lineNum);
                return;
            }

            double value = parseValue(elements[3]);
            NodeName node_1 = elements[1].toStdString();
            NodeName node_2 = elements[2].toStdString();
            Cap cap = {deviceName, value, node_1, node_2};
            Cap_vec.push_back(cap);

            std::cout << "Parsed Device Type: Capacitor ("
                      << "Name: " << deviceName << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }
    // Process Inductor
    else if (line.startsWith("L") | line.startsWith("l")) {
        if (num_elements != 4) {
            parseError("Failed to parse " + deviceName, lineNum);
        } else {
            if (checkNameRepetition<Ind>(Ind_vec, deviceName)) {
                parseError("Failed to parse " + deviceName + ", which already exits.",
                           lineNum);
                return;
            }

            double value = parseValue(elements[3]);
            NodeName node_1 = elements[1].toStdString();
            NodeName node_2 = elements[2].toStdString();
            Ind ind = {deviceName, value, node_1, node_2};
            Ind_vec.push_back(ind);

            std::cout << "Parsed Device Type: Inductor ("
                      << "Name: " << deviceName << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
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
            updateNodeVec();  // Program ends, update Node.
        }
    }
    // .PRINT
    else if (command == ".print") {
        if (num_elements == 1)
            parseError("Failed to parse .print, .print need parameters", lineNum);
        else if (elements[1].startsWith("i") |
                 elements[1].startsWith("v")) {  // .print V(node)
            if (analysisType == NONE)
                parseError("Failed to parse .print. Analysis type is not determined.",
                           lineNum);
            else {
                elements.removeFirst();  // remove the first one
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
            else
                parseError("Failed to parse .print. Invalid analysis type.", lineNum);

            elements.removeFirst();
            elements.removeFirst();  // remove the first two
            printParser(elements);
        }
    }
    // TODO: complete the logic
    else if (command == ".dc") {
        if (num_elements != 5)
            parseError("Failed to parse .dc", lineNum);
        else {
            DeviceName Vsrc_name = elements[1].toStdString();
            if (!checkNameRepetition<Vsrc>(Vsrc_vec, Vsrc_name))
                parseError("Failed to parse .dc. Target voltage source not exists.",
                           lineNum);
            else {
                analysisCommand.Vsrc_name = Vsrc_name;
                analysisCommand.start = elements[2].toDouble();
                analysisCommand.end = elements[3].toDouble();
                analysisCommand.step = elements[4].toDouble();

                std::cout << "Parsed Analysis Command DC "
                          << "(Vsrc: " << analysisCommand.Vsrc_name << "; "
                          << "Start: " << analysisCommand.start << "; "
                          << "End: " << analysisCommand.end << "; "
                          << "Step: " << analysisCommand.step << ")" << std::endl;
            }
        }
    }
}

// TODO: This method is far from complete.
void Parser::printParser(const QStringList elements) {
    print_T printType;
    analysisVariable_T analysisVariableType;
    NodeName node_1;
    NodeName node_2;

    QRegularExpression bracket_re("(?<=\\().*(?=\\))");

    for (QString e : elements) {
        if (e.startsWith("v")) {
            printType = V;

            QRegularExpressionMatch match = bracket_re.match(e);
            if (match.hasMatch())
                node_1 = match.captured().toStdString();

            analysisVariableType = MAG;
            node_2 = "0";
            printVariable printVar{printType, analysisVariableType, node_1, node_2};
            std::cout << "Parsed Output Coomand Print (Type: "
                      << analysis_T_lookup[analysisType] << "; "
                      << "Type: "
                      << "Voltage " << node_1 << ")" << std::endl;
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

    scaledUnit f = {"f", 1e-15};
    scaledUnit p = {"p", 1e-12};
    scaledUnit n = {"n", 1e-9};
    scaledUnit u = {"u", 1e-6};
    scaledUnit m = {"m", 1e-3};
    scaledUnit k = {"k", 1e3};
    scaledUnit meg = {"meg", 1e6};
    scaledUnit g = {"g", 1e9};
    scaledUnit t = {"t", 1e12};

    std::vector<scaledUnit> scaledUnit_vec = {f, p, n, u, m, k, meg, g, t};

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

void Parser::updateNodeVec() {
    std::vector<NodeName> tempNode_vec;

    for (auto Vsrc : Vsrc_vec) {
        tempNode_vec.push_back(Vsrc.node_1);
        tempNode_vec.push_back(Vsrc.node_2);
    }

    for (auto Res : Res_vec) {
        tempNode_vec.push_back(Res.node_1);
        tempNode_vec.push_back(Res.node_2);
    }

    for (auto Cap : Cap_vec) {
        tempNode_vec.push_back(Cap.node_1);
        tempNode_vec.push_back(Cap.node_2);
    }

    for (auto Ind : Ind_vec) {
        tempNode_vec.push_back(Ind.node_1);
        tempNode_vec.push_back(Ind.node_2);
    }

    std::set<NodeName> Node_set(tempNode_vec.begin(), tempNode_vec.end());

    Node_vec = std::vector<NodeName>(Node_set.begin(), Node_set.end());
}

/**
 * @brief Check whether the name has existed
 *
 * @tparam T the type of the struct
 * @param struct_vec
 * @param name
 * @return true
 * @return false
 */
template <typename T>
bool Parser::checkNameRepetition(std::vector<T> struct_vec, DeviceName name) {
    bool repetition = false;
    for (auto s : struct_vec)
        if (s.name == name) {
            repetition = true;
            break;
        }
    return repetition;
}