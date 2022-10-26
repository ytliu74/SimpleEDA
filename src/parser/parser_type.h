/**
 * @file ParserType.h
 * @author Yaotian Liu
 * @brief Header for type defines in Parser
 * @date 2022-10-13
 */

#if !defined(PARSERTYPE_H)
#define PARSERTYPE_H

#include <QString>
#include <iostream>

typedef QString DeviceName;
typedef QString NodeName;

struct BaseDevice {
    DeviceName name;
    double value;
    NodeName node_1;
    NodeName node_2;

    BaseDevice() : name(), value(), node_1(), node_2() {}
    BaseDevice(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : name(name), value(value), node_1(node_1), node_2(node_2) {}
};

struct Vsrc : BaseDevice {
    Vsrc() : BaseDevice() {}
    Vsrc(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
};

struct Isrc : BaseDevice {
    Isrc() : BaseDevice() {}
    Isrc(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
};

struct Res : BaseDevice {
    Res() : BaseDevice() {}
    Res(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
};

struct Cap : BaseDevice {
    Cap() : BaseDevice() {}
    Cap(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
};

struct Ind : BaseDevice {
    Ind() : BaseDevice() {}
    Ind(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
};

struct DependentSource : BaseDevice {
    NodeName ctrl_node_1;
    NodeName ctrl_node_2;

    DependentSource() {}
    DependentSource(DeviceName name, double value, NodeName node_1, NodeName node_2,
                    NodeName ctrl_node_1, NodeName ctrl_node_2)
        : BaseDevice(name, value, node_1, node_2),
          ctrl_node_1(ctrl_node_1),
          ctrl_node_2(ctrl_node_2) {}
};

struct VCCS : DependentSource {
    VCCS() {}
    VCCS(DeviceName name, double value, NodeName node_1, NodeName node_2,
         NodeName ctrl_node_1, NodeName ctrl_node_2)
        : DependentSource(name, value, node_1, node_2, ctrl_node_1, ctrl_node_2) {}
};

struct VCVS : DependentSource {
    VCVS() {}
    VCVS(DeviceName name, double value, NodeName node_1, NodeName node_2,
         NodeName ctrl_node_1, NodeName ctrl_node_2)
        : DependentSource(name, value, node_1, node_2, ctrl_node_1, ctrl_node_2) {}
};

struct Circuit {
    std::vector<Vsrc> vsrc_vec;
    std::vector<Isrc> isrc_vec;
    std::vector<VCCS> vccs_vec;
    std::vector<VCVS> vcvs_vec;
    std::vector<Res> res_vec;
    std::vector<Cap> cap_vec;
    std::vector<Ind> ind_vec;
    std::vector<NodeName> node_vec;
};

// TODO: CC

struct ScaledUnit {
    const QString unit;
    const double scaledValue;
};

enum AcVariationType { DEC, OCT, LIN };
const std::vector<std::string> AcVariationType_lookup = {"dec", "oct", "lin"};

// Vsrc_name; start; end; step;
struct DcAnalysis {
    DeviceName Vsrc_name;
    double start;
    double end;
    double step;
};

// variation_type; point_num; f_start; f_end;
struct AcAnalysis {
    AcVariationType variation_type;
    int point_num;
    double f_start;
    double f_end;
};

// t_step; t_stop; t_start
struct TranAnalysis {
    double t_step;
    double t_stop;
    double t_start;
};

enum AnalysisT { NONE, DC, AC, TRAN, NOISE, DISTO };
typedef AnalysisT PrintType;
const std::string AnalysisT_lookup[] = {"NONE", "DC", "AC", "TRAN", "NOISE", "DISTO"};

enum AnalysisVariableT { MAG, REAL, IMAGINE, PHASE, DB };
const std::string AnalysisVariableT_lookup[] = {"MAG", "REAL", "IMAGINE", "PHASE", "DB"};

enum PrintIV { I, V };
const std::string PrintIV_lookup[] = {"I", "V"};

// If print_type is not none, then this variable needs to be plotted.
struct PrintVariable {
    PrintIV print_i_v;
    PrintType print_type;
    AnalysisVariableT analysis_variable_type;
    NodeName node;
};

#endif  // PARSERTYPE_H
