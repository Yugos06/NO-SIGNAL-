#pragma once

#include <random>
#include <string>
#include <vector>

struct Relay {
    std::string name;
    int stability = 0;
    int interference = 0;
    bool online = false;
};

class Game {
public:
    Game();
    void run();

private:
    bool running_ = true;
    int turn_ = 1;
    int maxTurns_ = 20;
    int energy_ = 75;
    int spareParts_ = 4;
    int batteryPacks_ = 3;
    int hubCharge_ = 30;
    int currentRelay_ = 0;
    std::vector<Relay> relays_;
    std::mt19937 rng_;

    void printIntro() const;
    void printStatus() const;
    void printActions() const;
    int readAction() const;
    int readRelayIndex() const;

    void handleAction(int action);
    void scanNetwork() const;
    void travelToRelay();
    void repairRelay();
    void boostHub();
    void rest();
    void randomEvent();

    int computeSignal() const;
    bool checkEndConditions();
};
