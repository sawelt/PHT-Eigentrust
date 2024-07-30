/**
 * @file Collection of abstract interfaces used in the simulation.
 * @author Laurenz Neumann
 * @date 26.04.23
 * @note Please ignore the not-so-beautiful header inline constructors
 */

#ifndef SIMULATION_ABSTRACTS_H
#define SIMULATION_ABSTRACTS_H
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <Eigen/Eigen>
#include <optional>
#include "types.h"
#include "Abstracts.fwd.h"
#include "PeerCollection.h"
/*
 * AbstractPeer class implementing common features of all peers
 * Provides common interface
 */
class AbstractPeer {
public:
    AbstractPeer(t_peer_id id, bool is_malicious);
    [[nodiscard]] t_peer_id get_id() const;
    [[nodiscard]] bool is_malicious() const;
    /**
     * the global trust value of this peer, can be mutated by e.g. the calculation strategy.
     * used by other peers decisions.
     */
    t_normalised_trust_value global_trust_value = 0;
    /**
     * Returns the local trust values of this peer
     * @return
     */
    virtual std::map<t_peer_id, t_trust_value> get_local_trust_values() const;
    /**
     * Returns a vector containing all normalised trust values, for each peer by its id.
     * @param num_of_peers the current number of peer, i.e. the size of the vector
     * @return vector containing for each peer a normalised trust values
     * @deprecated use the matrix based method instead
     */
    virtual std::vector<t_normalised_trust_value> get_normalised_trust_values(size_t num_of_peers) const;
    /**
     * Returns a Eigen-compatible vector containing all normalised trust values, for each peer by its id.
     * gets a default value which is returned if the sum of all trust value is 0, i.e. no local trust value has been set.
     * @virtual allows virtual peers to implement this method as a function.
     */
    virtual t_reputation_column
    get_normalised_trust_values_matrix(size_t num_of_peers, t_reputation_column &default_value) const;
    /**
     *
     * @param for_peer the peer for which the trust value should be returned
     * @return the trust value if existent, 0 otherwise
     */
    t_trust_value get_local_trust_value(t_peer_id for_peer) const;
    virtual PeerRole get_role() const { return PeerRole::None; };
protected:
    std::map<t_peer_id, t_trust_value> local_trust_values;
    t_peer_id _id;
    bool _is_malicious;
};

/**
 * A TrainUser has projects in which she wants to perform
 * A project is a tuple of a topic and a task
 */
class AbstractTrainUser : public AbstractPeer {
public:
    AbstractTrainUser (t_peer_id id, bool is_malicious, std::vector<t_project> projects, t_token tokens) :
        AbstractPeer{id, is_malicious},  _projects {std::move(projects)}, _initial_tokens {tokens} {};
    [[nodiscard]] const std::vector<t_project> & projects() const;
    [[nodiscard]] t_token has_tokens() const;
    /**
     * Resets the tokens to the initial value.
     */
    void reset_tokens();
    void spend(t_token x);
    /**
     * Let the TrainUser choose a series of stations for an execution on a project the train user has
     * Precondition holding: the given project is a project the user has
     * @param for_project the project for which the train user should choose the stations
     * @param n the number of stations the train user should choose. 0 if the user should choose the number of stations by itself.
     * (possibly until its funds are exhausted)
     * @return the route of stations
     */
    virtual std::vector<std::shared_ptr<AbstractStation>> choose_route(t_project for_project, std::shared_ptr<PeerCollection> from_collection, size_t n = 0) = 0;
    /**
     * Let the TrainUser choose a train class.
     * @param for_project
     * @param from_collection
     * @return
     */
    virtual trainClass choose_trainClass(t_project for_project, std::shared_ptr<PeerCollection> from_collection) = 0;
    /**
     * Report back execution result quality to the train user, allowing it to update its local trust values for the peers.
     * @param for_execution The execution which was done
     * @param result_quality the overall quality of the execution
     * @param result_peer An optional peer id. If this value is given, use this peer id as the id of the corrosponding virtual result peer
     */
    virtual void report_total_execution_result_quality(AbstractExecution &for_execution, t_quality result_quality,
                                                       std::optional<t_peer_id> result_peer = std::nullopt) = 0;
    /**
     * Report back interrim execution results qualities to the train user, allowing it to update its local trust values for the peers
     * @param for_execution The execution which was done
     * @param result_qualities The overall quality of the execution
     * @param result_peer An optional peer id. If this value is given, use this peer id as the id of the corrosponding virtual result peer
     * @note This method should be used mutual exclusive to the report_total_execution_result_quality method. They do not need to be implemented both,
     *  if the concrete train user is only used with a concrete execution calling only one of both methods.
     */
    virtual void report_interim_execution_result_quality(AbstractExecution &for_execution,
                                                         std::map<t_peer_id, t_quality> result_qualities,
                                                         std::optional<t_peer_id> result_peer = std::nullopt) = 0;
    PeerRole get_role() const override { return PeerRole::TrainUser; };
protected:
    std::vector<t_project> _projects;
    const t_token _initial_tokens;
    t_token _has_tokens = _initial_tokens;
};

