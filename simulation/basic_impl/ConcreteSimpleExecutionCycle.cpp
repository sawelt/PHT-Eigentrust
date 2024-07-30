//
// Created by laurenz on 27.06.23.
//

#include "ConcreteSimpleExecutionCycle.h"

void ConcreteSimpleExecutionCycle::run() {
    std::vector<std::shared_ptr<AbstractExecution>> executions;
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(number_of_execution_per_cycle_lower, number_of_execution_per_cycle_upper);
    auto number_of_execution_per_cycle = distrib(gen);
    executions.reserve(number_of_execution_per_cycle);
    /*
     * Choose the train users which issues executions.
     * We just choose atm the first project.
     */
    for (const auto& tu : choose_n_random_train_user(number_of_execution_per_cycle)) {
        executions.push_back(std::shared_ptr<AbstractExecution>(new ConcreteSimpleExecution(_on_peer_collection, tu, tu->projects().front())));
    }
    // simulate the executions
    for (const auto& exe : executions) {
        exe->use_virtual_result_peer();
        exe->run();
    }

}


std::shared_ptr<AbstractTrainUser> ConcreteSimpleExecutionCycle::get_random_train_user() {
    std::srand(std::time(nullptr));
    const auto random = std::rand() / ((RAND_MAX + 1u) / _on_peer_collection->number_train_users()) ;
    return _on_peer_collection->get_train_user_at_offset(random);
}

std::vector<std::shared_ptr<AbstractTrainUser>> ConcreteSimpleExecutionCycle::choose_n_random_train_user(size_t n) {
    return _on_peer_collection->get_random_train_user_subset(n);
}

