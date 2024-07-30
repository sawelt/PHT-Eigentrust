//
// Created by laurenz on 27.06.23.
//

#include "ConcreteSimpleExecution.h"

void ConcreteSimpleExecution::run() {
    _used_class = _train_user->choose_trainClass(_for_project, _context);
    // we only want to do one-station executions.
    _route = _train_user->choose_route(_for_project, _context);
    const auto n = _route.size();
    std::optional<t_peer_id> virtual_result_peer_id = std::nullopt;
    t_quality overall_quality = 0;
    if (!_route.empty()) {
        for (const auto s : _route ) {
            auto quality = s->visit(*this);
            // taking a mean of the quality
            overall_quality += quality/n;
        }
        if (_use_virtual_result_peer) {
            std::vector<t_peer_id> v_id_route;
            for (const auto & s : _route) {
                v_id_route.push_back(s->get_id());
            }
            virtual_result_peer_id = _context->get_valid_peer_id();
            auto vp = construct_result_peer(_result_peer_type, virtual_result_peer_id.value(), _context, v_id_route, _used_class.creator);
            _context->add_virtual_peer(vp);
            SimulationReporter::get_simulation_reporter()->report_peer({
                                                                               .id = virtual_result_peer_id.value(),
                                                                               .role = VirtualPeer,
                                                                               .peer_parameters = {{"type", "self_weighting"}},
                                                                               .initial_budget = 0,
                                                                       });
        }
        _train_user->report_total_execution_result_quality(*this, overall_quality, virtual_result_peer_id);
    }
    std::vector<t_peer_id> _route_ids;
    for (const auto & s : _route) {_route_ids.push_back(s->get_id());};
    SimulationReporter::get_simulation_reporter()->execution_did_run({
                                                                             .train_user=_train_user->get_id(),
                                                                             .route = _route_ids,
                                                                             .execution_quality = overall_quality,
                                                                             .virtual_peer = virtual_result_peer_id
                                                                     });
    _train_user->reset_tokens();

}