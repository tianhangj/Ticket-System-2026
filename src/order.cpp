#include "order.hpp"

void query_ticket(DataManager& data_manager, const Command& cmd) {
    auto from = cmd.gets('s');
    auto to = cmd.gets('t');
    bool sortkey = false; // false: time, true: cost
    if (cmd.has('p') && cmd.gets('p') == "cost") {
        sortkey = true;
    }

    TicketInfo ticket_info;
    ticket_info.from = String<30>(from).hash();
    ticket_info.to = String<30>(to).hash();
    auto train_idxs = data_manager.ticket_idx.find(ticket_info);
    
    int date = day(cmd.gets('d'));
    struct Ticket {
        int key;
        String<20> train_id;
        int leaving_time, arriving_time;
        int price;
        int seat_num;
        bool operator < (const Ticket& other) const {
            if (key != other.key) {
                return other.key < key;
            }
            return other.train_id < train_id;
        }
    };
    sjtu::priority_queue<Ticket> pq;
    Ticket ticket;
    TrainInfo train;
    for (const int& train_idx : train_idxs) {
        data_manager.train_data.read(train, train_idx);
        int f, t;
        for (int i = 0; i < train.station_num; ++i) {
            if (train.stations[i] == from) {
                f = i;
            }
            if (train.stations[i] == to) {
                t = i;
            }
        }
        int start_date = date - train.leave_times[f] / (24*60);
        if (train.sale_date[0] <= start_date && start_date <= train.sale_date[1]) {
            ticket.train_id = train.trainid;
            ticket.leaving_time = train.leave_times[f] + start_date * 24*60;
            ticket.arriving_time = train.arrive_times[t] + start_date * 24*60;
            ticket.seat_num = 1e9;
            ticket.price = 0;
            for (int i = f; i < t; ++i) {
                ticket.seat_num = std::min(ticket.seat_num, train.seat_num[start_date][i]);
                ticket.price += train.prices[i];
            }
            ticket.key = sortkey ? ticket.price : (ticket.arriving_time - ticket.leaving_time);
            pq.push(ticket);
        }
    }
    std::cout << cmd.time_stamp << " " << pq.size() << "\n";
    while (!pq.empty()) {
        auto f = pq.top();
        pq.pop();
        std::cout << f.train_id << " ";
        std::cout << from << " " << timestr(f.leaving_time) << " -> ";
        std::cout << to << " " << timestr(f.arriving_time) << " ";
        std::cout << f.price << " " << f.seat_num << std::endl; 
    }
}