/**
 * A Station provides datasets for specific topics
 */
class AbstractStation : public AbstractPeer {
public:
    AbstractStation (t_peer_id id, bool is_malicious, std::vector<t_topic_id> topics, t_token cost, unsigned int size_data_sample) :
        AbstractPeer{id, is_malicious}, _topics {std::move(topics)}, _cost{cost}, _size_data_sample {size_data_sample} {};
    [[nodiscard]] const std::vector<t_topic_id> & topics() const;
    [[nodiscard]] t_token get_cost() const;
    /**
     * Simulates a visit of a train at this station in the context of the given execution. Allows the station to adapt its local trust values.
     * @returns the quality (not reputation) of the result the station produces for a visit of this execution
     * For example, a malicious station may produce a low quality, depending on how the simulation is done.
     * Note that this violates the abstraction principle, since the station of course cannot decide how the
     * quality of its data for the execution is in a real world application.
     */
    virtual t_quality visit(AbstractExecution& in_execution) = 0;
    PeerRole get_role() const override { return PeerRole::Station; };
    t_token get_earned_tokens() const {return earned_tokens;};
    /// public for the omniscent result peer
    t_quality _get_visit_quality() const;
protected:
    std::vector<t_topic_id> _topics;
    t_token _cost;
    /** The tokens earned */
    t_token earned_tokens = 0;
    unsigned int _size_data_sample;
    /// helper function which returns a visit quality based upon the predefined size_data_sample attribute

};


/**
 * A TrainCreator provides trains.
 * Each train has a task.
 * Since trains are a function of other components and do not participate in the reputation calculation,
 * they are not represented here, rather, we work directly with the tasks
 */
class AbstractTrainCreator : public AbstractPeer {
public:
    AbstractTrainCreator (t_peer_id id, bool is_malicious, std::vector<trainClass> train_classes) :
            AbstractPeer {id, is_malicious}, _trainClasses {std::move(train_classes)} {};
    [[nodiscard]] const std::vector<trainClass> & trainClasses() const;
    PeerRole get_role() const override { return PeerRole::TrainCreator; };
protected:
    std::vector<trainClass> _trainClasses;
};

/**
 * An abstract virtual peer, calculating responsible for giving trust values as a function of the network.
 */
class AbstractVirtualPeer : public AbstractPeer {
public:
    AbstractVirtualPeer (t_peer_id id, std::shared_ptr<PeerCollection> with_context) : AbstractPeer {id, true}, _context{std::move(with_context)} {};
    PeerRole get_role() const override { return PeerRole::VirtualPeer; };
protected:
    std::shared_ptr<PeerCollection> _context;
};


