//
// Created by Laurenz Neumann on 26.04.23.
//

#include "PeerCollection.h"

#include <utility>



const std::vector<std::shared_ptr<AbstractStation>> &PeerCollection::get_stations_for_topic(t_topic_id id) {
    if (!map_topic_stations.contains(id)) {
        map_topic_stations.emplace(id, std::vector<std::shared_ptr<AbstractStation>>{});
    }
    return map_topic_stations.at(id);
}

std::shared_ptr<AbstractTrainUser> PeerCollection::get_train_user(t_peer_id for_id) {
    return map_id_trainUser.at(for_id);
}

std::shared_ptr<AbstractStation> PeerCollection::get_station(t_peer_id for_id) {
    return map_id_station.at(for_id);
}

std::shared_ptr<AbstractTrainCreator> PeerCollection::get_train_creator(t_peer_id for_id) {
    return map_id_trainCreator.at(for_id);
}

void PeerCollection::add_station(std::shared_ptr<AbstractStation> p_station) {
    auto id = p_station->get_id();
    auto topics = p_station->topics();

    if (map_id_station.contains(id)) {
        throw std::runtime_error{"Conflicting station ids"};
    }
    map_id_station.emplace(id, p_station);
    index_general_peer(id, p_station);
    for (auto t : topics) {
        if (map_topic_stations.contains(t)) {
            map_topic_stations.at(t).push_back(p_station);
        } else {
            map_topic_stations.emplace(t, std::vector<std::shared_ptr<AbstractStation>>{{p_station}});
        }
    }

}

void PeerCollection::add_train_user(std::shared_ptr<AbstractTrainUser> p_trainUser) {
    auto id = p_trainUser->get_id();
    if (map_id_trainUser.contains(id)) {
        throw std::runtime_error{"Conflicting peer ids for train user"};
    }
    index_general_peer(id, p_trainUser);
    map_id_trainUser.emplace(id, std::move(p_trainUser));
}

void PeerCollection::add_train_creator(std::shared_ptr<AbstractTrainCreator> p_trainCreator) {
    auto id = p_trainCreator->get_id();


    if (map_id_trainCreator.contains(id)) {
        throw std::runtime_error{"Conflicting peer ids for train creator"};
    }
    _index_trains(p_trainCreator);
    index_general_peer(id, p_trainCreator);
    map_id_trainCreator.emplace(id,std::move(p_trainCreator));


}

void PeerCollection::add_virtual_peer(std::shared_ptr<AbstractVirtualPeer> p_virtualPeer) {
    auto id = p_virtualPeer->get_id();
    if (map_id_virtual.contains(id)) {
        throw std::runtime_error{"Conflicting peer ids for virtual peer"};
    }
    index_general_peer(id, p_virtualPeer);
    map_id_virtual.emplace(id, std::move(p_virtualPeer));

}

const std::vector<trainClass> &PeerCollection::get_trains_for_task(t_task_id id) {
    return map_task_trainClass.at(id);
}

size_t PeerCollection::number_train_users() const {
    return map_id_trainUser.size();
}

std::shared_ptr<AbstractTrainUser> PeerCollection::get_train_user_at_offset(size_t offset) {
    auto it = map_id_trainUser.begin();
    std::advance(it, offset);
    return it->second;
}

std::vector<std::shared_ptr<AbstractTrainUser>> PeerCollection::get_random_train_user_subset(size_t n) {
    std::vector<t_peer_id> peers;
    peers.reserve(map_id_trainUser.size());
    for (const auto& kv : map_id_trainUser) {
        peers.push_back(kv.first);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(peers.begin(), peers.end(), g);
    std::vector<std::shared_ptr<AbstractTrainUser>> result;
    result.reserve(std::min(n, peers.size()));
    for (int i = 0; i < std::min(n, peers.size()); ++i) {
        result.insert(result.end(), map_id_trainUser.at(peers[i]));
    }
    return result;
}

size_t PeerCollection::number_peers() const {
    return map_id_peer.size();
}

void PeerCollection::index_general_peer(t_peer_id id, std::shared_ptr<AbstractPeer> peer) {
    map_id_peer.emplace(id, std::move(peer));
}

std::shared_ptr<AbstractPeer> PeerCollection::get_peer(t_peer_id id) {
    return map_id_peer.at(id);
}

void PeerCollection::add_stations(AbstractPeerFactory &from_peer_factory, bool malicious, size_t n) {
    for (int i=0; i<n; i++) {
        malicious ? add_station(from_peer_factory.getMaliciousStation(get_valid_peer_id())) : add_station(from_peer_factory.getBenignStation(get_valid_peer_id()));
    }
}

void PeerCollection::add_train_creators(AbstractPeerFactory &from_peer_factory, bool malicious, size_t n) {
    for (int i=0; i<n; i++) {
        malicious ? add_train_creator(from_peer_factory.getMaliciousTrainCreator(get_valid_peer_id())) : add_train_creator(from_peer_factory.getBenignTrainCreator(get_valid_peer_id()));
    }
}

void PeerCollection::add_train_users(AbstractPeerFactory &from_peer_factory, bool malicious, size_t n) {
    for (int i=0; i<n; i++) {
        malicious ? add_train_user(from_peer_factory.getMaliciousTrainUser(get_valid_peer_id())) : add_train_user(from_peer_factory.getBenignTrainUser(get_valid_peer_id()));
    }
}

t_peer_id PeerCollection::get_valid_peer_id() {
    auto i = _smallest_free_id;
    _smallest_free_id++;
    return i;
}

std::vector<std::shared_ptr<AbstractTrainUser>> PeerCollection::get_all_train_users() {
    std::vector<std::shared_ptr<AbstractTrainUser>> result;
    result.reserve(map_id_trainUser.size());
    for (const auto & i : map_id_trainUser) {
        result.push_back(i.second);
    }
    return result;
}

void PeerCollection::add_train_user_creator(std::shared_ptr<AbstractTrainUser> user_view,
                                            std::shared_ptr<AbstractTrainCreator> creator_view) {
    add_train_user(std::move(user_view));
    _index_trains(std::move(creator_view));
}

void PeerCollection::_index_trains(std::shared_ptr<AbstractTrainCreator> of_creator) {
    const auto & trains = of_creator->trainClasses();
    for (const auto & t : trains) {
        if (map_task_trainClass.contains(t.for_task)) {
            map_task_trainClass.at(t.for_task).push_back(t);
        } else {
            map_task_trainClass.emplace(t.for_task, std::vector<trainClass>{{t}});
        }
    }
}

void PeerCollection::add_train_user_creator(std::pair<std::shared_ptr<AbstractTrainUser>, std::shared_ptr<AbstractTrainCreator>> views) {
    add_train_user_creator(views.first, views.second);
}

std::vector<t_peer_id> PeerCollection::get_valid_peer_ids(int n) {
    std::vector<t_peer_id> res;
    res.reserve(n);
    for(int i = 0; i < n; i++) {
        res.push_back(get_valid_peer_id());
    }
    return res;
}






