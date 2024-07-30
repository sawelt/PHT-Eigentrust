//
// Created by laurenz on 30.05.23.
//

#ifndef SIMULATION_RATEHELPER_H
#define SIMULATION_RATEHELPER_H
#pragma warning(disable:)
#include <types.h>

class RateHelper {
public:
    virtual void rate_peer_good(t_peer_id peer) = 0;
    virtual void rate_peer_bad(t_peer_id peer) = 0;
    /**
     * Rate a peer a specific trust value
     * @param peer the peer which should be rated
     * @param rating a value between 0 and 1 that should be rated
     */
    virtual void rate_peer(t_peer_id peer, t_trust_value rating) = 0;
    explicit RateHelper(std::map<t_peer_id, t_trust_value>& trust_values) : _trust_values {trust_values} {};
    /// necessary or compiler is sad
    virtual ~RateHelper() = default;
protected:
    std::map<t_peer_id, t_trust_value>& _trust_values;
};

class ForgettingRateHelper final : public RateHelper {
public:
    using RateHelper::RateHelper;
    void rate_peer_good(t_peer_id peer) final;
    void rate_peer_bad(t_peer_id peer) final;
    /**
     * Rate a peer a specific trust value
     * @param peer the peer which should be rated
     * @param rating a value between 0 and 1 that should be rated
     */
    void rate_peer(t_peer_id peer, t_trust_value rating) final;
};

#endif //SIMULATION_RATEHELPER_H
