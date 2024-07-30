//
// Created by Laurenz Neumann on 27.04.23.
// Contains Simulation Class interface
//

#ifndef SIMULATION_SIMULATION_H
#define SIMULATION_SIMULATION_H
#include "Abstracts.h"
#include "SimulationReporter.h"

struct SimulationConfiguration {
    std::shared_ptr<AbstractExecutionCycle> cycle;
    std::shared_ptr<PeerCollection> collection;
    std::shared_ptr<AbstractReputationCalculationStrategy> calculation_strategy;
};

/**
 * Manages a simulation
 */
class Simulation {
public:
    Simulation(SimulationConfiguration with_configuration);
    void run(size_t number_cycles, size_t number_execution_cycles=1);
private:
    SimulationConfiguration configuration;
    /// the number of cycles done, in each cycle number_of_execution_cycles execution cycles will be run
};


#endif //SIMULATION_SIMULATION_H
