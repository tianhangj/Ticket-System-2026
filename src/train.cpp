#include "train.hpp"

void add_train(DataManager& data_manager, const Command& cmd) {
    TrainInfo train_info;
    train_info.trainID = cmd.gets('i');
    auto trains = data_manager.train_data.find(train_info.trainID);
    if (!trains.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    train_info.station_num = cmd.geti('n');
    auto sale_date = cmd.getl('d');
    train_info.sale_date[0] = day(sale_date[0]);
    train_info.sale_date[1] = day(sale_date[1]);
    int seat_num = cmd.geti('m');
    for (int i = train_info.sale_date[0]; i <= train_info.sale_date[1]; ++i) {
        for (int j = 0; j < train_info.station_num - 1; ++j) {
            train_info.seat_num[i][j] = seat_num;
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
    data_manager.train_data.insert(train_info.trainID, train_info);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    return;
}

void delete_train(DataManager& data_manager, const Command& cmd) {
    auto trains = data_manager.train_data.find(cmd.gets('i'));
    if (trains.empty() || trains[0].released) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    data_manager.train_data.erase(cmd.gets('i'), trains[0]);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    return;
}

void release_train(DataManager& data_manager, const Command& cmd) {
    auto trains = data_manager.train_data.find(cmd.gets('i'));
    if (trains.empty() || trains[0].released) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    data_manager.train_data.erase(cmd.gets('i'), trains[0]);
    trains[0].released = true;
    data_manager.train_data.insert(cmd.gets('i'), trains[0]);
    std::cout << cmd.time_stamp << " 0" << std::endl;
    for (int i = 0; i < trains[0].station_num - 1; ++i) {
        for (int j = i+1; j < trains[0].station_num; ++j) {
            TicketInfo ticket_info;
            ticket_info.from = trains[0].stations[i];
            ticket_info.to = trains[0].stations[j];
            data_manager.ticket_data.insert(ticket_info, trains[0].trainID);
        }
        data_manager.station_data.insert(trains[0].stations[i], trains[0].trainID);
    }
    return;
}

void query_train(DataManager& data_manager, const Command& cmd) {
    auto trains = data_manager.train_data.find(cmd.gets('i'));
    if (trains.empty()) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    int d = day(cmd.gets('d'));
    if (trains[0].sale_date[0] > d || trains[0].sale_date[1] < d) {
        std::cout << cmd.time_stamp << " -1" << std::endl;
        return;
    }
    const auto& train_info = trains[0];
    std::cout << cmd.time_stamp << " " << train_info.trainID << " " << train_info.type << "\n";
    int cost = 0;
    for (int i = 0; i < train_info.station_num; ++i) {
        std::cout << train_info.stations[i] << " ";
        if (i == 0) {
            std::cout << "xx-xx xx:xx";
        } else {
            std::cout << timestr(train_info.arrive_times[i] + d * 24*60);
        }
        std::cout << " -> ";
        if (i == train_info.station_num - 1) {
            std::cout << "xx-xx xx:xx";
        } else {
            std::cout << timestr(train_info.leave_times[i] + d * 24*60);
        }
        std::cout << " " << cost << " ";
        if (i != train_info.station_num-1) {
            cost += train_info.prices[i];
        }
        if (i != train_info.station_num - 1) {
            std::cout << train_info.seat_num[d][i] << "\n";
        } else {
            std::cout << "x\n";
        }
    }
}