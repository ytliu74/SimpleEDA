/**
 * @file Parser.cpp
 * @author Yaotian Liu
 * @brief Parser implementation
 * @date 2022-10-11
 */

#include "parser.h"

#include "../utils/utils.h"

Parser::Parser() {
    command_op = false;
    command_end = false;
    analysis_type = NONE;
}

Parser::Parser(QTextEdit* output) {
    command_op = false;
    command_end = false;
    analysis_type = NONE;
    this->output = output;
}

Parser::~Parser() {}

void Parser::DeviceParser(const QString line, const int lineNum) {
    QStringList elements = line.split(" ");
    int num_elements = elements.length();

    DeviceName device_name = elements[0];

    // Process Voltage Source
    // TODO: Update Vsrc grammer
    if (line.startsWith("v")) {
        if (CheckNameRepetition<Vsrc>(circuit.vsrc_vec, device_name)) {
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
                circuit.vsrc_vec.push_back(Vsrc(device_name, value, node_1, node_2));

                output->append(QString("Parsed Device Type: Voltage Source (Name: ") +
                               device_name +
                               QString("; Value: " + QString::number(value, 'f', 3)) +
                               QString("; Node1: ") + node_1 + QString("; Node2: ") +
                               node_2 + QString(")"));

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
                circuit.vsrc_vec.push_back(Vsrc(device_name, value, node_1, node_2));

                output->append(QString("Parsed Device Type: Voltage Source (Name: ") +
                               device_name +
                               QString("; Value: " + QString::number(value, 'f', 3)) +
                               QString("; Node1: ") + node_1 + QString("; Node2: ") +
                               node_2 + QString("; Type: ") + elements[3] + QString(")"));

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

    // Process Current source
    // TODO: Update Vsrc grammer
    else if (line.startsWith("i")) {
        if (CheckNameRepetition(circuit.isrc_vec, device_name)) {
            ParseError("Failed to parse " + device_name + ", which already exits.",
                       lineNum);
            return;
        }

        switch (num_elements) {
            // Ix 1 0 10
            case 4: {
                double value = ParseValue(elements[3]);
                NodeName node_1 = ReadNodeName(elements[1]);
                NodeName node_2 = ReadNodeName(elements[2]);
                circuit.isrc_vec.push_back(Isrc(device_name, value, node_1, node_2));

                output->append(QString("Parsed Device Type: Current Source (Name: ") +
                               device_name +
                               QString("; Value: " + QString::number(value, 'f', 3)) +
                               QString("; Node1: ") + node_1 + QString("; Node2: ") +
                               node_2 + QString(")"));

                std::cout << "Parsed Device Type: Current Source ("
                          << "Name: " << device_name << "; "
                          << "Value: " << value << "; "
                          << "Node1: " << node_1 << "; "
                          << "Node2: " << node_2 << " )" << std::endl;
                break;
            }

            default: break;
        }
    }

    // Process Resistor
    else if (line.startsWith("r")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Res>(circuit.res_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.res_vec.push_back(Res(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Register (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            std::cout << "Parsed Device Type: Resistor ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }

    // Process Capacitor
    else if (line.startsWith("c")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Cap>(circuit.cap_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.cap_vec.push_back(Cap(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Capacitor (Name: ") +
                           device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            std::cout << "Parsed Device Type: Capacitor ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }

    // Process Inductor
    else if (line.startsWith("l")) {
        if (num_elements != 4) {
            ParseError("Failed to parse " + device_name, lineNum);
        } else {
            if (CheckNameRepetition<Ind>(circuit.ind_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[3]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            circuit.ind_vec.push_back(Ind(device_name, value, node_1, node_2));

            output->append(QString("Parsed Device Type: Inductor (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString(")"));

            std::cout << "Parsed Device Type: Inductor ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << " )" << std::endl;
        }
    }

    // Process VCCS
    else if (line.startsWith("g")) {
        if (num_elements != 6) {
            ParseError("Failed to parse " + device_name + ". Parameter error.", lineNum);
        } else {
            if (CheckNameRepetition<VCCS>(circuit.vccs_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[5]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            NodeName ctrl_node_1 = ReadNodeName(elements[3]);
            NodeName ctrl_node_2 = ReadNodeName(elements[4]);
            circuit.vccs_vec.push_back(
                VCCS(device_name, value, node_1, node_2, ctrl_node_1, ctrl_node_2));

            output->append(QString("Parsed Device Type: VCCS (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString("; CtrlNode1: ") + ctrl_node_1 +
                           QString("; CtrlNode2: ") + ctrl_node_2 + QString(")"));

            std::cout << "Parsed Device Type: VCCS ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << "; "
                      << "CtrlNode1: " << ctrl_node_1 << "; "
                      << "CtrlNode2: " << ctrl_node_2 << " ) " << std::endl;
        }
    }

    // Process VCVS
    else if (line.startsWith("e")) {
        if (num_elements != 6) {
            ParseError("Failed to parse " + device_name + ". Parameter error.", lineNum);
        } else {
            if (CheckNameRepetition<VCVS>(circuit.vcvs_vec, device_name)) {
                ParseError("Failed to parse " + device_name + ", which already exits.",
                           lineNum);
                return;
            }

            double value = ParseValue(elements[5]);
            NodeName node_1 = ReadNodeName(elements[1]);
            NodeName node_2 = ReadNodeName(elements[2]);
            NodeName ctrl_node_1 = ReadNodeName(elements[3]);
            NodeName ctrl_node_2 = ReadNodeName(elements[4]);
            circuit.vcvs_vec.push_back(
                VCVS(device_name, value, node_1, node_2, ctrl_node_1, ctrl_node_2));

            output->append(QString("Parsed Device Type: VCVS (Name: ") + device_name +
                           QString("; Value: " + QString::number(value, 'f', 3)) +
                           QString("; Node1: ") + node_1 + QString("; Node2: ") + node_2 +
                           QString("; CtrlNode1: ") + ctrl_node_1 +
                           QString("; CtrlNode2: ") + ctrl_node_2 + QString(")"));

            std::cout << "Parsed Device Type: VCVS ("
                      << "Name: " << device_name << "; "
                      << "Value: " << value << "; "
                      << "Node1: " << node_1 << "; "
                      << "Node2: " << node_2 << "; "
                      << "CtrlNode1: " << ctrl_node_1 << "; "
                      << "CtrlNode2: " << ctrl_node_2 << " ) " << std::endl;
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
        else if (elements[1].startsWith("i") ||
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
            analysis_type = DC;
            DeviceName vsrc_name = elements[1];
            if (!CheckNameRepetition<Vsrc>(circuit.vsrc_vec, vsrc_name))
                ParseError("Failed to parse .dc. Target voltage source not exists.",
                           lineNum);
            else {
                dc_analysis.Vsrc_name = vsrc_name;
                dc_analysis.start = ParseValue(elements[2]);
                dc_analysis.end = ParseValue(elements[3]);
                dc_analysis.step = ParseValue(elements[4]);

                std::cout << "Parsed Analysis Command DC "
                          << "(Vsrc: " << dc_analysis.Vsrc_name << "; "
                          << "Start: " << dc_analysis.start << "; "
                          << "End: " << dc_analysis.end << "; "
                          << "Step: " << dc_analysis.step << ")" << std::endl;
            }
        }
    }

    // .ac
    else if (command == ".ac") {
        if (num_elements != 5)
            ParseError("Failed to parse .ac", lineNum);
        else {
            analysis_type = AC;
            AcVariationType variation_type = LIN;
            for (uint i = 0; i < AcVariationType_lookup.size(); i++) {
                if (elements[1] == qstr(AcVariationType_lookup[i])) {
                    variation_type = static_cast<AcVariationType>(i);
                    break;
                }
            }
            ac_analysis = {variation_type, elements[2].toInt(), ParseValue(elements[3]),
                           ParseValue(elements[4])};

            std::cout << "Parsed Analysis Command AC "
                      << "(Variation Type: "
                      << AcVariationType_lookup[ac_analysis.variation_type] << "; "
                      << "Points: " << ac_analysis.point_num << "; "
                      << "f_Start: " << ac_analysis.f_start << "; "
                      << "f_End: " << ac_analysis.f_end << ")" << std::endl;
        }
    }

    // .tran
    else if (command == ".tran") {
        switch (num_elements) {
            // .tran tstep tstop
            case 3: {
                analysis_type = TRAN;
                double t_step = ParseValue(elements[1]);
                double t_stop = ParseValue(elements[2]);
                double t_start = 0;  // default value
                tran_analysis = {t_step, t_stop, t_start};
                break;
            }
            // .tran tstep tstop tstart
            case 4: {
                analysis_type = TRAN;
                double t_step = ParseValue(elements[1]);
                double t_stop = ParseValue(elements[2]);
                double t_start = ParseValue(elements[3]);
                tran_analysis = {t_step, t_stop, t_start};
                break;
            }

            default: {
                ParseError("Parse .tran error.", lineNum);
                return;
            }
        }
        std::cout << "Parsed Analysis Command TRAN "
                  << "(Tstep: " << tran_analysis.t_step
                  << "; tstop: " << tran_analysis.t_stop
                  << "; tstart: " << tran_analysis.t_start << " )" << std::endl;
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
                node_1 = match.captured();

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
void Parser::ParseError(const QString error_msg, const int lineNum) {
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
    return (qstr_name == "gnd") ? QString("0") : qstr_name;
}

/**
 * @brief Update and sort node_vec
 */
void Parser::UpdateNodeVec() {
    std::vector<NodeName> temp_node_vec;

    for (auto Vsrc : circuit.vsrc_vec) {
        temp_node_vec.push_back(Vsrc.node_1);
        temp_node_vec.push_back(Vsrc.node_2);
    }

    for (auto Isrc : circuit.isrc_vec) {
        temp_node_vec.push_back(Isrc.node_1);
        temp_node_vec.push_back(Isrc.node_2);
    }

    for (auto vccs : circuit.vccs_vec) {
        temp_node_vec.push_back(vccs.node_1);
        temp_node_vec.push_back(vccs.node_2);
        temp_node_vec.push_back(vccs.ctrl_node_1);
        temp_node_vec.push_back(vccs.ctrl_node_2);
    }

    for (auto vcvs : circuit.vcvs_vec) {
        temp_node_vec.push_back(vcvs.node_1);
        temp_node_vec.push_back(vcvs.node_2);
        temp_node_vec.push_back(vcvs.ctrl_node_1);
        temp_node_vec.push_back(vcvs.ctrl_node_2);
    }

    for (auto Res : circuit.res_vec) {
        temp_node_vec.push_back(Res.node_1);
        temp_node_vec.push_back(Res.node_2);
    }

    for (auto Cap : circuit.cap_vec) {
        temp_node_vec.push_back(Cap.node_1);
        temp_node_vec.push_back(Cap.node_2);
    }

    for (auto Ind : circuit.ind_vec) {
        temp_node_vec.push_back(Ind.node_1);
        temp_node_vec.push_back(Ind.node_2);
    }

    std::set<NodeName> node_set(temp_node_vec.begin(), temp_node_vec.end());

    circuit.node_vec = std::vector<NodeName>(node_set.begin(), node_set.end());
    std::sort(circuit.node_vec.begin(), circuit.node_vec.end());
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
    for (auto node : circuit.node_vec) {
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