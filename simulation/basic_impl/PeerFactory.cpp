//
// Created by Laurenz Neumann on 08.05.23.
//

#include "PeerFactory.h"

#include <memory>
/**
 * Sampling a normal distribution with clipping the values at 1 and 100
 * @param mean
 * @param variation
 * @param gen
 * @return
 */
unsigned int sample_size_normal_distribution(unsigned int mean, unsigned int variation, std::mt19937 gen) {
    std::normal_distribution<float> d((float)mean, (float)variation);
    auto res = (unsigned int)std::round(d(gen));
    if (res <=0) {return 1;};
    if (res >100) {return 100;};
    return res;
}


std::shared_ptr<AbstractStation> PeerFactory::getBenignStation(t_peer_id id) {
    std::vector<t_topic_id> tp;
    tp.push_back(get_topic());
    SimulationReporter::get_simulation_reporter()->report_peer({
        .id = id,
        .role = ::Station,
        .malicious = false,
        .peer_parameters = {{"cost", "1"}},
        .initial_budget = 0,
        });
    return std::shared_ptr<AbstractStation>(new ConcreteStation(id, false, tp, 1, get_data_size_sample(false)));
}

t_topic_id PeerFactory::get_topic() {
    // return from distribution i.e. values from [0, number_of_topics]
    return d(gen);
}

t_topic_id PeerFactory::get_task() {
// TODO: change to real random task
    return 1;
}

PeerFactory::PeerFactory(size_t number_of_topics, size_t number_of_tasks)  : _number_of_topics{number_of_topics}, _number_of_tasks{number_of_tasks} {
    std::random_device rd;
    gen = std::mt19937(rd());

    std::vector<int> distribution_weights;
    for (int i=0; i<number_of_topics; ++i) {
        distribution_weights.push_back(std::pow(2,i));
    }
    d = std::discrete_distribution<int>(distribution_weights.begin(), distribution_weights.end());
}

std::shared_ptr<AbstractTrainCreator> PeerFactory::getBenignTrainCreator(t_peer_id id) {
    std::vector<trainClass> tcs;
    trainClass tc;
    tc.creator = id;
    tc.for_task = get_task();
    tc.quality = 0.8;
    tcs.push_back(tc);
    SimulationReporter::get_simulation_reporter()->report_peer({
        .id = id,
        .role = ::TrainCreator,
        .malicious = false,
        .initial_budget = 0,
    });
    return std::make_shared<AbstractTrainCreator>(id, false, tcs);
}

std::shared_ptr<AbstractTrainUser> PeerFactory::getBenignTrainUser(t_peer_id id) {
//return std::shared_ptr<AbstractTrainUser>(new QualityDrivenTrainUser(get_valid_peer_id(), false, {t_project {get_topic(), get_task()}}, 5));
const t_token _initial_budget = get_random_initial_budget();
SimulationReporter::get_simulation_reporter()->report_peer({
    .id = id,
    .role = ::TrainUser,
    .malicious = false,
    .peer_parameters = {{"greediness", "0.5"}},
    .initial_budget = _initial_budget,
});
return std::shared_ptr<AbstractTrainUser>(new BaselineTrainUser(id, false, {t_project {get_topic(), get_task()}}, _initial_budget));
}

std::shared_ptr<AbstractStation> PeerFactory::getMaliciousStation(t_peer_id id) {
    std::vector<t_topic_id> tp;
    auto topic = get_topic();
    tp.push_back(topic);
    SimulationReporter::get_simulation_reporter()->report_peer({
        .id = id,
        .role = ::Station,
        .malicious = true,
        .peer_parameters = {{"cost", "1"}, {"topic", std::to_string(topic)}},
        .initial_budget = 0,
        });
    return std::shared_ptr<AbstractStation>(new ConcreteStation(id, true, tp, 1, get_data_size_sample(true)));
}

std::shared_ptr<AbstractTrainUser> PeerFactory::getMaliciousTrainUser(t_peer_id id) {
    return {nullptr};
}

std::shared_ptr<AbstractTrainCreator> PeerFactory::getMaliciousTrainCreator(t_peer_id id) {
    return {nullptr};
}

unsigned int PeerFactory::get_data_size_sample(bool for_malicious) {
    if (for_malicious) {
        return sample_size_normal_distribution(_median_normal_data_size_malicious, _variation_normal_data_size_malicious, gen);
    } else {
        return sample_size_normal_distribution(_median_normal_data_size_benign, _variation_normal_data_size_benign, gen);
    }
}

void PeerFactory::set_initial_budget(t_token initial_budget_min, t_token initial_budget_max) {
    assert(initial_budget_min<=initial_budget_max);
    _initial_budget_min = initial_budget_min;
    _initial_budget_max = initial_budget_max;
}

std::pair<std::shared_ptr<AbstractTrainUser>, std::shared_ptr<AbstractTrainCreator>>
PeerFactory::getBenignTrainUserCreator(t_peer_id id) {
    std::vector<trainClass> tcs;
    trainClass tc;
    tc.creator = id;
    tc.for_task = get_task();
    tc.quality = 0.8;
    tcs.push_back(tc);
    const t_token _initial_budget = get_random_initial_budget();
    SimulationReporter::get_simulation_reporter()->report_peer({
                                                                       .id = id,
                                                                       .role = ::TrainUserCreator,
                                                                       .malicious = false,
                                                                       .peer_parameters = {},
                                                                       .initial_budget = _initial_budget,
                                                               });
    auto p = std::shared_ptr<FuzzyQualityDrivenUserCreator>(new FuzzyQualityDrivenUserCreator(id, false, {t_project {get_topic(), get_task()}}, _initial_budget, tcs));
    return {std::shared_ptr<AbstractTrainUser>(p), std::shared_ptr<AbstractTrainCreator>(p)};
}

std::vector<std::shared_ptr<AbstractStation>> PeerFactory::getMaliciousCollective(const std::vector<t_peer_id>& ids) {
    std::vector<std::shared_ptr<AbstractStation>> collective;
    const auto topic = get_topic();
    for (auto const id : ids) {
        auto new_malicious_member = new MaliciousCollectiveStation(id, {topic}, 1, get_data_size_sample(true));
        SimulationReporter::get_simulation_reporter()->report_peer({
                                                                           .id = id,
                                                                           .role = ::Station,
                                                                           .malicious = true,
                                                                           .peer_parameters = {{"cost", "1"}, {"topic", std::to_string(topic)}},
                                                                           .initial_budget = 0,
                                                                           });
        new_malicious_member->set_collective_peers(ids);
        collective.emplace_back(new_malicious_member);
    }
    return collective;

}

t_token PeerFactory::get_random_initial_budget() {
    std::uniform_int_distribution<> dist(_initial_budget_min, _initial_budget_max);
    return dist(gen);
}

std::shared_ptr<AbstractTrainUser> FuzzyReputationPeerFactory::getBenignTrainUser(t_peer_id id) {
    const t_token _initial_budget = get_random_initial_budget();
    SimulationReporter::get_simulation_reporter()->report_peer({
                                                                       .id = id,
                                                                       .role = ::TrainUser,
                                                                       .malicious = false,
                                                                       .peer_parameters = {},
                                                                       .initial_budget = _initial_budget,
                                                               });
    return std::shared_ptr<AbstractTrainUser>(new FuzzyQualityDrivenTrainUser(id, false, {t_project {get_topic(), get_task()}}, _initial_budget));
}