void query_transfer(DataManager& data_manager, const Command& cmd) {
    auto from = cmd.gets('s');
    auto to = cmd.gets('t');
    int date = day(cmd.gets('d'));
    bool sortkey = false; // false: time, true: cost
    if (cmd.has('p') && cmd.gets('p') == "cost") {
        sortkey = true;
    }
    struct Ticket {
        String<20> trainid;
        String<30> from, to;
        int leaving_time, arriving_time;
        int price, seat;
    };
    sjtu::map<String<30>, sjtu::vector<Ticket>>first;
    auto train_idx1 = data_manager.station_idx.find(from);
    auto train_idx2 = data_manager.station_idx.find(to);
    TrainInfo train;
    for (const int &trainidx: train_idx1) {
        data_manager.train_data.read(train, trainidx);
        Ticket ticket;
        ticket.trainid = train.trainid;
        ticket.from = from;
        ticket.seat = 1e9;
        ticket.price = 0;
        bool flag = false;
        int start_date = 0;
        for (int i = 0; i < train.station_num; ++i) {
            if (train.stations[i] == from) {
                flag = true;
                start_date = date - train.leave_times[i] / (24*60);
                if (start_date < train.sale_date[0] || start_date > train.sale_date[1]) {
                    break;
                }
                ticket.leaving_time = start_date * 24*60 + train.leave_times[i];
                continue;
            }
            if (flag) {
                ticket.price += train.prices[i-1];
                ticket.seat = std::min(ticket.seat, train.seat_num[start_date][i-1]);
                ticket.to = train.stations[i];
                ticket.arriving_time = train.arrive_times[i] + start_date * 24*60;
                if (ticket.seat >= 1) {
                    first[ticket.to].push_back(ticket);
                } else {
                    break;
                }
            }
        }
    }
    struct CompareKey {
        int key1, key2;
        String<20> id1, id2;
        bool operator < (const CompareKey& rhs) const {
            if (key1 < rhs.key1) {
                return true;
            } else if (key1 == rhs.key1) {
                if (key2 < rhs.key2) {
                    return true;
                } else if (key2 == rhs.key2) {
                    if (id1 < rhs.id1) {
                        return true;
                    } else if (id1 == rhs.id1) {
                        return id2 < rhs.id2;
                    }
                }
            }
            return false;
        }
    } minimum;
    minimum.key1 = minimum.key2 = 1e9;
    Ticket A, B;
    for (const auto &trainidx: train_idx2) {
        data_manager.train_data.read(train, trainidx);
        int to_station = 0;
        for (int i = 0; i < train.station_num; ++i) {
            if (train.stations[i] == to) {
                to_station = i;
                break;
            }
        }
        for (int i = 0; i < to_station; ++i) {
            if (first.count(train.stations[i])) {
                for (const auto &fir: first[train.stations[i]]) {
                    if (fir.trainid == train.trainid) {
                        continue;
                    }
                    for (int d = train.sale_date[0]; d <= train.sale_date[1]; ++d) {
                        if (d * 24*60 + train.leave_times[i] < fir.arriving_time) {
                            continue;
                        }
                        int remain = 1e9;
                        int price = 0;
                        for (int j = i; j < to_station; ++j) {
                            remain = std::min(remain, train.seat_num[d][j]);
                            price += train.prices[j];
                            if (remain == 0) {
                                break;
                            }
                        }
                        if (remain >= 1) {
                            CompareKey key;
                            key.key1 = train.arrive_times[to_station] + d * 24*60 - fir.leaving_time;
                            key.key2 = price + fir.price;
                            if (sortkey) {
                                std::swap(key.key1, key.key2);
                            }
                            key.id1 = fir.trainid;
                            key.id2 = train.trainid;
                            if (key < minimum) {
                                minimum = key;
                                A = fir;
                                B.trainid = train.trainid;
                                B.arriving_time = train.arrive_times[to_station] + d * 24*60;
                                B.leaving_time = train.leave_times[i] + d * 24*60;
                                B.from = train.stations[i];
                                B.to = to;
                                B.price = price;
                                B.seat = remain;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    if (minimum.key1 == 1e9) {
        std::cout << cmd.time_stamp << " 0\n";
    } else {
        std::cout << cmd.time_stamp << " ";
        std::cout << A.trainid << " ";
        std::cout << A.from << " " << timestr(A.leaving_time) << " -> ";
        std::cout << A.to << " " << timestr(A.arriving_time) << " ";
        std::cout << A.price << " " << A.seat << std::endl;

        std::cout << B.trainid << " ";
        std::cout << B.from << " " << timestr(B.leaving_time) << " -> ";
        std::cout << B.to << " " << timestr(B.arriving_time) << " ";
        std::cout << B.price << " " << B.seat << std::endl;
    }
}

void buy_ticket(DataManager& data_manager, const Command& cmd) {
    auto user = cmd.gets('u');
    if (!data_manager.login_status.count(user)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto trainid = cmd.gets('i');
    int date = day(cmd.gets('d'));
    auto from = cmd.gets('f');
    auto to = cmd.gets('t');
    int n = cmd.geti('n');
    auto train_idxs = data_manager.train_idx.find(trainid);
    if (train_idxs.empty()) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    TrainInfo train;
    data_manager.train_data.read(train, train_idxs[0]);
    if (!train.released || n > train.total_seat) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    int f = -1, t = -1;
    int price = 0, seat = 1e9;
    for (int i = 0; i < train.station_num; ++i) {
        if (train.stations[i] == from) {
            f = i;
            date -= train.leave_times[i] / (24*60);
            if (date > train.sale_date[1] || date < train.sale_date[0]) {
                std::cout << cmd.time_stamp << " -1\n";
                return;
            }
            continue;
        }
        if (f != -1) {
            price += train.prices[i-1];
            seat = std::min(seat, train.seat_num[date][i-1]);
        }
        if (train.stations[i] == to) {
            t = i;
            break;
        }
    }
    if (f == -1 || t == -1 || t < f) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    OrderInfo orderinfo;
    orderinfo.time = cmd.timestamp();
    orderinfo.date = date;
    orderinfo.trainid = trainid;
    orderinfo.leaving_time = date * 24*60 + train.leave_times[f];
    orderinfo.arriving_time = date * 24*60 + train.arrive_times[t];
    orderinfo.from = from;
    orderinfo.f = f;
    orderinfo.to = to;
    orderinfo.t = t;
    orderinfo.price = price;
    orderinfo.num = n;
    if (n <= seat) {
        for (int i = f; i < t; ++i) {
            train.seat_num[date][i] -= n;
        }
        data_manager.train_data.update(train, train_idxs[0]);
        orderinfo.status = 0;
        data_manager.order_list.insert(user, orderinfo);
        std::cout << cmd.time_stamp << " " << n * orderinfo.price << "\n";
    } else {
        if (cmd.has('q') && cmd.gets('q') == "true") {
            WaitInfo wait;
            wait.time = cmd.timestamp();
            wait.trainid = trainid;
            wait.date = date;
            wait.from = from;
            wait.f = f;
            wait.to = to;
            wait.t = t;
            wait.num = n;
            wait.user = user;
            data_manager.wait_list.insert(trainid, wait);
            orderinfo.status = 1;
            data_manager.order_list.insert(user, orderinfo);
            std::cout << cmd.time_stamp << " queue\n";
        } else {
            std::cout << cmd.time_stamp << " -1\n";
        }
    }
}
const char * status[3] = {"[success]", "[pending]", "[refunded]"};
void query_order(DataManager& data_manager, const Command& cmd) {
    auto user = cmd.gets('u');
    if (!data_manager.login_status.count(user)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto orders = data_manager.order_list.find(user);
    std::cout << cmd.time_stamp << " " << orders.size() << "\n";
    for (const auto& order: orders) {
        std::cout << status[order.status] << " " << order.trainid << " ";
        std::cout << order.from << " " << timestr(order.leaving_time) << " -> ";
        std::cout << order.to << " " << timestr(order.arriving_time) << " ";
        std::cout << order.price << " " << order.num << "\n";
    }
}

void refund_ticket(DataManager& data_manager, const Command& cmd) {
    auto user = cmd.gets('u');
    if (!data_manager.login_status.count(user)) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    int n = cmd.has('n') ? cmd.geti('n') : 1;
    auto orders = data_manager.order_list.find(user);
    if (orders.size() < n) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    auto order = orders[n-1];
    if (order.status == 2) {
        std::cout << cmd.time_stamp << " -1\n";
        return;
    }
    if (order.status == 1) {
        WaitInfo wait;
        wait.time = order.time;
        data_manager.wait_list.erase(order.trainid, wait);
        data_manager.order_list.erase(user, order);
        order.status = 2;
        data_manager.order_list.insert(user, order);
        std::cout << cmd.time_stamp << " 0\n";
        return;
    }
    data_manager.order_list.erase(user, order);
    order.status = 2;
    data_manager.order_list.insert(user, order);
    int train_idx = data_manager.train_idx.find(order.trainid)[0];
    TrainInfo train;
    data_manager.train_data.read(train, train_idx);
    auto queue = data_manager.wait_list.find(train.trainid);
    for (int i = order.f; i < order.t; ++i) {
        train.seat_num[order.date][i] += order.num;
    }
    for (const auto& wait: queue) {
        if (wait.date != order.date) {
            continue;
        }
        bool flag = true;
        for (int i = wait.f; i < wait.t; ++i) {
            if (train.seat_num[wait.date][i] < wait.num) {
                flag = false;
                break;
            }
        }
        if (flag) {
            data_manager.wait_list.erase(train.trainid, wait);
            for (int i = wait.f; i < wait.t; ++i) {
                train.seat_num[wait.date][i] -= wait.num;
            }
            auto orders = data_manager.order_list.find(wait.user);
            for (auto& corder: orders) {
                if (corder.time == wait.time) {
                    data_manager.order_list.erase(wait.user, corder);
                    corder.status = 0;
                    data_manager.order_list.insert(wait.user, corder);
                    break;
                }
            }
        }
    }
    data_manager.train_data.update(train, train_idx);
    std::cout << cmd.time_stamp << " 0\n";
}