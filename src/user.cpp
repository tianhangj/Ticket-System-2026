#include "user.hpp"

void add_user(DataManager& data_manager, const Command& cmd) {
    UserInfo user_info;
    user_info.username = cmd.gets('u');
    user_info.password = cmd.gets('p');
    user_info.name = cmd.gets('n');
    user_info.mail = cmd.gets('m');
    if (data_manager.user_data.empty()) {
        user_info.privilege = 10;
        data_manager.user_data.insert(user_info.username, user_info);
        std::cout << cmd.time_stamp << " 0\n";
    } else {
        user_info.privilege = cmd.geti('g');
        if (data_manager.login_status.count(cmd.gets('c'))
            && data_manager.login_status[cmd.gets('c')] > user_info.privilege) {
            if (data_manager.user_data.find(user_info.username).size()) {
                std::cout << cmd.time_stamp << " -1\n";
                return;
            }
            data_manager.user_data.insert(user_info.username, user_info);
            std::cout << cmd.time_stamp << " 0\n";
        } else {
            std::cout << cmd.time_stamp << " -1\n";
        }
    }
}

void login(DataManager& data_manager, const Command& cmd) {
    std::string username = cmd.gets('u');
    std::string password = cmd.gets('p');
    if (data_manager.login_status.count(username)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto users = data_manager.user_data.find(username);
    if (users.size() == 0) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    if (users[0].password == password) {
        data_manager.login_status[username] = users[0].privilege;
        std::cout << cmd.time_stamp << " 0\n";
    } else {
        std::cout << cmd.time_stamp << " -1\n";
    }
}

void logout(DataManager& data_manager, const Command& cmd) {
    std::string username = cmd.gets('u');
    if (data_manager.login_status.count(username)) {
        data_manager.login_status.erase(data_manager.login_status.find(username));
        std::cout << cmd.time_stamp << " 0\n";
    } else {
        std::cout << cmd.time_stamp << " -1\n";
    }
}

void query_profile(DataManager& data_manager, const Command& cmd) {
    std::string cur_user = cmd.gets('c');
    std::string username = cmd.gets('u');
    if (!data_manager.login_status.count(cur_user)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto users = data_manager.user_data.find(username);
    if (users.size() == 0) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    if (cur_user == username || data_manager.login_status[cur_user] > users[0].privilege) {
        std::cout << cmd.time_stamp << " " << users[0].username << " "
                  << users[0].name << " " << users[0].mail << " "
                  << users[0].privilege << "\n";
    } else {
        std::cout << cmd.time_stamp << " -1\n";
    }
}

void modify_profile(DataManager& data_manager, const Command& cmd) {
    std::string cur_user = cmd.gets('c');
    std::string username = cmd.gets('u');
    if (!data_manager.login_status.count(cur_user)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto users = data_manager.user_data.find(username);
    if (users.size() == 0) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    UserInfo user_info = users[0];
    if (cur_user == username || data_manager.login_status[cur_user] > users[0].privilege) {
        UserInfo user_info = users[0];
        if (cmd.has('p')) {
            user_info.password = cmd.gets('p');
        }
        if (cmd.has('n')) {
            user_info.name = cmd.gets('n');
        }
        if (cmd.has('m')) {
            user_info.mail = cmd.gets('m');
        }
        if (cmd.has('g')) {
            if (data_manager.login_status[cur_user] <= cmd.geti('g')) {
                std::cout << cmd.time_stamp << " -1\n";
                return;
            }
            if (data_manager.login_status.count(username)) {
                data_manager.login_status[username] = cmd.geti('g');
            }
            user_info.privilege = cmd.geti('g');
        }
        data_manager.user_data.erase(username, users[0]);
        data_manager.user_data.insert(username, user_info);
        std::cout << cmd.time_stamp << " " << user_info.username << " "
                  << user_info.name << " " << user_info.mail << " "
                  << user_info.privilege << "\n";
    } else {
        std::cout << cmd.time_stamp << " -1\n";
    }
}

