/**
 * @file Parser.cpp
 * @author Yaotian Liu
 * @brief Parser implementation
 * @date 2022-10-11
 */

#include "parser.h"

#include "utils/utils.h"

Parser::Parser() {
    command_op = false;
    command_end = false;
    analysis_type = NONE;
}

Parser::~Parser() {
    vsrc_vec.clear();
    res_vec.clear();
    cap_vec.clear();
    ind_vec.clear();
}

void Parser::DeviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    DeviceName device_name = str(elements[0]);

    // Process Voltage Source
    // TODO: Update Vsrc grammer
    if (line.startsWith("V") | line.startsWith("v")) {
        if (CheckNameRepetition<Vsrc>(vsrc_vec, device_name)) {
            ParseError("Failed to parse " + device_name + ", which already exits.",
                       lineNum);
            return;
        }

        switch (num_elements) {
                // Vx 1 0 10
            case 4: {
                double value = ParseValue(elements[3]);
                NodeName node_1 = ReadNodeName(elements[1]);
                NodeName node_2 = ReadNodeName(elements[2]);
                Vsrc vsrc = {device_name, value, node_1, node_2};
                vsrc_vec.push_back(vsrc);

                std::cout << "Parsed Device Type: Voltage Source ("
                          << "Name: " << device_name << "; "
                          << "Value: " << value << "; "
                          << "Node1: " << node_1 << "; "
                          << "Node2: " << node_2 << " )" << std::endl;

                break;
            }
                // Vx 1 0 dc 10
                // TODO: Wrong need to be fixed
            case 5: {
                double value = ParseValue(elements[4]);
                NodeName node_1 = ReadNodeName(elements[1]);
                NodeName node_2 = ReadNodeName(elements[2]);
                Vsrc vsrc = {device_name, value, node_1, node_2};
                vsrc_vec.push_back(vsrc);

                std::cout << "Parsed Device Type: Voltage Source ("
                          << "Name: " << device_name << "; "
                          << "Value: " << value << "; "
                          << "Node1: " << node_1 << "; "
                          << "Node2: " << node_2 << "; "
                          << "Type: " << elements[3] << " )" << std::endl;
                break;
            }

            default: break;
        }
    }
    // Process Resistor
    else if (line.startsWith("R") | line.startsWith("r")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Res>(res_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            Res res = {device_name, value, node_1, node_2};
            res_vec.push_back(res);

            std::cout << "Parsed Device Type: Resistor ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }
    // Process Capacitor
    else if (line.startsWith("C") | line.startsWith("c")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Cap>(cap_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            Cap cap = {device_name, value, node_1, node_2};
            cap_vec.push_back(cap);

            std::cout << "Parsed Device Type: Capacitor ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }
    // Process Inductor
    else if (line.startsWith("L") | line.startsWith("l")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Ind>(ind_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            Ind ind = {device_name, value, node_1, node_2};
            ind_vec.push_back(ind);

            std::cout << "Parsed Device Type: Inductor ("
                      << "Name: " << device_name << "; "
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
void Parser::CommandParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    QString command = elements[0];

    // .OP
    if (command == ".op") {
        if (num_elements != 1)
            ParseError("Failed to parse .op", lineNum);
        else {
            command_op = true;
            analysis_type = DC;
            std::cout << "Parsed Analysis Command .OP Token" << std::endl;
        }
    }
    // .END
    else if (command == ".end") {
        if (num_elements != 1)
            ParseError("Failed to parse .end", lineNum);
        else {
            command_end = true;
            std::cout << "Parsed .END Token" << std::endl;
            UpdateNodeVec();  // Program ends, update Node.
        }
    }
    // .PRINT
    else if (command == ".print") {
        if (num_elements == 1)
            ParseError("Failed to parse .print, .print need parameters", lineNum);
        else if (elements[1].startsWith("i") |
                 elements[1].startsWith("v")) {  // .print V(node)
            if (analysis_type == NONE)
                ParseError("Failed to parse .print. Analysis type is not determined.",
                           lineNum);
            else {
                elements.removeFirst();  // remove the first one
                PrintCommandParser(elements);
            }
        }
        // .print dc V(..)
        else {
            if (elements[1] == "dc")
                analysis_type = DC;
            else if (elements[1] == "ac")
                analysis_type = AC;
            else if (elements[1] == "tran")
                analysis_type = TRAN;
            else
                ParseError("Failed to parse .print. Invalid analysis type.", lineNum);

            elements.removeFirst();
            elements.removeFirst();  // remove the first two
            PrintCommandParser(elements);
        }
    }
    // TODO: complete the logic
    else if (command == ".dc") {
        if (num_elements != 5)
            ParseError("Failed to parse .dc", lineNum);
        else {
            DeviceName vsrc_name = str(elements[1]);
            if (!CheckNameRepetition<Vsrc>(vsrc_vec, vsrc_name))
                ParseError("Failed to parse .dc. Target voltage source not exists.",
                           lineNum);
            else {
                analysis_command.Vsrc_name = vsrc_name;
                analysis_command.start = elements[2].toDouble();
                analysis_command.end = elements[3].toDouble();
                analysis_command.step = elements[4].toDouble();

                std::cout << "Parsed Analysis Command DC "
                          << "(Vsrc: " << analysis_command.Vsrc_name << "; "
                          << "Start: " << analysis_command.start << "; "
                          << "End: " << analysis_command.end << "; "
                          << "Step: " << analysis_command.step << ")" << std::endl;
            }
        }
    }
}

// TODO: This method is far from complete.
void Parser::PrintCommandParser(const QStringList elements) {
    PrintT print_type;
    AnalysisVariableT analysis_variable_type;
    NodeName node_1;
    NodeName node_2;

    QRegularExpression bracket_re("(?<=\\().*(?=\\))");

    for (QString e : elements) {
        if (e.startsWith("v")) {
            print_type = V;

            QRegularExpressionMatch match = bracket_re.match(e);
            if (match.hasMatch())
                node_1 = match.captured().toStdString();

            analysis_variable_type = MAG;
            node_2 = "0";
            PrintVariable print_var{print_type, analysis_variable_type, node_1, node_2};
            std::cout << "Parsed Output Coomand Print (Type: "
                      << AnalysisT_lookup[analysis_type] << "; "
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
double Parser::ParseValue(const QString value_in_str) {
    double value;

    QRegExp number_with_e("\\d+(\\.\\d+)?e-?\\d+");
    if (number_with_e.exactMatch(value_in_str)) {
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

    ScaledUnit f = {"f", 1e-15};
    ScaledUnit p = {"p", 1e-12};
    ScaledUnit n = {"n", 1e-9};
    ScaledUnit u = {"u", 1e-6};
    ScaledUnit m = {"m", 1e-3};
    ScaledUnit k = {"k", 1e3};
    ScaledUnit meg = {"meg", 1e6};
    ScaledUnit g = {"g", 1e9};
    ScaledUnit t = {"t", 1e12};

    std::vector<ScaledUnit> scaled_unit_vec = {f, p, n, u, m, k, meg, g, t};

    for (auto scaled_unit : scaled_unit_vec) {
        if (value_in_str.endsWith(scaled_unit.unit)) {
            value *= scaled_unit.scaledValue;
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
void Parser::ParseError(const std::string error_msg, const int lineNum) {
    std::cout << "Error: line " << lineNum << ": " << error_msg << std::endl;
}

/**
 * @brief Read node name from QString. \
 * If node is 'gnd', convert it to '0'
 *
 * @param qstrName
 * @return NodeName
 */
NodeName Parser::ReadNodeName(const QString qstr_name) {
    NodeName node_name = str(qstr_name);
    return (node_name == "gnd") ? "0" : node_name;
}

/**
 * @brief Update and sort node_vec
 */
void Parser::UpdateNodeVec() {
    std::vector<NodeName> temp_node_vec;

    for (auto Vsrc : vsrc_vec) {
        temp_node_vec.push_back(Vsrc.node_1);
        temp_node_vec.push_back(Vsrc.node_2);
    }

    for (auto Res : res_vec) {
        temp_node_vec.push_back(Res.node_1);
        temp_node_vec.push_back(Res.node_2);
    }

    for (auto Cap : cap_vec) {
        temp_node_vec.push_back(Cap.node_1);
        temp_node_vec.push_back(Cap.node_2);
    }

    for (auto Ind : ind_vec) {
        temp_node_vec.push_back(Ind.node_1);
        temp_node_vec.push_back(Ind.node_2);
    }

    std::set<NodeName> node_set(temp_node_vec.begin(), temp_node_vec.end());

    node_vec = std::vector<NodeName>(node_set.begin(), node_set.end());
    std::sort(node_vec.begin(), node_vec.end());
}

/**
 * @brief Check whether the name has existed
 *
 * @tparam T the type of the struct
 * @param struct_vec
 * @param name
 * @return true: Found \
 * @return false: Not found
 */
template <typename T>
bool Parser::CheckNameRepetition(std::vector<T> struct_vec, DeviceName name) {
    for (auto s : struct_vec)
        if (s.name == name) {
            return true;
        }
    return false;
}

bool Parser::CheckGndNode() {
    for (auto node : node_vec) {
        if (node == "0")
            return true;
    }
    return false;
}

// TODO: Need more checks
/**
 * @brief Parser check
 *
 * @return true : Check pass \
 * @return false : Not OK
 */
bool Parser::ParserFinalCheck() {
    if (command_end)
        return CheckGndNode();
    else
        return false;
}