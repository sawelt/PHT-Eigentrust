//
// Created by Laurenz Neumann on 05.07.23.
// Contains all peers that act as a baseline
// These peers do not consider any trust values, but rather just choose stations and train classes randomly
//

#ifndef SIMULATION_BASELINEPEERS_H
#define SIMULATION_BASELINEPEERS_H
#include <vector>
#include <random>
#include <memory>

#include "Abstracts.h"
#include "RateHelper.h"
class BaselineTrainUser : public AbstractTrainUser {
public:
    using AbstractTrainUser::AbstractTrainUser;
    std::vector<std::shared_ptr<AbstractStation>> choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) override;

    trainClass choose_trainClass(t_project for_project, std::shared_ptr<PeerCollection> from_collection) override;

    void report_total_execution_result_quality(AbstractExecution &for_execution, t_quality result_quality,
                                               std::optional<t_peer_id> result_peer) override;

    /// @warning NOT IMPLEMENTED
    void report_interim_execution_result_quality(AbstractExecution &for_execution,
                                                 std::map<t_peer_id, t_quality> result_qualities,
                                                 std::optional<t_peer_id> result_peer) override;
protected:
    std::unique_ptr<RateHelper> rate_helper = std::unique_ptr<RateHelper>{new ForgettingRateHelper{local_trust_values}};
private:
    static void clean_candidates_list_too_expensive(std::list<std::shared_ptr<AbstractStation>> & candidates_list, t_token limit);
};

#endif //SIMULATION_BASELINEPEERS_H
