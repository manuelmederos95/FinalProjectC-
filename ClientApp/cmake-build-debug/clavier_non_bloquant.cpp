#include <thread>
#include <mutex>
#include <iostream>
#include <list>

#include "clavier_non_bloquant.h"

static std::thread* THREAD = nullptr;
static std::mutex MUTEX;
static std::list<char> LISTE_CAR;

void clavier_non_bloquant::thread_lire_clavier() {
    std::string str;
    while (true) {
        std::getline(std::cin, str);
        MUTEX.lock();
        for (auto i = str.begin(), i_max = str.end(); i != i_max; i++) {
            LISTE_CAR.push_back(*i);
        }
        MUTEX.unlock();
    }
}

void clavier_non_bloquant::lancer() {
    if (!THREAD) {
        LISTE_CAR.clear();
        THREAD = new std::thread(clavier_non_bloquant::thread_lire_clavier);
    }
}

void clavier_non_bloquant::arreter() {
    MUTEX.lock();
    if (THREAD) {
        delete THREAD;
        THREAD = nullptr;
    }
    MUTEX.unlock();
}

std::string& clavier_non_bloquant::lireLigne(std::string& str) {
    str.clear();
    MUTEX.lock();
    while (LISTE_CAR.size()) {
        str.push_back(LISTE_CAR.front());
        LISTE_CAR.pop_front();
    }
    MUTEX.unlock();
    return str;
}
