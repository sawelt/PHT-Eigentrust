//
// Created by Laurenz Neumann on 05.07.23.
//

#include "BaselinePeers.h"

std::vector<std::shared_ptr<AbstractStation>>
BaselineTrainUser::choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) {
    const auto topic = std::get<project_topic>(for_project);
    auto candidates = from_collection->get_stations_for_topic(topic);
    if (candidates.size()>n && n!=0) {
        throw std::runtime_error{"Not enough candidates for requested fixed route size."};
    }

    std::random_device rd;
    auto gen = std::mt19937(rd());
    std::vector<std::shared_ptr<AbstractStation>> route;

    std::shuffle(candidates.begin(),candidates.end(), gen);

    for (const auto & candidate : candidates) {
        if(has_tokens()>=candidate.get()->get_cost()) {
            route.push_back(candidate);
            spend(candidate.get()->get_cost());
        }
    }

    return route;
}

trainClass
BaselineTrainUser::choose_trainClass(t_project for_project, std::shared_ptr<PeerCollection> from_collection) {
    auto possible_train_classes = from_collection->get_trains_for_task(std::get<project_task>(for_project));
    return possible_train_classes.front();
}

void
BaselineTrainUser::report_total_execution_result_quality(AbstractExecution &for_execution, t_quality result_quality,
                                                         std::optional<t_peer_id> result_peer) {
    if (result_peer.has_value()) {
        // use the result peer for rating
        rate_helper->rate_peer(result_peer.value(), result_quality);
        return;
    }
    // do not use a result peer and rate all peers manually.
    for (const auto & s : for_execution.get_route()) {
        rate_helper->rate_peer(s->get_id(), result_quality);
    }

    rate_helper->rate_peer(for_execution.get_used_class().creator, result_quality);
}

void BaselineTrainUser::report_interim_execution_result_quality(AbstractExecution &for_execution,
                                                                std::map<t_peer_id, t_quality> result_qualities,
                                                                std::optional<t_peer_id> result_peer) {
    return;

}

void
BaselineTrainUser::clean_candidates_list_too_expensive(std::list<std::shared_ptr<AbstractStation>> &candidates_list,
                                                       t_token limit) {
    candidates_list.remove_if([limit](const std::shared_ptr<AbstractStation>& s){return s->get_cost()>limit;});
}
