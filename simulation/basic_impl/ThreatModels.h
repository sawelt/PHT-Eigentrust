//
// Created by Laurenz Neumann on 10.07.23.
//

#ifndef SIMULATION_THREATMODELS_H
#define SIMULATION_THREATMODELS_H
#include <memory>
#include "ThreatModels.fwd.h"
#include "Peers.h"
#include "Abstracts.h"
class MaliciousCollectiveStation : public AbstractStation {
public:
    MaliciousCollectiveStation(t_peer_id id, std::vector<t_topic_id> topics, t_token cost, unsigned int size_data_sample) :
            AbstractStation(id, true, std::move(topics), cost, size_data_sample) {};
    void set_collective_peers(std::vector<t_peer_id> peer_ids) {_collective_peer_ids = std::move(peer_ids);};
    /// Returns 1 for each collective peer and 0 for each other
    t_reputation_column get_normalised_trust_values_matrix(size_t num_of_peers, t_reputation_column &default_value) const override;

    t_quality visit(AbstractExecution &in_execution) override;

private:
    std::vector<t_peer_id> _collective_peer_ids;
};


#endif //SIMULATION_THREATMODELS_H
