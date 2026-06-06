#ifndef _DATA_HPP_
#define _DATA_HPP_

#include "string.hpp"
#include "bpt.hpp"
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
    String<20> trainID;
    int station_num;
    String<30> stations[100];
    int arrive_times[100], leave_times[100];
    int seat_num[100][99];
    int prices[99];
    int start_time;
    int sale_date[2];
    char type;
    bool released;
    bool operator < (const TrainInfo& other) const {
        return trainID < other.trainID;
    }
    bool operator == (const TrainInfo& other) const {
        return trainID == other.trainID;
    }
    bool operator != (const TrainInfo& other) const {
        return trainID != other.trainID;
    }
};

struct TicketInfo {
    String<30> from;
    String<30> to;
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
    BPT<String<20>, TrainInfo> train_data;
    BPT<TicketInfo, String<20>> ticket_data; // (from, to) -> trainID
    BPT<String<30>, String<20>> station_data; // station -> trainID
    BPT<String<20>, WaitInfo> wait_list;
    BPT<String<20>, OrderInfo> order_list;
    sjtu::map<String<20>, int> login_status; // username -> privilege
    void initailize() {
        user_data.initialize("user.db");
        train_data.initialize("train.db");
        ticket_data.initialize("ticket.db");
        station_data.initialize("station.db");
        wait_list.initialize("waitlist.db");
        order_list.initialize("order.db");
    }
    void close() {
        user_data.close();
        train_data.close();
        ticket_data.close();
        station_data.close();
        wait_list.close();
        order_list.close();
    }
    void clear() {
        user_data.clear();
        train_data.clear();
        ticket_data.clear();
        station_data.clear();
        wait_list.clear();
        order_list.clear();
    }
    DataManager(const DataManager& other) = delete;
    DataManager& operator=(const DataManager& other) = delete;
};

#endif