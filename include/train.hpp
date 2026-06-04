#ifndef _TRAIN_HPP_
#define _TRAIN_HPP_

#include "data.hpp"
#include "command.hpp"

void add_train(DataManager& data_manager, const Command& cmd);
void delete_train(DataManager& data_manager, const Command& cmd);
void release_train(DataManager& data_manager, const Command& cmd);
void query_train(DataManager& data_manager, const Command& cmd);

#endif