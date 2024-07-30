//
// Created by Laurenz Neumann on 09.05.23.
//

#include "CalculationStrategy.h"

void print_peer_vector(const t_reputation_vector& t, std::shared_ptr<PeerCollection> peer_collection) {
    for (int i = 0; i<peer_collection->number_peers(); i++) {
        std::cout << t(i);
        auto peer = peer_collection->get_peer(i);
        switch (peer->get_role()) {
            case TrainUser:
                std::cout << "  (Train User)";
                std::cout << " (Topics: ";
                for (const auto p : std::dynamic_pointer_cast<AbstractTrainUser>(peer)->projects()) {
                    std::cout << std::get<project_topic>(p) << " ";
                }
                std::cout << ")" << std::endl;
                break;
            case TrainCreator:
                std::cout << "  (Train Creator)" << std::endl;
                break;
            case Station:
                std::cout << "  (Station)";
                std::cout << " (Topics: ";
                for (const auto t : std::dynamic_pointer_cast<AbstractStation>(peer)->topics()) {
                    std::cout << t << " ";
                }
                std::cout << ")" << std::endl;
                break;
            case VirtualPeer:
                std::cout << " (Virtual Peer)" << std::endl;
                break;
            case TrainUserCreator:
                std::cout << " (Train User / Creator)";
                for (const auto p : std::dynamic_pointer_cast<AbstractTrainUser>(peer)->projects()) {
                    std::cout << std::get<project_topic>(p) << " ";
                }
                std::cout << ")" << std::endl;
                break;
            case None:
                std::cout << "  (Other)" << std::endl;
                break;
        }
    }
}

void CalculationStrategy::calculate(std::shared_ptr<PeerCollection> peer_collection) {
    auto num_peers = peer_collection->number_peers();
    // matrix C
    t_reputation_matrix trust_matrix = t_reputation_matrix::Constant(num_peers, num_peers, 0);
    t_reputation_vector global_vec = t_reputation_vector::Constant(num_peers,1,0);
    // get the pretrusted vector
    const auto pretrusted_vector = get_pretrusted_vector(peer_collection->number_peers());
    // column which is given as default value for the normalised trust value matrix method
    t_reputation_column pretrusted_colum = pretrusted_vector.reshaped(1,num_peers);
    // for each peer, set the row of the trust matrix
    for (int i = 0; i<num_peers; i++) {
        const auto p = peer_collection->get_peer(i);
        trust_matrix.row(i) = p->get_normalised_trust_values_matrix(num_peers, pretrusted_colum);
        if (initial_calulation) {
            // in the first round the global trust values are initialised with the pretrusted values
            p->global_trust_value = pretrusted_vector(i);
        }

        global_vec(i) = p->global_trust_value;
    }
    //std::cout << "C:" << std::endl;
    //std::cout << trust_matrix << std::endl;
    //std::cout << "p:" << std::endl;
    //print_peer_vector(pretrusted_vector, peer_collection);
    // start calculating
    trust_matrix.transposeInPlace();
    for (int i=0; i<_max_calculation_rounds; i++) {
        auto old_vec = global_vec;
        global_vec= (1-_a)*(trust_matrix * global_vec) + (pretrusted_vector * _a);

        if ((global_vec-old_vec).stableNorm()<_cutoff_threshold_difference) {
            break;
        }
    }

    for (int i = 0; i<num_peers; i++) {
        peer_collection->get_peer(i)->global_trust_value = global_vec(i);
    }

    //std::cout << "New global vector t:" << std::endl;
    //print_peer_vector(global_vec, peer_collection);

    initial_calulation = false;
}

t_reputation_vector CalculationStrategy::get_pretrusted_vector(size_t num_of_peers) const {
    t_reputation_vector new_vec = t_reputation_vector::Constant(num_of_peers, 1, 0);
    for (const auto i : _pretrusted_peers) {
        new_vec(i) = 1.0/_pretrusted_peers.size();
    }
    return new_vec;

}

