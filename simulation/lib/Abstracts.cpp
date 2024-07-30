//
// Created by Laurenz Neumann on 26.04.23.
//

#include "Abstracts.h"

AbstractPeer::AbstractPeer(t_peer_id id, bool is_malicious) : _id{id}, _is_malicious{is_malicious} {

}

t_peer_id AbstractPeer::get_id() const {
    return _id;
}

bool AbstractPeer::is_malicious() const {
    return _is_malicious;
}

std::map<t_peer_id, t_trust_value> AbstractPeer::get_local_trust_values() const {
    return local_trust_values;
}

t_trust_value AbstractPeer::get_local_trust_value(t_peer_id for_peer) const {
    return local_trust_values.contains(for_peer) ? local_trust_values.at(for_peer) : 0;
}

std::vector<t_normalised_trust_value> AbstractPeer::get_normalised_trust_values(size_t num_of_peers) const {
    std::vector<t_normalised_trust_value> v_ntv(num_of_peers);
    size_t sum = 0;
    // we do not allow values below 0 in this simulation, therefore, we do not need the max() in the normalized local trust value
    // calculation
    for (const auto & pair_id_tvalue : get_local_trust_values()) {
        sum += pair_id_tvalue.second;
    }

    for (const auto & pair_id_tvalue : get_local_trust_values()) {
        v_ntv[pair_id_tvalue.first] = (float)pair_id_tvalue.second / (float)sum;
    }

    return v_ntv;
}

t_reputation_column
AbstractPeer::get_normalised_trust_values_matrix(size_t num_of_peers, t_reputation_column &default_value) const {
    t_reputation_column v_ntv = t_reputation_column::Constant(1,num_of_peers,0);
    t_trust_value sum = 0;
    // we do not allow values below 0 in this simulation, therefore, we do not need the max() in the normalized local trust value
    // calculation
    for (const auto & pair_id_tvalue : get_local_trust_values()) {
        sum += pair_id_tvalue.second;
    }
    if (sum==0) {
        return default_value;
    }

    for (const auto & pair_id_tvalue : get_local_trust_values()) {
        v_ntv(0,pair_id_tvalue.first) = (t_normalised_trust_value)pair_id_tvalue.second / sum;
    }

    return v_ntv;
}

const std::vector<t_project> &AbstractTrainUser::projects() const {
    return _projects;
}

t_token AbstractTrainUser::has_tokens() const {
    return _has_tokens;
}

void AbstractTrainUser::reset_tokens() {
    _has_tokens = _initial_tokens;
}

void AbstractTrainUser::spend(t_token x) {
    _has_tokens -= x;
}

const std::vector<t_topic_id> & AbstractStation::topics() const {
    return _topics;
}

t_token AbstractStation::get_cost() const {
    return _cost;
}

t_quality AbstractStation::_get_visit_quality() const {
    return (t_quality)(1-(1/std::pow(_size_data_sample,0.2)));
}

const std::vector<trainClass> &AbstractTrainCreator::trainClasses() const {
    return _trainClasses;
}

AbstractExecution::AbstractExecution(std::shared_ptr<PeerCollection> in_context, std::shared_ptr<AbstractTrainUser> with_train_user, t_project for_project)
        : _context {std::move(in_context)}, _train_user {std::move(with_train_user)}, _for_project{for_project} {

}

trainClass AbstractExecution::get_used_class() const {
    return _used_class;
}

const std::vector<std::shared_ptr<AbstractStation>> &AbstractExecution::get_route() const {
    return _route;
}

