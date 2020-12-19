//
// Created by CurtisChen on 2018/12/3.
//

#include "view.h"

bool view::Create(string name, vector<string> message) {
    this->name = name;
    (this->message).clear();

    for (int i = 0; i < message.size(); i++) {
        (this->message).push_back(message[i]);
    }

    return true;
}


view::view(string name, vector<string> message) {
    this->name = name;
    (this->message).clear();

    for (int i = 0; i < message.size(); i++) {
        (this->message).push_back(message[i]);
    }
}


bool view::write() {
    return false;
}

bool view::del() {
    return false;
}