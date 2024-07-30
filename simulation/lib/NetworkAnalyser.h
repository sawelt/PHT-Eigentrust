//
// Created by laurenz on 16.05.23.
//

#ifndef SIMULATION_NETWORKANALYSER_H
#define SIMULATION_NETWORKANALYSER_H
#include <iostream>
#include "NetworkAnalyser.fwd.h"
#include "PeerCollection.h"

class NetworkAnalyser {
public:
    static float get_average_global_trust_value_train_user(std::shared_ptr<PeerCollection> pc, bool include_malicious = true,
                                                           bool include_non_malicious = true);
    static float get_median_global_trust_value_train_creator(std::shared_ptr<PeerCollection> pc, bool include_malicious = true,
                                                             bool include_non_malicious = true);
    static float get_median_global_trust_value_station(std::shared_ptr<PeerCollection> pc, bool include_malicious = true,
                                                       bool include_non_malicious = true);
    static void print_earned_tokens(std::shared_ptr<PeerCollection> pc, bool include_malicious = true,
                                    bool include_non_malicious = true);
    /**
     * Performs sanity checks to validate that the calculation was done correctly
     * - Checks that the global trust values add up to 1
     * @param in_collection
     */
    static void sanity_check_simulated_peers(std::shared_ptr<PeerCollection> in_collection);
private:
    static void _sanity_check_global_trust_value_sum(std::shared_ptr<PeerCollection> in_collection);
};


#endif //SIMULATION_NETWORKANALYSER_H
