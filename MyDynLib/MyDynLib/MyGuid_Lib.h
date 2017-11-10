#pragma once
#include "Guid.hpp"

std::string guidToDB(xg::Guid guid) {
    std::string g = guid.str();
    std::string s = "X'";
    for (std::string::iterator it = g.begin(); it != g.end(); ++it) {
        if ((*it) != '-') {
            s += (*it);
        }
    }
    s += "'";
    return s;
}

xg::Guid dbToGuid(char *str) {
    std::string s = "";
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == 'X' | str[i] == '\'') {
            ;
        }
        else {
            s += str[i];
        }
    }
    xg::Guid g = s;
    return g;
}