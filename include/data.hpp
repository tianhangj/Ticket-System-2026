#ifndef _DATA_HPP_
#define _DATA_HPP_

#include "bpt.hpp"
#include "string.hpp"
#include "memoryriver.hpp"
#include "STLite/map.hpp"

struct UserInfo {
    String<20> username;
    String<30> password;
    String<15> name;
    String<30> mail;
    int privilege;
    bool operator < (const UserInfo& other) const {
        return username < other.username;
    }
    bool operator == (const UserInfo& other) const {
        return username == other.username;
    }
    bool operator != (const UserInfo& other) const {
        return username != other.username;
    }
};

struct TrainInfo {
    String<20> trainid;
    int station_num;
    String<30> stations[35];
    int arrive_times[100], leave_times[100];
    int seat_num[100][35];
    int total_seat;
    int prices[99];
    int start_time;
    int sale_date[2];
    char type;
    bool released;
    bool operator < (const TrainInfo& other) const {
        return trainid < other.trainid;
    }
    bool operator == (const TrainInfo& other) const {
        return trainid == other.trainid;
    }
    bool operator != (const TrainInfo& other) const {
        return trainid != other.trainid;
    }
};

struct TicketInfo {
    size_t from, to; // hash
    bool operator < (const TicketInfo& other) const {
        if (from == other.from) {
            return to < other.to;
        }
        return from < other.from;
    }
    bool operator == (const TicketInfo& other) const {
        return from == other.from && to == other.to;
    }
    bool operator != (const TicketInfo& other) const {
        return from != other.from || to != other.to;
    }
};

struct WaitInfo {
    int time;
    String<20> user;
    String<20> trainid;
    int date; // 从起点站发车的日期
    String<30> from, to;
    int f, t; // index of from, to
    int num;
    bool operator < (const WaitInfo &rhs) const {
        return time < rhs.time;
    }
    bool operator == (const WaitInfo &rhs) const {
        return time == rhs.time;
    }
    bool operator != (const WaitInfo &rhs) const {
        return time != rhs.time;
    }
};

struct OrderInfo {
    int time;
    int status; // 0: success, 1: pending, 2: refuneded
    String<20> trainid;
    String<30> from, to;
    int f, t; // index of from, to
    int date; // 从起点站发车的日期
    int leaving_time, arriving_time;
    int price;
    int num;
    bool operator < (const OrderInfo &rhs) const {
        return time > rhs.time;
    }
    bool operator == (const OrderInfo &rhs) const {
        return time == rhs.time;
    }
    bool operator != (const OrderInfo &rhs) const {
        return time != rhs.time;
    }
};

class DataManager {
public:
    DataManager() {
    }
    BPT<String<20>, UserInfo> user_data;
    MemoryRiver<TrainInfo> train_data;
    BPT<String<20>, int> train_idx;
    BPT<TicketInfo, int> ticket_idx; // (from, to) -> train_idx
    BPT<String<30>, int> station_idx; // station -> train_idx
    BPT<String<20>, WaitInfo> wait_list;
    BPT<String<20>, OrderInfo> order_list;
    sjtu::map<String<20>, int> login_status; // username -> privilege
    void initailize() {
        user_data.initialize("user.db");
        train_data.initialize("train.dat");
        train_idx.initialize("train.db");
        ticket_idx.initialize("ticket.db");
        station_idx.initialize("station.db");
        wait_list.initialize("waitlist.db");
        order_list.initialize("order.db");
    }
    void close() {
        user_data.close();
        train_data.close();
        ticket_idx.close();
        station_idx.close();
        wait_list.close();
        order_list.close();
    }
    void clear() {
        user_data.clear();
        train_data.clear();
        train_idx.clear();
        ticket_idx.clear();
        station_idx.clear();
        wait_list.clear();
        order_list.clear();
    }
    DataManager(const DataManager& other) = delete;
    DataManager& operator=(const DataManager& other) = delete;
};

#endif