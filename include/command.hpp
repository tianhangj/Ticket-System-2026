#ifndef _COMMAND_HPP_
#define _COMMAND_HPP_

#include "string"
#include "STLite/map.hpp"
#include "utils.hpp"

struct Command {
    std::string time_stamp;
    std::string verb;
    sjtu::map<char, std::string> args;
    static Command parse_command(const std::string& input) {
        Command cmd;
        sjtu::vector<std::string>tokens = split(input, ' ');
        cmd.time_stamp = tokens[0];
        cmd.verb = tokens[1];
        for (size_t i = 2; i < tokens.size(); i += 2) {
            cmd.args[tokens[i][1]] = tokens[i+1];
        }
        return cmd;
    }
    bool has(char arg) const {
        return args.count(arg);
    }
    int geti(char arg) const {
        assert(args.count(arg));
        int result = 0;
        sscanf(args[arg].c_str(), "%d", &result);
        return result;
    }
    std::string gets(char arg) const {
        assert(args.count(arg));
        return args[arg];
    }
    sjtu::vector<std::string> getl(char arg) const {
        assert(args.count(arg));
        return split(args[arg], '|');
    }
    sjtu::vector<int> getli(char arg) const {
        assert(args.count(arg));
        sjtu::vector<std::string> tokens = split(args[arg], '|');
        sjtu::vector<int> result;
        for (size_t i = 0; i < tokens.size(); ++i) {
            int x;
            sscanf(tokens[i].c_str(), "%d", &x);
            result.push_back(x);
        }
        return result;
    }
};

#endif