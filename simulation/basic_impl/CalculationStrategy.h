//
// Created by Laurenz Neumann on 09.05.23.
//

#ifndef SIMULATION_CALCULATIONSTRATEGY_H
#define SIMULATION_CALCULATIONSTRATEGY_H
#include <Eigen/Eigen>
#include <stdexcept>
#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include "Abstracts.h"



class CalculationStrategy : public AbstractReputationCalculationStrategy {
public:
    explicit CalculationStrategy(size_t max_calculation_rounds=1000, float a=0.2, float cutoff_threshold_difference=0.1);
    /**
     * Choose a pretrusted peer set
     * @param n number of pre trusted peers
     * @param from_collection peer collection to choose from
     * @param stations flag indicating whether stations are allowed as pretrusted peers
     * @param users flag indicating whether users are allowed as pretrusted peers
     * @param creators flag indicating whether train creators are allowed as pretrusted peers
     */
    void choose_pretrusted(size_t n, std::shared_ptr<PeerCollection> from_collection, bool stations=true, bool users=true,
                           bool creators=true, bool allow_malicious=false);
    void calculate(std::shared_ptr<PeerCollection> peer_collection) override;
    /**
     * Get the pretrusted vector initialised
     * @param num_of_peers Specify this value if the pretrusted vector needs to be resized to some size by appending 0es. Used if the
     *  number of peers changed throughout the execution
     * @return the (potentially resized) pretrusted vector
     */
    [[nodiscard]] t_reputation_vector get_pretrusted_vector(size_t num_of_peers) const override;
protected:
    size_t _max_calculation_rounds;
    float _a;
    float _cutoff_threshold_difference=0.1;
    std::vector<t_peer_id> _pretrusted_peers;
    bool pretrusted_initialised = false;
    bool initial_calulation = true;
};

/**
 * Implements additive multiplexing with offset factor beta
 */
class MultiplexCalculationStrategy : public CalculationStrategy {
public:
    explicit MultiplexCalculationStrategy(size_t max_calculation_rounds=1000, float a=0.2, float beta=0.5, float cutoff_threshold_difference=0.1);
    /**
     * Overrides the pretrusted vector getter method to be return a vector incoorporating the multiplex trust vector
     * @param num_of_peers
     * @return
     */
    t_reputation_vector get_pretrusted_vector(size_t num_of_peers) const override;
    /**
     * Sets the multiplex trust value for the calculation
     * @param trust_values the trust values
     */
    void set_multiplex_trust(std::map<t_peer_id, t_normalised_trust_value> trust_values);
    void choose_random_multiplex_trust_values(std::vector<t_peer_id> for_peers);
    std::map<t_peer_id, t_normalised_trust_value> get_multiplex_trust_values() const;
private:
    t_reputation_vector get_multiplex_vector_p(size_t num_of_peers) const;
    std::map<t_peer_id, t_normalised_trust_value> _multiplex_trust_values;
    float _beta;
};

#endif //SIMULATION_CALCULATIONSTRATEGY_H
