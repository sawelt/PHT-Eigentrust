//
// Created by Laurenz Neumann on 25.05.23.
//

#include "ConfigurableTrainUser.h"

std::vector<std::shared_ptr<AbstractStation>> ConfigurableTrainUser::choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) {
    auto candidates = from_collection->get_stations_for_topic(std::get<project_topic>(for_project));
    // lazy af
    decltype(candidates) route;
    // find out the highest price
    t_token highest_price = 0;
    for (const auto & s : candidates) {
        highest_price = s->get_cost() > highest_price ? s->get_cost() : highest_price;
    }
    // we shuffle the station list pre-sorting to prevent deterministic order in the case of two stations with the same score
    std::random_device rd;
    auto gen = std::mt19937(rd());
    std::shuffle(candidates.begin(), candidates.end(), gen);
    // sort the stations by trust
    std::sort(candidates.begin(), candidates.end(), [this, highest_price](std::shared_ptr<AbstractStation>& a, std::shared_ptr<AbstractStation>& b) -> bool {
        return this->_calculate_local_station_score(a, highest_price) > this->_calculate_local_station_score(b, highest_price);
    });
    // choose until exhausted
    size_t i = 0;
    for (auto const & c : candidates) {
        // if we have enough money, choose the station
        if (n>0 && i==n) {
            break;
        }
        if (has_tokens()>=c->get_cost()) {
            route.push_back(c);
            spend(c->get_cost());
            i++;
        }

    }
    return route;
}

trainClass
ConfigurableTrainUser::choose_trainClass(t_project for_project, std::shared_ptr<PeerCollection> from_collection) {
    auto possible_train_classes = from_collection->get_trains_for_task(std::get<project_task>(for_project));
    std::sort(possible_train_classes.begin(), possible_train_classes.end(), [] (trainClass a, trainClass b) {return a.quality>b.quality;});
    return possible_train_classes.front();
}

void ConfigurableTrainUser::report_total_execution_result_quality(AbstractExecution &for_execution,
                                                                   t_quality result_quality,
                                                                   std::optional<t_peer_id> result_peer) {
    if (result_peer.has_value()) {
        // use the result peer for rating
        if (result_quality>0.5) {
            rate_helper.rate_peer_good(result_peer.value());
        } else {
            rate_helper.rate_peer_bad(result_peer.value());
        }
        return;
    }
    // do not use a result peer and rate all peers manually.
    for (const auto & s : for_execution.get_route()) {
        if (result_quality>0.5) {
            rate_helper.rate_peer_good(s->get_id());
        } else {
            rate_helper.rate_peer_bad(s->get_id());
        }
    }

    if (result_quality>0.5) {
        rate_helper.rate_peer_good(for_execution.get_used_class().creator);
    }
}

void ConfigurableTrainUser::report_interim_execution_result_quality(AbstractExecution &for_execution,
                                                                     std::map<t_peer_id, t_quality> result_qualities,
                                                                     std::optional<t_peer_id> result_peer) {
    // DO NOTHING YAY
    return;
}

ConfigurableTrainUser::ConfigurableTrainUser(t_peer_id id, bool isMalicious, const std::vector<t_project> &projects,
                                             t_token tokens, float greediness_factor) : AbstractTrainUser(id, isMalicious, projects, tokens), _greediness_factor{greediness_factor} {}

t_station_score
ConfigurableTrainUser::_calculate_local_station_score(const std::shared_ptr<AbstractStation> &for_station,
                                                      t_token max_cost) const {
    return (1-_greediness_factor)*for_station->global_trust_value * _greediness_factor/for_station->get_cost();
}
