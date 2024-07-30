//
// Created by Laurenz Neumann on 02.05.23.
//

#include "Peers.h"

#include <utility>

std::vector<std::shared_ptr<AbstractStation>> QualityDrivenTrainUser::choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n) {
    auto candidates = from_collection->get_stations_for_topic(std::get<project_topic>(for_project));
    // reset the money of the peer
    // lazy af
    decltype(candidates) route;
    // sort the stations by trust
    std::sort(candidates.begin(), candidates.end(), station_pointer_sorting_trust());
    // choose until exhausted
    size_t i = 0;
    auto it = candidates.begin();
    // choose from the candidates
    // we need to consider the case where multiple stations have the same trust value
    // then, these stations should be chosen non-deterministic, that is if we just follow the produced order
    // we will always choose the same stations first
    // therefore we create buckets of same station throughout the iteration and shuffle those buckets if we have not enough money
    // to take all into consideration
    while (it!=candidates.end() && (n!=0 || i!=n)) {
        // build the bucket of candidates with the same price
        decltype(candidates) bucket;
        auto bucket_price = it->get()->get_cost();
        for (;it!=candidates.end();it++) {
            if (it->get()->get_cost()==bucket_price) {
                bucket.push_back(*it);
            } else {
                break;
            }
        }
        // check if we can afford the whole bucket
        if (bucket_price*bucket.size( )<=has_tokens()) {
            route.insert(route.begin(), bucket.begin(), bucket.end());
            spend(bucket_price*bucket.size());
            i+=bucket.size();
        } else {
            // we cannot afford the whole bucket and need to sample it.
            std::random_device rd;
            auto gen = std::mt19937(rd());
            std::shuffle(bucket.begin(), bucket.end(), gen);
            auto till = bucket.begin();
            // if n is not equal 0 and i smaller than the number of stations we can afford,
            // we need to consider the defined upper route length
            if (n!=0 && (n-i)<has_tokens()/bucket_price) {
                std::advance(till, n-i);
                i = n;
            } else {
                std::advance(till, has_tokens()/bucket_price);
                i += has_tokens()/bucket_price;
            }

            route.insert(route.end(), bucket.begin(), till);
        }
    }
    return route;
}

t_quality ConcreteStation::visit(AbstractExecution& in_execution) {
    auto tc = in_execution.get_used_class();
    // for now, just rate the train creator "good"
    rate_helper->rate_peer_good(tc.creator);
    earned_tokens += get_cost();
    return _get_visit_quality();
}


std::vector<std::shared_ptr<AbstractStation>>
FuzzyQualityDrivenTrainUser::choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection,
                                          size_t n) {
    const auto topic = std::get<project_topic>(for_project);
    const auto candidates = from_collection->get_stations_for_topic(topic);
    if (candidates.size()>n && n!=0) {
        throw std::runtime_error{"Not enough candidates for requested fixed route size."};
    }
    std::list<std::shared_ptr<AbstractStation>> candidates_list;
    candidates_list.insert(candidates_list.end(),candidates.begin(),candidates.end());

    std::random_device rd;
    std::mt19937 gen(rd());
    const int WEIGHT_MAX = 1000000;
    int stations_chosen = 0;
    std::vector<std::shared_ptr<AbstractStation>> route;
    while (!candidates_list.empty() && (n==0 || stations_chosen<n)) {
        // clean all candidates that are too expensive
        clean_candidates_list_too_expensive(candidates_list, has_tokens());
        if (candidates_list.empty()) {
            break;
        }
        // expand the "probabilities" by multiplying with a high value constant to be able to exploit c++s
        // discrete_distribution sampling distribution.

        std::vector<long> weights;
        weights.reserve(candidates_list.size());
        for (auto const& candidate : candidates_list) {
            // lower bound probabilities by 0.1
            // if no global trusts values are chosen,
            weights.push_back((int)(std::max(candidate->global_trust_value,(t_normalised_trust_value)1/WEIGHT_MAX)* WEIGHT_MAX));
        }
        std::discrete_distribution<> d{weights.begin(), weights.end()};
        const auto choice = d(gen);
        // move the iterator to the chosen station
        auto it = candidates_list.begin();
        std::advance(it, choice);
        // add it to the route
        route.push_back(*it);
        // ... spend the money
        spend(it->get()->get_cost());
        // and remove the station from the candidates
        candidates_list.remove(*it);
    }

    return route;
}

void FuzzyQualityDrivenTrainUser::clean_candidates_list_too_expensive(
        std::list<std::shared_ptr<AbstractStation>> &candidates_list, t_token limit) {
    candidates_list.remove_if([limit](const std::shared_ptr<AbstractStation>& s){return s->get_cost()>limit;});
}
