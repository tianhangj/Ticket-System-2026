#ifndef _ORDER_HPP_
#define _ORDER_HPP_

#include "data.hpp"
#include "command.hpp"
#include "STLite/priority_queue.hpp"

void query_ticket(DataManager& data_manager, const Command& cmd);
void query_transfer(DataManager& data_manager, const Command& cmd);
void buy_ticket(DataManager& data_manager, const Command& cmd);
void query_order(DataManager& data_manager, const Command& cmd);
void refund_ticket(DataManager& data_manager, const Command& cmd);

#endif