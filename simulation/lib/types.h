//
// Created by Laurenz Neumann on 26.04.23.
//

#ifndef SIMULATION_TYPES_H
#define SIMULATION_TYPES_H
#include <tuple>
#include <Eigen/Eigen>

typedef unsigned int t_peer_id;
typedef unsigned int t_topic_id;
typedef unsigned int t_task_id;
typedef double t_trust_value;
typedef double t_normalised_trust_value;
/**
 * value to measure quality of e.g. result data at a station. Should be in [0,1]
 */
typedef float t_quality;
typedef unsigned int t_token;
/**
 * A project is a project a train user wants to do. It is a tuple consisting of a topic id, identifying the topic of the project
 * and a task id consisting of identyifing the type of task the user want to do.
 */
typedef std::tuple<t_topic_id, t_task_id> t_project;
constexpr char project_topic = 0;
constexpr char project_task = 1;
/**
 * A trainClass type consist of a creator id, identyfing the train creator who created that train class,
 * a quality attribute, simulating its (absolut, not perceived) quality and
 * a task identifier indicating for what kind of task this train can be used.
 */
struct trainClass {
    t_peer_id creator;
    t_quality quality;
    t_task_id for_task;
};
typedef Eigen::Matrix<t_normalised_trust_value, Eigen::Dynamic, Eigen::Dynamic> t_reputation_matrix;
typedef  Eigen::Matrix<t_normalised_trust_value, 1, Eigen::Dynamic> t_reputation_column;
typedef  Eigen::Matrix<t_normalised_trust_value, Eigen::Dynamic, 1> t_reputation_vector;

enum PeerRole {
    TrainUser,
    Station,
    TrainCreator,
    VirtualPeer,
    TrainUserCreator,
    None
};

#endif //SIMULATION_TYPES_H
