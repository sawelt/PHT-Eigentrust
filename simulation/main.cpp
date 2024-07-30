#include <iostream>
#include <fstream>

#define SIMULATION_QUIETER

#include "Peers.h"
#include "ConcreteSimpleExecution.h"
#include "ConcreteSimpleExecutionCycle.h"
#include "PeerFactory.h"
#include "CalculationStrategy.h"
#include "Simulation.h"
#include "SimulationReporter.h"
#include "NetworkAnalyser.h"
#include "JSONReporter.h"

struct GlobalConfiguration {
    bool use_baseline = false;
    bool add_malicious_collective = false;
    /// baseline malicious stations, not part of any collective etc.
    size_t num_malicious_stations = 5;
    size_t num_benign_stations = 30;
    size_t num_benign_train_users = 100;
    size_t num_benign_train_creators = 1;
    size_t num_malicious_train_users = 0;
    size_t num_malicious_train_creators = 0;
    // decides the number of stations in a route
    int budget_train_user_upper = 5;
    int budget_train_user_lower = 1;
    /// budget secondary is fixed, min=max
    int budget_secondary_users = 1;
    size_t num_benign_train_users_secondary = 0;
    int num_simulation_cycle = 30;
    int num_execution_cycle = 5;
    int num_of_execution_per_cycle_lower = 5;
    int num_of_execution_per_cycle_upper = 40;
    int num_of_topics = 3;
    int num_of_tasks = 1;
    // (1-a) * C + a*random_jump
    float a = 0.25;
    // (1-beta) * pretrusted + beta * multiplex
    float beta = 0.5;
    int num_pretrusted = 5;
    int num_simulations = 15;
};

void parse_cmd_configuration(int argc, char* argv[], GlobalConfiguration& config) {
    // TODO implement command line parameters
}

int main(int argc, char* argv[]) {
    GlobalConfiguration global_config;

    global_config.add_malicious_collective = true;
    global_config.num_malicious_stations = 0;

    parse_cmd_configuration(argc, argv, global_config);

    auto reporter = std::make_shared<JSONReporter>();
    SimulationReporter::set_simulation_reporter(reporter);

    for (int _i = 0; _i<global_config.num_simulations; ++_i) {


        std::shared_ptr<PeerFactory> pf;
        if (global_config.use_baseline) {
           pf = std::make_shared<PeerFactory>(global_config.num_of_topics,global_config.num_of_tasks);
        } else {
            pf = std::make_shared<FuzzyReputationPeerFactory>(global_config.num_of_topics,global_config.num_of_tasks);
        }
        pf->set_initial_budget(global_config.budget_train_user_lower, global_config.budget_train_user_upper);

        auto pc = std::make_shared<PeerCollection>();
        SimulationReporter::get_simulation_reporter()->set_collection(pc);
        pc->add_stations(*pf, false, global_config.num_benign_stations);
        pc->add_stations(*pf, true, global_config.num_malicious_stations);
        pc->add_train_users(*pf, false, global_config.num_benign_train_users);
        pc->add_train_creators(*pf, false, global_config.num_benign_train_creators);
        pc->add_train_user_creator(pf->getBenignTrainUserCreator(pc->get_valid_peer_id()));

        pf->set_initial_budget(global_config.budget_secondary_users, global_config.budget_secondary_users);
        pc->add_train_users(*pf, false, global_config.num_benign_train_users_secondary);



        auto ec = std::make_shared<ConcreteSimpleExecutionCycle> (pc, global_config.num_of_execution_per_cycle_lower, global_config.num_of_execution_per_cycle_upper, pf);
        auto cs = std::make_shared<MultiplexCalculationStrategy> (50,global_config.a,global_config.beta,0.05);

        // choose the pretrusted and initialise the multiplex values
        cs->choose_pretrusted(global_config.num_pretrusted, pc, true, true, false);

        if (global_config.add_malicious_collective) {
            // Add a malicious collective after choosing pre-trusted to prevent them from being part of the pre-trusted
            auto collective = pf->getMaliciousCollective(pc->get_valid_peer_ids(5));
            for (auto const& coll_p : collective) {
                pc->add_station(coll_p);
            }
        }


        auto v_p_peers = pc->get_all_train_users();
        std::vector<t_peer_id> train_user_ids;
        train_user_ids.resize(v_p_peers.size());
        std::transform(v_p_peers.begin(), v_p_peers.end(),
                       train_user_ids.begin(), [] (const std::shared_ptr<AbstractPeer>& p) -> t_peer_id {return p->get_id();});
        cs->choose_random_multiplex_trust_values(train_user_ids);
        reporter->report_multiplex_trust(cs->get_multiplex_trust_values());


        SimulationConfiguration config = {
                .cycle = ec,
                .collection = pc,
                .calculation_strategy = cs,
        };
        Simulation s(config);
        s.run(global_config.num_simulation_cycle, global_config.num_execution_cycle);
        NetworkAnalyser::sanity_check_simulated_peers(pc);
        std::cout << "average b station rep:" << NetworkAnalyser::get_median_global_trust_value_station(pc, false) << std::endl;
        std::cout << "average b train user rep:" << NetworkAnalyser::get_average_global_trust_value_train_user(pc, false, true)
                  << std::endl;
        std::cout << "average b train creator rep:" << NetworkAnalyser::get_median_global_trust_value_train_creator(pc, false) << std::endl;
        std::cout << "average m station rep:" << NetworkAnalyser::get_median_global_trust_value_station(pc, true, false) << std::endl;
        std::cout << "average m train user rep:" << NetworkAnalyser::get_average_global_trust_value_train_user(pc, true, false)
                  << std::endl;
        std::cout << "average m train creator rep:" << NetworkAnalyser::get_median_global_trust_value_train_creator(pc, true, false) << std::endl;
        NetworkAnalyser::print_earned_tokens(pc);


        /*std::cout << "malicious collective:" << std::endl;
        for (auto const& coll_p : collective) {
            std::cout << coll_p->global_trust_value << " earned: " << coll_p->get_earned_tokens() << " ";
        }*/

    }
    reporter->execution_did_end();
    std::ofstream _out("simulation.json");
    reporter->write_to(_out, false);
    _out.close();
    return 0;
}
