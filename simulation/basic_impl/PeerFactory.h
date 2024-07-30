//
// Created by Laurenz Neumann on 08.05.23.
//

#ifndef SIMULATION_PEERFACTORY_H
#define SIMULATION_PEERFACTORY_H
#include "PeerFactory.fwd.h"
#include <random>
#include <cmath>
#include "Abstracts.h"
#include "Peers.h"
#include "ConfigurableTrainUser.h"
#include "SimulationReporter.h"
#include "ThreatModels.h"

class PeerFactory : public AbstractPeerFactory {
public:
    PeerFactory(size_t number_of_topics, size_t number_of_tasks);
    std::shared_ptr<AbstractStation> getBenignStation(t_peer_id id) override;
    std::shared_ptr<AbstractTrainUser> getBenignTrainUser(t_peer_id id) override;
    std::shared_ptr<AbstractTrainCreator> getBenignTrainCreator(t_peer_id id) override;
    std::shared_ptr<AbstractStation> getMaliciousStation(t_peer_id id) override;
    std::vector<std::shared_ptr<AbstractStation>> getMaliciousCollective(const std::vector<t_peer_id>& ids);
    std::pair<std::shared_ptr<AbstractTrainUser>, std::shared_ptr<AbstractTrainCreator>>
    getBenignTrainUserCreator(t_peer_id id) override;

    void set_initial_budget(t_token initial_budget_min, t_token initial_budget_max);
private:

    std::shared_ptr<AbstractTrainUser> getMaliciousTrainUser(t_peer_id id) override;

    std::shared_ptr<AbstractTrainCreator> getMaliciousTrainCreator(t_peer_id id) override;
protected:
    /**
     * Returns a topic following a zipf distribution
     * @return a topic id
     */
    t_topic_id get_topic();
    /**
     * Returns a task at random
     * @return a task id
     */
    t_topic_id get_task();
    size_t _number_of_topics;
    size_t _number_of_tasks;
    std::discrete_distribution<> d;
    std::mt19937 gen;
    /**
     * Returns a random data set size for a station. Number is dependent on whether it is malicious or not.
     * @param for_malicious If the data size is for a malicious station
     * @return a random data size n with 0<n<=100
     */
    unsigned int get_data_size_sample(bool for_malicious);
    t_token get_random_initial_budget();
    unsigned int _median_normal_data_size_benign = 50;
    unsigned int _variation_normal_data_size_benign = 15;
    unsigned int _median_normal_data_size_malicious = 2;
    unsigned int _variation_normal_data_size_malicious = 1;
    t_token _initial_budget_min = 5;
    t_token _initial_budget_max = 5;
};

class FuzzyReputationPeerFactory : public PeerFactory {
public:
    using PeerFactory::PeerFactory;
    std::shared_ptr<AbstractTrainUser> getBenignTrainUser(t_peer_id id) override;
};

#endif //SIMULATION_PEERFACTORY_H
