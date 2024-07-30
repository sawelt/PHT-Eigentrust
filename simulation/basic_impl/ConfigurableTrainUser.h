//
// Created by Laurenz Neumann on 25.05.23.
//

#ifndef SIMULATION_CONFIGURABLETRAINUSER_H
#define SIMULATION_CONFIGURABLETRAINUSER_H
#include "Abstracts.h"
#include "Peers.h"
#include "RateHelper.h"

typedef float t_station_score;

class ConfigurableTrainUser : public AbstractTrainUser {
public:
    ConfigurableTrainUser(t_peer_id id, bool isMalicious, const std::vector<t_project> &projects, t_token tokens, float greediness_factor);

    using AbstractTrainUser::AbstractTrainUser;
    std::vector<std::shared_ptr<AbstractStation>> choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) override;
    trainClass choose_trainClass(t_project for_project, std::shared_ptr<PeerCollection> from_collection) override;
    void report_total_execution_result_quality(AbstractExecution &for_execution, t_quality result_quality,
                                               std::optional<t_peer_id> result_peer) override;

    void report_interim_execution_result_quality(AbstractExecution &for_execution,
                                                 std::map<t_peer_id, t_quality> result_qualities,
                                                 std::optional<t_peer_id> result_peer) override;

private:
    t_station_score _calculate_local_station_score(const std::shared_ptr<AbstractStation> &for_station,
                                                   t_token max_cost) const;
    float _greediness_factor;
    ForgettingRateHelper rate_helper {local_trust_values};
};


#endif //SIMULATION_CONFIGURABLETRAINUSER_H
