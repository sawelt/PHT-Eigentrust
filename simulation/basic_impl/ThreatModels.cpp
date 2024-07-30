//
// Created by Laurenz Neumann on 10.07.23.
//

#include "ThreatModels.h"

t_reputation_column MaliciousCollectiveStation::get_normalised_trust_values_matrix(size_t num_of_peers,
                                                                                   t_reputation_column &default_value) const {
    t_reputation_column v_ntv = t_reputation_column::Constant(1,num_of_peers,0);
    if (_collective_peer_ids.size()<=1) {
        return default_value;
    }
    for (auto const peer_id : _collective_peer_ids) {
        v_ntv(0,peer_id) = peer_id == _id ? 0 : 1.0/(double)(_collective_peer_ids.size()-1);
    }
    return v_ntv;
}

t_quality MaliciousCollectiveStation::visit(AbstractExecution &in_execution) {
    earned_tokens += get_cost();
    return _get_visit_quality();
}

