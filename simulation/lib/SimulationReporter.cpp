//
// Created by Laurenz Neumann on 27.04.23.
//

#include "SimulationReporter.h"

#include <utility>

std::optional<std::shared_ptr<SimulationReporter>> SimulationReporter::_instance = std::nullopt;

void SimulationReporter::set_collection(std::shared_ptr<PeerCollection> collection) {
    this->collection = std::move(collection);
}

void SimulationReporter::simulation_did_start() {
    std::cout << "Simulation did start." << std::endl;
    start_timer();
}

void SimulationReporter::simulation_did_end() {
    std::cout << "Simulation did end.";
    time_report();
    std::cout << std::endl;
}

void SimulationReporter::simulation_cycle_did_start() {
    print_msg("Simulation cycle did start.");
}

void SimulationReporter::simulation_cycle_did_end() {
    print_msg("Simulation cycle did end.");
}

void SimulationReporter::execution_cycle_did_start() {
    print_msg("Execution cycle did start.");
}

void SimulationReporter::execution_cycle_did_end() {
    print_msg("Execution cycle did end.");
}

std::chrono::milliseconds::rep SimulationReporter::duration_since_start() const {
    auto end = std::chrono::high_resolution_clock::now();
    return  std::chrono::duration_cast<std::chrono::milliseconds>(end-start_point).count();
}

void SimulationReporter::time_report() {
    std::cout << " (" << duration_since_start() << " ms)";
}

void SimulationReporter::start_timer() {
    start_point = std::chrono::high_resolution_clock::now();
}

void SimulationReporter::stop_timer() {
    end_point = std::chrono::high_resolution_clock::now();
}

void SimulationReporter::set_simulation_reporter(std::shared_ptr<SimulationReporter> instance) {
    SimulationReporter::_instance = std::move(instance);
}

std::shared_ptr<SimulationReporter> SimulationReporter::get_simulation_reporter() {
    if (SimulationReporter::_instance.has_value()) {
        return SimulationReporter::_instance.value();
    } else {
        // return dummy simulation report which does nothing.
        return {};
    }

}

std::map<t_peer_id, t_normalised_trust_value> SimulationReporter::get_current_trust_values() {
    std::map<t_peer_id, t_normalised_trust_value> result;
    for (const auto & pair : collection->map_id_peer) {
        result[pair.first] = pair.second->global_trust_value;
    }
    return result;
}

std::map<t_peer_id, t_token> SimulationReporter::get_funds() {
    std::map<t_peer_id, t_token> funds;
    for (const auto & pair : collection->map_id_station) {
        funds[pair.first] = pair.second->get_earned_tokens();
    }
    return funds;
}

void SimulationReporter::execution_did_end() {
    std::cout << "Execution did end.";
    time_report();
    std::cout << std::endl;
}

void SimulationReporter::print_msg(const std::string &msg) {
#ifndef SIMULATION_QUIETER
    //std::cout << msg;
    //time_report();
    //std::cout << std::endl;
#endif
}
