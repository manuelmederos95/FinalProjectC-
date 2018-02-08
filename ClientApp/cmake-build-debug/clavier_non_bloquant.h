#pragma once

#include <string>

#ifndef CLAVIER_NON_BLOQUANT_H
#define CLAVIER_NON_BLOQUANT_H

class clavier_non_bloquant {
private:
    static void thread_lire_clavier();
public:
    static void lancer();
    static void arreter();
    static std::string& lireLigne(std::string&);
};

#endif /* CLAVIER_NON_BLOQUANT_H */
