//
// Created by Laurenz Neumann on 23.05.23.
//

#ifndef SIMULATION_RESULTPEERS_H
#define SIMULATION_RESULTPEERS_H

//#include "ResultPeers.fwd.h"

#include <utility>

#include "Abstracts.h"
#include "types.h"

enum ResultPeerType {
    SimpleForking,
    SelfWeighting,
    Omniscient
};

class SimpleForkingResultPeer : public AbstractVirtualResultPeer {
public:
    SimpleForkingResultPeer(t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class);
};

class SelfWeightingResultPeer : public AbstractVirtualResultPeer {
public:
    SelfWeightingResultPeer(t_peer_id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class);

    [[nodiscard]] std::map<t_peer_id, t_trust_value> get_local_trust_values() const override;
};

class OmniscientResultPeer : public AbstractVirtualResultPeer {
public:
    OmniscientResultPeer(t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class)
        : AbstractVirtualResultPeer(id, std::move(with_context), std::move(station_route), creator_of_class) {};

    [[nodiscard]] std::map<t_peer_id, t_trust_value> get_local_trust_values() const override;
};

std::shared_ptr<AbstractVirtualResultPeer> construct_result_peer(ResultPeerType type, t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class);

#endif //SIMULATION_RESULTPEERS_H
