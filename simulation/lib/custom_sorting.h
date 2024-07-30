//
// Created by Laurenz Neumann on 02.05.23.
//

#ifndef SIMULATION_CUSTOM_SORTING_H
#define SIMULATION_CUSTOM_SORTING_H
#include "Abstracts.h"
// Reminder sorting method: True if first param should be before second param in a vector, false otherwise

struct station_pointer_sorting_trust {
    inline bool operator() (const std::shared_ptr<AbstractStation> &a, const std::shared_ptr<AbstractStation> &b) {
        return a->global_trust_value > b->global_trust_value;
    }
};

struct station_pointer_sorting_price {
    inline bool operator() (const std::shared_ptr<AbstractStation>& a, const std::shared_ptr<AbstractStation>& b) {
        return a->get_cost() < b->get_cost();
    }
};
#endif //SIMULATION_CUSTOM_SORTING_H
