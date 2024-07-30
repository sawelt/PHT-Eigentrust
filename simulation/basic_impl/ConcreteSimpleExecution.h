//
// Created by laurenz on 27.06.23.
//

#ifndef SIMULATION_CONCRETESIMPLEEXECUTION_H
#define SIMULATION_CONCRETESIMPLEEXECUTION_H
#include "ConcreteSimpleExecution.fwd.h"
#include "ResultPeers.h"
#include "Abstracts.h"

class ConcreteSimpleExecution : public AbstractExecution {
public:
    using AbstractExecution::AbstractExecution;
    void run() override;
    void set_result_peer_type(ResultPeerType type) {_result_peer_type=type;};
private:
    ResultPeerType _result_peer_type = SelfWeighting;
};

#endif //SIMULATION_CONCRETESIMPLEEXECUTION_H
