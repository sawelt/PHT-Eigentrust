//
// Created by Laurenz Neumann on 26.04.23.

//

#ifndef SIMULATION_PEERCOLLECTION_H
#define SIMULATION_PEERCOLLECTION_H
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <exception>

#include "PeerCollection.fwd.h"
#include "Abstracts.h"
#include "types.h"
#include "NetworkAnalyser.h"
#include "SimulationReporter.h"



class PeerNotExistentException : std::runtime_error {
public:
    explicit PeerNotExistentException(const std::string& what) : std::runtime_error(what) {};
    explicit PeerNotExistentException(const char* what) : std::runtime_error(what) {};
};

class [[maybe_unused]] PeerCollection {
public:
    const std::vector<trainClass>& get_trains_for_task(t_task_id id);
    const std::vector<std::shared_ptr<AbstractStation>>& get_stations_for_topic(t_topic_id id);

    std::shared_ptr<AbstractTrainUser> get_train_user(t_peer_id for_id);
    std::shared_ptr<AbstractTrainUser> get_train_user_at_offset(size_t offset);
    std::vector<std::shared_ptr<AbstractTrainUser>> get_random_train_user_subset(size_t n);
    std::vector<std::shared_ptr<AbstractTrainUser>> get_all_train_users();
    std::shared_ptr<AbstractStation> get_station(t_peer_id for_id);
    std::shared_ptr<AbstractTrainCreator> get_train_creator(t_peer_id for_id);
    std::shared_ptr<AbstractPeer> get_peer(t_peer_id id);


    size_t number_train_users() const;
    size_t number_peers() const;

    void add_station(std::shared_ptr<AbstractStation> p_station);
    void add_train_user(std::shared_ptr<AbstractTrainUser> p_trainUser);
    /**
     * adds a train creator and indexes all its provided train classes.
     * Hence the train creator needs to be fully initiliased when calling this method
     * @param p_trainCreator pointer to the train creator
     */
    void add_train_creator(std::shared_ptr<AbstractTrainCreator> p_trainCreator);
    void add_virtual_peer(std::shared_ptr<AbstractVirtualPeer> p_virtualPeer);
    /**
     * Add a train user who is a train creator at the same time
     * Both superclasses need to have the same id
     * It works by indexing only the user view as the general peer, while
     * indexing the train creator view only in the train creator map
     * It also indexes the train classes
     * @param user_view a shared pointer providing a train user view on the peer
     * @param creator_view a shared pointer providing a train creator view on the peer
     */
    void add_train_user_creator(std::shared_ptr<AbstractTrainUser> user_view, std::shared_ptr<AbstractTrainCreator> creator_view);
    void add_train_user_creator(std::pair<std::shared_ptr<AbstractTrainUser>, std::shared_ptr<AbstractTrainCreator>> views);

    void add_stations(AbstractPeerFactory& from_peer_factory, bool malicious=false, size_t n=0);
    void add_train_creators(AbstractPeerFactory& from_peer_factory, bool malicious=false, size_t n=0);
    void add_train_users(AbstractPeerFactory& from_peer_factory, bool malicious=false, size_t n=0);
    [[nodiscard]] t_peer_id get_valid_peer_id();
    [[nodiscard]] std::vector<t_peer_id> get_valid_peer_ids(int n);
private:
    friend NetworkAnalyser;
    friend SimulationReporter;
    void index_general_peer(t_peer_id id, std::shared_ptr<AbstractPeer> peer);
    void _index_trains(std::shared_ptr<AbstractTrainCreator> of_creator);
    std::map<t_peer_id, std::shared_ptr<AbstractStation>> map_id_station;
    std::map<t_peer_id, std::shared_ptr<AbstractTrainUser>> map_id_trainUser;
    std::map<t_peer_id, std::shared_ptr<AbstractTrainCreator>> map_id_trainCreator;
    std::map<t_peer_id, std::shared_ptr<AbstractVirtualPeer>> map_id_virtual;
    std::map<t_task_id, std::vector<trainClass>> map_task_trainClass;
    std::map<t_peer_id, std::shared_ptr<AbstractPeer>> map_id_peer;
    std::map<t_topic_id, std::vector<std::shared_ptr<AbstractStation>>> map_topic_stations;
    t_peer_id _smallest_free_id = 0;


};



#endif //SIMULATION_PEERCOLLECTION_H