class AbstractVirtualResultPeer : public AbstractVirtualPeer {
public:
    AbstractVirtualResultPeer  (t_peer_id id, std::shared_ptr<PeerCollection> with_context, std::vector<t_peer_id> station_route, t_peer_id creator_of_class) : AbstractVirtualPeer(id, with_context), _station_route{station_route}, _creator_of_class{creator_of_class} {};
protected:
    std::vector<t_peer_id> _station_route;
    t_peer_id _creator_of_class;
};
/**
 * Abstract factory for producing the different peers
 * Subclasses implement behavior, such as the topics interested, concrete peers with behavior...
 */
class AbstractPeerFactory {
public:
    virtual std::shared_ptr<AbstractStation> getBenignStation(t_peer_id id) = 0;
    virtual std::shared_ptr<AbstractStation> getMaliciousStation(t_peer_id id) = 0;
    virtual std::shared_ptr<AbstractTrainUser> getBenignTrainUser(t_peer_id id) = 0;
    virtual std::shared_ptr<AbstractTrainUser> getMaliciousTrainUser(t_peer_id id) = 0;
    virtual std::shared_ptr<AbstractTrainCreator> getBenignTrainCreator(t_peer_id id) = 0;
    virtual std::shared_ptr<AbstractTrainCreator> getMaliciousTrainCreator(t_peer_id id) = 0;
    virtual std::pair<std::shared_ptr<AbstractTrainUser>, std::shared_ptr<AbstractTrainCreator>> getBenignTrainUserCreator(t_peer_id) = 0;
private:
};

/**
 * Execution models an execution of a train at some stations with a train from some traincreator
 * Concrete implementations model how the quality of the execution is formed, before giving it to the train user
 */
class AbstractExecution {
public:
    /**
     * Create an execution
     * @param in_context collection of all peers
     * @param with_train_user The train user who does this execution
     * @param for_project The project of the train user for which the execution should be done
     */
    AbstractExecution(std::shared_ptr<PeerCollection> in_context, std::shared_ptr<AbstractTrainUser> with_train_user, t_project for_project);
    trainClass get_used_class() const;
    /**
     * This method is called when the execution should be "run", i.e.
     * - request train class from the train user
     * - request station selection from the train user
     * - collect qualities from the stations via their corrosponding method
     */
    virtual void run() = 0;
    /**
     * Indicates that the execution should employ a virtual result peer.
     *  Could also be implemented as a constructor parameters
     */
    void use_virtual_result_peer() {_use_virtual_result_peer = true;};
    const std::vector<std::shared_ptr<AbstractStation>>& get_route() const;
protected:
    std::shared_ptr<PeerCollection> _context;
    std::shared_ptr<AbstractTrainUser> _train_user;
    /**
     * class is chosen by train user
     */
    trainClass _used_class;
    /**
     * Project is defined by execution cycle
     */
    t_project _for_project;
    std::vector<std::shared_ptr<AbstractStation>> _route;
    bool _use_virtual_result_peer = false;
};
/**
 * A simulation cycle, choosing some peers and issuing a simulation request to those peers
 */
class AbstractExecutionCycle {
public:
    AbstractExecutionCycle(std::shared_ptr<PeerCollection> on_peer_collection) : _on_peer_collection {std::move(on_peer_collection)} {};
    /**
     * run one abstract simulation cycle
     */
    virtual void run() = 0;
protected:
    std::shared_ptr<PeerCollection> _on_peer_collection;
};

/**
 * Interface for a strategy to calculate the reputation.
 * calculate() is called at the end of each simulation cycle.
 */
class AbstractReputationCalculationStrategy {
public:
    /**
     * calculate the reputation of all peers
     */
    virtual void calculate(std::shared_ptr<PeerCollection> peer_collection) = 0;
    virtual t_reputation_vector get_pretrusted_vector(size_t num_of_peers) const = 0;
private:
};

#endif //SIMULATION_ABSTRACTS_H
