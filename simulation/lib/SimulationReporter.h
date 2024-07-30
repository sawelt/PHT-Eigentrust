//
// Created by Laurenz Neumann on 27.04.23.
//

#ifndef SIMULATION_SIMULATIONREPORTER_H
#define SIMULATION_SIMULATIONREPORTER_H
#include <memory>
#include <iostream>
#include <chrono>
#include <optional>

#include "SimulationReporter.fwd.h"

#include "types.h"
#include "PeerCollection.h"
#include "Abstracts.h"

struct PeerReport {
    t_peer_id id;
    PeerRole role;
    bool malicious;
    /// not every peer has reasonably a budget, but we can leave this field as 0 then
    std::map<std::string, std::string> peer_parameters;
    t_token initial_budget;
};

struct execution_report {
    t_peer_id train_user;
    std::vector<t_peer_id> route;
    t_quality execution_quality;
    std::optional<t_peer_id> virtual_peer = std::nullopt;
};



/**
 * Observer of a simulation reporting the events
 * Used as a singleton with implemented subclasses
 * The Common information provided through this (abstractly) interface are communicated directly through the components
 * More specialised information found in concrete implementations only must be communicated manually through interfaces of subclasses.
 */
class SimulationReporter {
public:
    virtual void simulation_did_start();
    virtual void simulation_did_end();
    virtual void execution_did_end();
    virtual void simulation_cycle_did_start();
    virtual void simulation_cycle_did_end();
    virtual void execution_cycle_did_start();
    virtual void execution_cycle_did_end();
    virtual void report_pre_trusted(std::vector<t_peer_id> pretrusted_peers) {};
    virtual void execution_did_run(execution_report report) {};
    virtual void report_peer(PeerReport report) {};

    virtual void set_collection(std::shared_ptr<PeerCollection> collection);

    static void set_simulation_reporter(std::shared_ptr<SimulationReporter> instance);
    static std::shared_ptr<SimulationReporter> get_simulation_reporter();
private:
    static std::optional<std::shared_ptr<SimulationReporter>> _instance;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_point;
    std::chrono::milliseconds::rep duration_since_start() const;
    void time_report();
    void start_timer();
    void stop_timer();
protected:
    std::map<t_peer_id, t_normalised_trust_value> get_current_trust_values();
    std::map<t_peer_id, t_token> get_funds();
    std::shared_ptr<PeerCollection> collection;
    inline void print_msg(const std::string & msg);
};




#endif //SIMULATION_SIMULATIONREPORTER_H
