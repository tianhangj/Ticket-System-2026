#ifndef _USER_HPP_
#define _USER_HPP_

#include "data.hpp"
#include "command.hpp"


void add_user(DataManager& data_manager, const Command& cmd);
void login(DataManager& data_manager, const Command& cmd);
void logout(DataManager& data_manager, const Command& cmd);
void query_profile(DataManager& data_manager, const Command& cmd);
void modify_profile(DataManager& data_manager, const Command& cmd);

#endif