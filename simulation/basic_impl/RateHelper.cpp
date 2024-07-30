//
// Created by laurenz on 30.05.23.
//

#include "RateHelper.h"

void ForgettingRateHelper::rate_peer_good(t_peer_id peer) {
    _trust_values.insert(std::make_pair(peer,1));
}

void ForgettingRateHelper::rate_peer_bad(t_peer_id peer) {
    _trust_values.insert(std::make_pair(peer,0));
}

void ForgettingRateHelper::rate_peer(t_peer_id peer, t_trust_value rating) {
    assert(0<=rating && rating <=1);
    _trust_values.insert(std::make_pair(peer,rating));
}
