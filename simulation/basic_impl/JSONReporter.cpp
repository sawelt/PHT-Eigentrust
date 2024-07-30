//
// Created by Laurenz Neumann on 21.06.23.
//

#include "JSONReporter.h"

#include <utility>

void JSONReporter::simulation_did_start() {
    SimulationReporter::simulation_did_start();
}

void JSONReporter::simulation_did_end() {
    _reports.push_back(_report);
    _report = std::make_shared<simulation_report>();
    SimulationReporter::simulation_did_end();
}

void JSONReporter::simulation_cycle_did_start() {
    _report->simulation_cycles.push_back(simulation_cycle_report{});
    _current_cycle_report = &_report->simulation_cycles.back();
    _current_cycle_report->pre_global_trust_values = get_current_trust_values();
    SimulationReporter::simulation_cycle_did_start();
}

void JSONReporter::simulation_cycle_did_end() {
    _current_cycle_report->post_global_trust_values = get_current_trust_values();
    _current_cycle_report->post_funds = get_funds();
    SimulationReporter::simulation_cycle_did_end();
}

void JSONReporter::execution_cycle_did_start() {
    SimulationReporter::execution_cycle_did_start();
}

void JSONReporter::execution_cycle_did_end() {
    SimulationReporter::execution_cycle_did_end();
}

void JSONReporter::report_parameter(const std::string& identifier, std::string value) {
    _report->parameters[identifier] = std::move(value);
}

void JSONReporter::report_pre_trusted(std::vector<t_peer_id> pretrusted_peers) {
    _report->pre_trusted = std::move(pretrusted_peers);
}

void JSONReporter::execution_did_run(execution_report report) {
    _current_cycle_report->executions.push_back(report);
    SimulationReporter::execution_did_run(report);
}


void JSONReporter::report_peer(PeerReport report) {
    _report->peer_reports.push_back(std::move(report));
    SimulationReporter::report_peer(report);
}

void JSONReporter::report_multiplex_trust(std::map<t_peer_id, t_normalised_trust_value> multiplex_trust_values) {
    _report->multiplex_pretrusted = std::move(multiplex_trust_values);
}

void JSONReporter::write_to(std::ofstream &output, bool beautify) {
    nlohmann::json j;
    for (auto const & p_report : _reports) {
        nlohmann::json report_j;
        for (auto & peer_report : p_report->peer_reports) {
            report_j["peers"].push_back(to_json(peer_report));
        }
        p_report->populate_json(report_j);
        j["simulations"].push_back(report_j);
    }
    if (beautify) {
        output << std::setw(4) << j;
    } else {
        output << j;
    }

}

void simulation_report::populate_json(nlohmann::json & j) {
    j["parameters"] = parameters;
    j["pre_trusted"] = pre_trusted;
    j["multiplex_pretrusted"] = multiplex_pretrusted;
    for (auto & cycle : simulation_cycles) {
        nlohmann::json cycle_j;
        cycle.populate_json(cycle_j);
        j["cycles"].push_back(cycle_j);
    }
}

void simulation_cycle_report::populate_json(nlohmann::json & j) {
    j["pre_global_trust_values"] = stringify_map_keys(pre_global_trust_values);
    j["post_global_trust_values"] = stringify_map_keys(post_global_trust_values);
    j["post_funds"] = stringify_map_keys(post_funds);
    for (auto const & exe : executions) {
        j["executions"].push_back(to_json(exe));
    }
}

nlohmann::json to_json(const PeerReport &p) {
    nlohmann::json j;
    j["id"] = p.id;
    j["malicious"] = p.malicious;
    j["parameters"] = p.peer_parameters;
    j["initial_budget"] = p.initial_budget;
    switch(p.role) {
        case Station:
            j["role"] = "station";
            break;
        case TrainUser:
            j["role"] = "train_user";
            break;
        case TrainCreator:
            j["role"] = "train_creator";
            break;
        case None:
            j["role"] = "none";
            break;
        case VirtualPeer:
            j["role"] = "virtual";
            break;
        case TrainUserCreator:
            j["role"] = "usercreator";
            break;
    }
    return j;
}

nlohmann::json to_json(const execution_report &ep) {
    nlohmann::json j;
    j["train_user"] = ep.train_user;
    j["route"] = ep.route;
    j["quality"] = ep.execution_quality;
    if (ep.virtual_peer.has_value()) {
        j["virtual_peer"] = ep.virtual_peer.value();
    }
    return j;
}

template<class K, class V>
std::map<std::string, V> stringify_map_keys(std::map<K, V> m) {
    std::map<std::string, V> res;
    for (auto const & pair : m) {
        res[std::to_string(pair.first)] = pair.second;
    }
    return res;
}
