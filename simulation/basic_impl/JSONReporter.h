//
// Created by Laurenz Neumann on 21.06.23.
//

#ifndef SIMULATION_JSONREPORTER_H
#define SIMULATION_JSONREPORTER_H
#include <map>
#include <string>
#include <fstream>
#include "SimulationReporter.h"
#include "Abstracts.h"
#include "json.h"

struct simulation_cycle_report {
    std::map<t_peer_id, t_normalised_trust_value> pre_global_trust_values;
    std::map<t_peer_id, t_normalised_trust_value> post_global_trust_values;
    std::vector<execution_report> executions;
    /// the amount of funds each peer has after this simulaiton cycle.
    std::map<t_peer_id, t_token> post_funds;
    void populate_json(nlohmann::json & j);
};

struct simulation_report {
    /// dynamic parameters
    std::map<std::string, std::string> parameters;
    std::vector<t_peer_id> pre_trusted;
    std::map<t_peer_id, t_normalised_trust_value> multiplex_pretrusted;
    std::vector<simulation_cycle_report> simulation_cycles;
    std::vector<PeerReport> peer_reports;
    void populate_json(nlohmann::json & j);
};

template<class K, class V>
std::map<std::string, V> stringify_map_keys(std::map<K,V> m);

nlohmann::json to_json(const PeerReport& p);
nlohmann::json to_json(const execution_report& ep);
/**
 * Provides additional interfaces for information (such as multiplex) and the possibility to export the log as json
 */
class JSONReporter : public SimulationReporter{
public:
    void report_parameter(const std::string& identifier, std::string value);
    void report_parameter(std::string identifier, auto value) { report_parameter(identifier, std::to_string(value));};

    void simulation_did_start() override;

    void simulation_did_end() override;

    void simulation_cycle_did_start() override;

    void simulation_cycle_did_end() override;

    void execution_cycle_did_start() override;

    void execution_cycle_did_end() override;

    void report_pre_trusted(std::vector<t_peer_id> pretrusted_peers) override;

    void report_multiplex_trust(std::map<t_peer_id, t_normalised_trust_value> multiplex_trust_values);

    void execution_did_run(execution_report report) override;

    void report_peer(PeerReport report) override;

    void write_to(std::ofstream &output, bool beautify);

private:
    std::vector<std::shared_ptr<simulation_report>> _reports;
    std::shared_ptr<simulation_report> _report = std::make_shared<simulation_report>();
    simulation_cycle_report* _current_cycle_report = nullptr;


};


#endif //SIMULATION_JSONREPORTER_H