CalculationStrategy::CalculationStrategy(size_t max_calculation_rounds, float a, float cutoff_threshold_difference) :
    _max_calculation_rounds{max_calculation_rounds}, _a{a}, _cutoff_threshold_difference{cutoff_threshold_difference} {

}

void CalculationStrategy::choose_pretrusted(size_t n, std::shared_ptr<PeerCollection> from_collection, bool stations, bool users,
                                            bool creators, bool allow_malicious) {
    auto num_peers = from_collection->number_peers();
    if (n>num_peers) {
        throw std::invalid_argument("number pre trusted peers cannot exceed number peers");
    }
    // choose candidates
    std::vector<std::shared_ptr<AbstractPeer>> candidates;
    for (int i=0; i<num_peers; i++) {
        auto p = from_collection->get_peer(i);
        if (p->is_malicious() && !allow_malicious) {
            continue;
        }
        if (p->get_role()==PeerRole::Station && stations) {
            candidates.push_back(p);
        } else if (p->get_role()==PeerRole::TrainUser && users) {
            candidates.push_back(p);
        } else if (p->get_role()==PeerRole::TrainCreator && creators) {
            candidates.push_back(p);
        }
    }
    if (n>candidates.size()) {
        throw std::logic_error("n exceeds number of candidates");
    }
    // sample from candidates
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(candidates.begin(), candidates.end(), g);
    for (int i=0; i<n; i++) {
        auto p = candidates[i];
        _pretrusted_peers.push_back(p->get_id());
    }
    SimulationReporter::get_simulation_reporter()->report_pre_trusted(_pretrusted_peers);
    pretrusted_initialised = true;
}

MultiplexCalculationStrategy::MultiplexCalculationStrategy(size_t max_calculation_rounds, float a, float beta,
                                                           float cutoff_threshold_difference)
                                                           : CalculationStrategy(max_calculation_rounds, a, cutoff_threshold_difference), _beta{beta} {
}

t_reputation_vector MultiplexCalculationStrategy::get_multiplex_vector_p(size_t num_of_peers) const {
    t_reputation_vector new_vec = t_reputation_vector::Constant(num_of_peers, 1, 0);
    for (const auto i : _multiplex_trust_values) {
        new_vec(i.first) = i.second;
    }
    return new_vec;
}

t_reputation_vector MultiplexCalculationStrategy::get_pretrusted_vector(size_t num_of_peers) const {
    return (_beta * CalculationStrategy::get_pretrusted_vector(num_of_peers)) + ((1-_beta) * get_multiplex_vector_p(num_of_peers));
}

void MultiplexCalculationStrategy::set_multiplex_trust(std::map<t_peer_id, t_normalised_trust_value> trust_values) {
    _multiplex_trust_values = std::move(trust_values);
}

void MultiplexCalculationStrategy::choose_random_multiplex_trust_values(std::vector<t_peer_id> for_peers) {
    std::map<t_peer_id,unsigned int> probability_bucket;
    size_t sum = 0;

    std::random_device rd;
    auto gen = std::mt19937(rd());
    std::uniform_int_distribution id{1,25};
    // randomly assign each bucket a value between 1 and 25, then normalise this value as a fraction to get a somewhat random
    // probability vector
    // increasing the upper limit leads to higher differences between the multiplex trust values.
    for (const auto peer_id : for_peers) {
        probability_bucket[peer_id] = id(gen);
        sum += probability_bucket[peer_id];
    }


    std::map<t_peer_id,t_normalised_trust_value> _multiplex_trust;
    for (const auto peer_id : for_peers) {
        _multiplex_trust[peer_id] = (float)probability_bucket[peer_id]/sum;
    }
    set_multiplex_trust(_multiplex_trust);
}

std::map<t_peer_id, t_normalised_trust_value> MultiplexCalculationStrategy::get_multiplex_trust_values() const {
    return _multiplex_trust_values;
}
