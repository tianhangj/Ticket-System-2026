#include "train.hpp"

void add_train(DataManager& data_manager, const Command& cmd) {
    TrainInfo train_info;
    train_info.trainid = cmd.gets('i');
    auto train_idxs = data_manager.train_idx.find(train_info.trainid);
    if (!train_idxs.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    train_info.station_num = cmd.geti('n');
    auto sale_date = cmd.getl('d');
    train_info.sale_date[0] = day(sale_date[0]);
    train_info.sale_date[1] = day(sale_date[1]);
    
    train_info.total_seat = cmd.geti('m');
    for (int i = train_info.sale_date[0]; i <= train_info.sale_date[1]; ++i) {
        for (int j = 0; j < train_info.station_num - 1; ++j) {
            train_info.seat_num[i][j] = train_info.total_seat;
        }
    }
    auto stations = cmd.getl('s');
    for (int i = 0; i < train_info.station_num; ++i) {
        train_info.stations[i] = stations[i];
    }
    auto prices = cmd.getli('p');
    for (int i = 0; i < train_info.station_num - 1; ++i) {
        train_info.prices[i] = prices[i];
    }
    auto start_time = cmd.gets('x');
    train_info.start_time = daytime(start_time);
    auto travel_times = cmd.getli('t');
    auto stopover_times = cmd.getli('o');
    for (int i = 0; i < train_info.station_num; ++i) {
        if (i == 0) {
            train_info.leave_times[i] = train_info.start_time;
        } else if (i == train_info.station_num - 1) {
            train_info.arrive_times[i] = train_info.leave_times[i-1] + travel_times[i-1];
        } else {
            train_info.arrive_times[i] = train_info.leave_times[i-1] + travel_times[i-1];
            train_info.leave_times[i] = train_info.arrive_times[i] + stopover_times[i-1];
        }
    }
    auto type = cmd.gets('y');
    train_info.type = type[0];
    train_info.released = false;
    int pos = data_manager.train_data.insert(train_info);
    data_manager.train_idx.insert(train_info.trainid, pos);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    return;
}

void delete_train(DataManager& data_manager, const Command& cmd) {
    auto train_idxs = data_manager.train_idx.find(cmd.gets('i'));
    if (train_idxs.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    TrainInfo train;
    data_manager.train_data.read(train, train_idxs[0]);
    if (train.released) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    data_manager.train_idx.erase(cmd.gets('i'), train_idxs[0]);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    return;
}

void release_train(DataManager& data_manager, const Command& cmd) {
    auto train_idxs = data_manager.train_idx.find(cmd.gets('i'));
    if (train_idxs.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    TrainInfo train;
    data_manager.train_data.read(train, train_idxs[0]);
    if (train.released) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    train.released = true;
    data_manager.train_data.update(train, train_idxs[0]);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    for (int i = 0; i < train.station_num - 1; ++i) {
        for (int j = i+1; j < train.station_num; ++j) {
            TicketInfo ticket_info;
            ticket_info.from = train.stations[i].hash();
            ticket_info.to = train.stations[j].hash();
            data_manager.ticket_idx.insert(ticket_info, train_idxs[0]);
        }
    }
    for (int i = 0; i < train.station_num; ++i) {
        data_manager.station_idx.insert(train.stations[i], train_idxs[0]);
    }
    return;
}

void query_train(DataManager& data_manager, const Command& cmd) {
    auto train_idxs = data_manager.train_idx.find(cmd.gets('i'));
    if (train_idxs.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    TrainInfo train;
    data_manager.train_data.read(train, train_idxs[0]);
    int d = day(cmd.gets('d'));
    if (train.sale_date[0] > d || train.sale_date[1] < d) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    std::cout << cmd.time_stamp << " " << train.trainid << " " << train.type << "\n";
    int cost = 0;
    for (int i = 0; i < train.station_num; ++i) {
        std::cout << train.stations[i] << " ";
        if (i == 0) {
            std::cout << "xx-xx xx:xx";
        } else {
            std::cout << timestr(train.arrive_times[i] + d * 24*60);
        }
        std::cout << " -> ";
        if (i == train.station_num - 1) {
            std::cout << "xx-xx xx:xx";
        } else {
            std::cout << timestr(train.leave_times[i] + d * 24*60);
        }
        std::cout << " " << cost << " ";
        if (i != train.station_num-1) {
            cost += train.prices[i];
        }
        if (i != train.station_num - 1) {
            std::cout << train.seat_num[d][i] << "\n";
        } else {
            std::cout << "x\n";
        }
    }
}