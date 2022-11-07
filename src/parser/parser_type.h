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
typedef QString ModelName;

enum AnalysisType { NONE, DC, AC, TRAN, NOISE, DISTO };
typedef AnalysisType PrintType;
const std::string AnalysisType_lookup[] = {"NONE", "DC", "AC", "TRAN", "NOISE", "DISTO"};

struct Pulse {
    double v1;
    double v2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
};

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
    AnalysisType analysis_type;
    Pulse pulse;

    Vsrc() : BaseDevice() {}
    Vsrc(DeviceName name, AnalysisType analysis_type, double value, NodeName node_1,
         NodeName node_2)
        : BaseDevice(name, value, node_1, node_2),
          analysis_type(analysis_type),
          pulse() {}
    Vsrc(DeviceName name, NodeName node_1, NodeName node_2, Pulse pulse)
        : BaseDevice(name, 0, node_1, node_2), pulse(pulse) {}
};

struct Isrc : BaseDevice {
    // AnalysisType analysis_type;
    double ac_value;
    double tran_const_value;

    Isrc() : BaseDevice() {}
    Isrc(DeviceName name, double value, NodeName node_1, NodeName node_2)
        : BaseDevice(name, value, node_1, node_2) {}
    Isrc(DeviceName name, double value, NodeName node_1, NodeName node_2,
         double tran_const_value)
        : BaseDevice(name, value, node_1, node_2),
          ac_value(0),
          tran_const_value(tran_const_value) {}
    Isrc(DeviceName name, double value, NodeName node_1, NodeName node_2, double ac_value,
         double tran_const_value)
        : BaseDevice(name, value, node_1, node_2),
          ac_value(ac_value),
          tran_const_value(tran_const_value) {}
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

struct Diode {
    DeviceName name;
    NodeName node_1;
    NodeName node_2;
    ModelName model;

    Diode() {}
    Diode(DeviceName name, NodeName node_1, NodeName node_2, ModelName model)
        : name(name), node_1(node_1), node_2(node_2), model(model) {}
};

struct DiodeModel {
    ModelName model;
    double i_sat;

    DiodeModel() {}
    DiodeModel(ModelName model, double i_sat) : model(model), i_sat(i_sat) {}
};

const std::vector<DiodeModel> diode_model_lut = {DiodeModel(QString("diode"), 1)};

struct Circuit {
    std::vector<Vsrc> vsrc_vec;
    std::vector<Isrc> isrc_vec;
    std::vector<VCCS> vccs_vec;
    std::vector<VCVS> vcvs_vec;
    std::vector<Res> res_vec;
    std::vector<Cap> cap_vec;
    std::vector<Ind> ind_vec;
    std::vector<Diode> diode_vec;
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
    DeviceName Vsrc_name;
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

enum AnalysisVariableT { MAG, REAL, IMAGINE, PHASE, DB };
const std::string AnalysisVariableT_lookup[] = {"MAG", "REAL", "IMAGINE", "PHASE", "DB"};

enum PrintIV { I, V };
const std::string PrintIV_lookup[] = {"I", "V"};

// If print_type is not none, then this variable needs to be plotted.
struct PrintVariable {
    PrintIV print_i_v;
    AnalysisVariableT analysis_variable_type;
    NodeName node;
};

#endif  // PARSERTYPE_H
