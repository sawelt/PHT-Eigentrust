//
// Created by laurenz on 16.05.23.
//

#include "NetworkAnalyser.h"

float
NetworkAnalyser::get_average_global_trust_value_train_user(std::shared_ptr<PeerCollection> pc, bool include_malicious,
                                                           bool include_non_malicious) {
    const size_t n = pc->map_id_trainUser.size();
    float median = 0;
    for (const auto & pair_id_p : pc->map_id_trainUser) {
        if ((pair_id_p.second->is_malicious() && include_malicious) || (!pair_id_p.second->is_malicious() && include_non_malicious)) {
            median += pair_id_p.second->global_trust_value / n;
        }
    }
    return median;
}

float NetworkAnalyser::get_median_global_trust_value_train_creator(std::shared_ptr<PeerCollection> pc, bool include_malicious,
                                                                   bool include_non_malicious) {
    const size_t n = pc->map_id_trainCreator.size();
    float median = 0;
    for (const auto & pair_id_p : pc->map_id_trainCreator) {
        if ((pair_id_p.second->is_malicious() && include_malicious) || (!pair_id_p.second->is_malicious() && include_non_malicious)) {
            median += pair_id_p.second->global_trust_value / n;
        }

    }
    return median;
}

float NetworkAnalyser::get_median_global_trust_value_station(std::shared_ptr<PeerCollection> pc, bool include_malicious,
                                                             bool include_non_malicious) {
    const size_t n = pc->map_id_station.size();
    float median = 0;
    for (const auto & pair_id_p : pc->map_id_station) {
        if ((pair_id_p.second->is_malicious() && include_malicious) || (!pair_id_p.second->is_malicious() && include_non_malicious)) {
            median += pair_id_p.second->global_trust_value / n;
        }
    }
    return median;
}

void NetworkAnalyser::print_earned_tokens(std::shared_ptr<PeerCollection> pc, bool include_malicious,
                                          bool include_non_malicious) {
    float average_b = 0;
    int num_b = 0;
    float average_m = 0;
    int num_m = 0;
    for (const auto & paid_id_p : pc->map_id_station) {
        if (paid_id_p.second->is_malicious() && include_malicious) {
            average_m += paid_id_p.second->get_earned_tokens();
            num_m++;
        }
        if (!paid_id_p.second->is_malicious() && include_non_malicious) {
            average_b += paid_id_p.second->get_earned_tokens();
            num_b++;
        }
    }
    if (include_malicious) {
        std::cout << "Average tokens earned by malicious station:" << average_m/num_m << std::endl;
    }
    if (include_non_malicious) {
        std::cout << "Average tokens earned by benign station:" << average_b/num_b << std::endl;
    }
}

void NetworkAnalyser::sanity_check_simulated_peers(std::shared_ptr<PeerCollection> in_collection) {
    _sanity_check_global_trust_value_sum(in_collection);
}

void NetworkAnalyser::_sanity_check_global_trust_value_sum(std::shared_ptr<PeerCollection> in_collection) {
    float sum = 0;
    for(auto const & pair : in_collection->map_id_peer) {
        sum += pair.second->global_trust_value;
    }
    // ignore rounding errors etc.
    if (sum > 1.01 ) {
        throw std::logic_error("Sum of global trust values is " + std::to_string(sum) + " but should be <1");
    }
}
