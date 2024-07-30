//
// Created by laurenz on 27.06.23.
//

#ifndef SIMULATION_CONCRETESIMPLESIMULATIONCYCLE_H
#define SIMULATION_CONCRETESIMPLESIMULATIONCYCLE_H

#include <memory>
#include "Abstracts.h"
#include "PeerFactory.h"


class ConcreteSimpleExecutionCycle : public AbstractExecutionCycle {
public:
    /// The peer factory is used to determine a correct peer id.
    ConcreteSimpleExecutionCycle(std::shared_ptr<PeerCollection> on_peer_collection,
                                 size_t number_of_execution_per_cycle_lower,
                                 size_t number_of_execution_per_cycle_upper,
                                 std::shared_ptr<PeerFactory> peer_factory)
                                 : AbstractExecutionCycle{std::move(on_peer_collection)}, number_of_execution_per_cycle_lower {number_of_execution_per_cycle_lower}, number_of_execution_per_cycle_upper {number_of_execution_per_cycle_upper},  _peer_factory{std::move(peer_factory)} {
    };
    void run() override;
private:
    /**
     * Returns a random train user, regardless it is malicious or not
     * @return A pointer on the train user
     */
    std::shared_ptr<AbstractTrainUser> get_random_train_user();
    std::vector<std::shared_ptr<AbstractTrainUser>> choose_n_random_train_user(size_t n);
    size_t number_of_execution_per_cycle_lower;
    size_t number_of_execution_per_cycle_upper;
    std::shared_ptr<PeerFactory> _peer_factory;
};


#endif //SIMULATION_CONCRETESIMPLESIMULATIONCYCLE_H
