
#include <string>

#include "data.hpp"
#include "command.hpp"
#include "user.hpp"
#include "train.hpp"
#include "order.hpp"



DataManager data_manager;

void exit() {
    data_manager.close();
}
void clear() {
    data_manager.clear();
}
int main() {
    data_manager.initailize();
    std::string line;
    while (getline(std::cin, line)) {
        Command cmd = Command::parse_command(line);
        // std::cerr << cmd.time_stamp << "\n";
        if (cmd.verb == "exit") {
            std::cout << cmd.time_stamp << " bye\n";
            exit();
        } else if (cmd.verb == "clear") {
            clear();
        } else if (cmd.verb == "add_user") {
            add_user(data_manager, cmd);
        } else if (cmd.verb == "login") {
            login(data_manager, cmd);
        } else if (cmd.verb == "logout") {
            logout(data_manager, cmd);
        } else if (cmd.verb == "query_profile") {
            query_profile(data_manager, cmd);
        } else if (cmd.verb == "modify_profile") {
            modify_profile(data_manager, cmd);
        } else if (cmd.verb == "add_train") {
            add_train(data_manager, cmd);
        } else if (cmd.verb == "delete_train") {
            delete_train(data_manager, cmd);
        } else if (cmd.verb == "release_train") {
            release_train(data_manager, cmd);
        } else if (cmd.verb == "query_train") {
            query_train(data_manager, cmd);
        } else if (cmd.verb == "query_ticket") {
            query_ticket(data_manager, cmd);
        } else if (cmd.verb == "query_transfer") {
            query_transfer(data_manager, cmd);
        } else if (cmd.verb == "buy_ticket") {
            buy_ticket(data_manager, cmd);
        } else if (cmd.verb == "query_order") {
            query_order(data_manager, cmd);
        } else if (cmd.verb == "refund_ticket") {
            refund_ticket(data_manager, cmd);
        }
    }
}
