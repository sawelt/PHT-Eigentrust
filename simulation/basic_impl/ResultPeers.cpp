//
// Created by Laurenz Neumann on 23.05.23.
//

#include "ResultPeers.h"


SimpleForkingResultPeer::SimpleForkingResultPeer(t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class)
: AbstractVirtualResultPeer(id, std::move(with_context), std::move(station_route), creator_of_class) {
    // set every participating peer to 1
    local_trust_values.insert(std::make_pair(creator_of_class, 1));
    for (const auto s : _station_route) {
        local_trust_values.insert(std::make_pair(s, 1));
    }
    // normalisation is automatically done via the get_normalized method
}

SelfWeightingResultPeer::SelfWeightingResultPeer(t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class)
    : AbstractVirtualResultPeer(id, std::move(with_context), std::move(station_route), creator_of_class) {
    local_trust_values.insert(std::make_pair(creator_of_class, 1));
    for (const auto s : _station_route) {
        local_trust_values.insert(std::make_pair(s, std::max(_context->get_peer(s)->global_trust_value, (t_normalised_trust_value)0.0001)));
    }
}

std::map<t_peer_id, t_trust_value> SelfWeightingResultPeer::get_local_trust_values() const {
    std::map<t_peer_id, t_trust_value> l_trust_values;
    for (const auto s : _station_route) {
        l_trust_values[(t_peer_id)s] = std::max(_context->get_peer(s)->global_trust_value, 0.00001);
    }
    return l_trust_values;
}


std::shared_ptr<AbstractVirtualResultPeer> construct_result_peer(ResultPeerType type, t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class) {
    switch (type) {
        case SimpleForking:
            return std::make_shared<SimpleForkingResultPeer>(id, std::move(with_context), std::move(station_route), creator_of_class);
        case SelfWeighting:
            return std::make_shared<SelfWeightingResultPeer>(id, std::move(with_context), std::move(station_route), creator_of_class);
        case Omniscient:
            return std::make_shared<OmniscientResultPeer>(id, std::move(with_context), std::move(station_route), creator_of_class);

    }
    return std::make_shared<SimpleForkingResultPeer>(id, std::move(with_context), std::move(station_route), creator_of_class);
}

std::map<t_peer_id, t_trust_value> OmniscientResultPeer::get_local_trust_values() const {
    std::map<t_peer_id, t_trust_value> l_trust_values;
    for (const auto s : _station_route) {
        l_trust_values[(t_peer_id)s] = _context->get_station((t_peer_id )s)->_get_visit_quality();
    }
    return l_trust_values;
}
