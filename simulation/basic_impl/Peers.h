//
// Created by Laurenz Neumann on 02.05.23.
//

#ifndef SIMULATION_PEERS_H
#define SIMULATION_PEERS_H
#include "Peers.fwd.h"
#include <random>
#include <ctime>
#include <optional>
#include <stdexcept>
#include <list>

#include "Abstracts.h"
#include "BaselinePeers.h"
#include "ConcreteSimpleExecution.h"
#include "SimulationReporter.h"
#include "ResultPeers.h"
#include "custom_sorting.h"


/**
 * A simple train user who always chooses the stations with the highest quality available
 */
class QualityDrivenTrainUser : public BaselineTrainUser {
public:
    using BaselineTrainUser::BaselineTrainUser;
    std::vector<std::shared_ptr<AbstractStation>> choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) override;


};

/**
 * An extension to the quality driven train user who treats trust values as probabilities that a train
 */
class FuzzyQualityDrivenTrainUser : public QualityDrivenTrainUser {
public:
    using QualityDrivenTrainUser::QualityDrivenTrainUser;
    std::vector<std::shared_ptr<AbstractStation>>
    choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) override;
private:
    static void clean_candidates_list_too_expensive(std::list<std::shared_ptr<AbstractStation>> & candidates_list, t_token limit);
};

class FuzzyQualityDrivenUserCreator : public FuzzyQualityDrivenTrainUser, public AbstractTrainCreator {
public:
    FuzzyQualityDrivenUserCreator (t_peer_id id, bool is_malicious, std::vector<t_project> projects, t_token tokens, std::vector<trainClass> train_classes)
        : FuzzyQualityDrivenTrainUser{id, is_malicious, std::move(projects), tokens}, AbstractTrainCreator{id, is_malicious, std::move(train_classes)} {};

    PeerRole get_role() const override {
        return PeerRole::TrainUserCreator;
    }

};


/**
 * A dumb station giving a static trust value to an execution
 */
class ConcreteStation : public AbstractStation {
public:
    using AbstractStation::AbstractStation;
    t_quality visit(AbstractExecution& in_execution) override;
private:
    std::unique_ptr<RateHelper> rate_helper = std::unique_ptr<RateHelper>{new ForgettingRateHelper{local_trust_values}};
};





#endif //SIMULATION_PEERS_H
