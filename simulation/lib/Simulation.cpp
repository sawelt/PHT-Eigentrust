//
// Created by Laurenz Neumann on 27.04.23.
//

#include "Simulation.h"

Simulation::Simulation(SimulationConfiguration with_configuration) : configuration {with_configuration} {

}

void Simulation::run(size_t number_cycles, size_t number_execution_cycles) {
    SimulationReporter::get_simulation_reporter()->simulation_did_start();
    for (int cycle = 0; cycle<number_cycles; cycle++) {
        SimulationReporter::get_simulation_reporter()->simulation_cycle_did_start();
        for (int execution_cycle = 0; execution_cycle<number_execution_cycles; execution_cycle++) {
            SimulationReporter::get_simulation_reporter()->execution_cycle_did_start();
            configuration.cycle->run();
            SimulationReporter::get_simulation_reporter()->execution_cycle_did_end();
        }
        configuration.calculation_strategy->calculate(configuration.collection);
        SimulationReporter::get_simulation_reporter()->simulation_cycle_did_end();
    }
    SimulationReporter::get_simulation_reporter()->simulation_did_end();
}
