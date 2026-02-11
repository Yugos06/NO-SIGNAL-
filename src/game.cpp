#include "game.hpp"

#include <algorithm>
#include <iostream>
#include <string>

namespace {

int clamp100(int value) {
    return std::clamp(value, 0, 100);
}

std::string relayState(const Relay& relay) {
    return relay.online ? "ONLINE" : "OFFLINE";
}

} 

Game::Game() : rng_(std::random_device{}()) {
    relays_ = {
        {"North Tower", 50, 12, true},
        {"East Tower", 40, 20, true},
        {"South Tower", 35, 22, false},
        {"West Tower", 30, 28, false},
    };
}

void Game::run() {
    printIntro();

    while (running_) {
        printStatus();
        if (checkEndConditions()) {
            break;
        }

        printActions();
        const int action = readAction();
        handleAction(action);

        if (!running_) {
            break;
        }

        if (checkEndConditions()) {
            break;
        }

        randomEvent();
        ++turn_;
    }
}

void Game::printIntro() const {
    std::cout << "=== NO SIGNAL :: V0.1 (terminal prototype) ===\n";
    std::cout << "You are the last field engineer.\n";
    std::cout << "Restore the network before turn " << maxTurns_ << ".\n";
    std::cout << "Win condition: overall signal >= 85 and at least 3 relays online.\n\n";
}

void Game::printStatus() const {
    std::cout << "\n--- TURN " << turn_ << "/" << maxTurns_ << " ---\n";
    std::cout << "Energy: " << energy_
              << " | Spare Parts: " << spareParts_
              << " | Battery Packs: " << batteryPacks_
              << " | Hub Charge: " << hubCharge_ << "\n";

    const Relay& relay = relays_[currentRelay_];
    std::cout << "Current Relay: " << relay.name
              << " (" << relayState(relay) << ")\n";
    std::cout << "Global Signal: " << computeSignal() << "/100\n";
}

void Game::printActions() const {
    std::cout << "\nActions:\n";
    std::cout << "  1) Scan network\n";
    std::cout << "  2) Travel to relay (-5 energy)\n";
    std::cout << "  3) Repair current relay (-12 energy, -1 spare part)\n";
    std::cout << "  4) Boost central hub (-10 energy, -1 battery pack)\n";
    std::cout << "  5) Rest (+energy, but hub drains)\n";
    std::cout << "  6) Quit\n";
    std::cout << "Select action: ";
}

int Game::readAction() const {
    int action = 0;
    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 6;
        }
        try {
            action = std::stoi(line);
        } catch (...) {
            action = 0;
        }
        if (action >= 1 && action <= 6) {
            return action;
        }
        std::cout << "Invalid action. Enter 1-6: ";
    }
}

int Game::readRelayIndex() const {
    std::cout << "Choose relay:\n";
    for (std::size_t i = 0; i < relays_.size(); ++i) {
        std::cout << "  " << (i + 1) << ") " << relays_[i].name << " ("
                  << relayState(relays_[i]) << ")\n";
    }
    std::cout << "Selection: ";

    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return currentRelay_;
        }
        int selection = 0;
        try {
            selection = std::stoi(line);
        } catch (...) {
            selection = 0;
        }
        if (selection >= 1 && selection <= static_cast<int>(relays_.size())) {
            return selection - 1;
        }
        std::cout << "Invalid relay. Enter 1-" << relays_.size() << ": ";
    }
}

void Game::handleAction(int action) {
    switch (action) {
    case 1:
        scanNetwork();
        break;
    case 2:
        travelToRelay();
        break;
    case 3:
        repairRelay();
        break;
    case 4:
        boostHub();
        break;
    case 5:
        rest();
        break;
    case 6:
        running_ = false;
        std::cout << "Session ended.\n";
        break;
    default:
        break;
    }
}

void Game::scanNetwork() const {
    std::cout << "\nNetwork scan:\n";
    for (const Relay& relay : relays_) {
        std::cout << "  - " << relay.name
                  << " | State: " << relayState(relay)
                  << " | Stability: " << relay.stability
                  << " | Interference: " << relay.interference
                  << "\n";
    }
}

void Game::travelToRelay() {
    if (energy_ < 5) {
        std::cout << "Not enough energy to travel.\n";
        return;
    }

    const int destination = readRelayIndex();
    if (destination == currentRelay_) {
        std::cout << "Already at this relay.\n";
        return;
    }

    energy_ -= 5;
    currentRelay_ = destination;
    std::cout << "Moved to " << relays_[currentRelay_].name << ".\n";
}

void Game::repairRelay() {
    if (energy_ < 12 || spareParts_ <= 0) {
        std::cout << "Cannot repair: missing energy or spare parts.\n";
        return;
    }

    std::uniform_int_distribution<int> gainRoll(12, 25);
    std::uniform_int_distribution<int> cleanRoll(4, 15);

    Relay& relay = relays_[currentRelay_];
    energy_ -= 12;
    --spareParts_;

    const int gain = gainRoll(rng_);
    const int clean = cleanRoll(rng_);
    relay.stability = clamp100(relay.stability + gain);
    relay.interference = clamp100(relay.interference - clean);
    if (relay.stability >= 35) {
        relay.online = true;
    }

    std::cout << relay.name << " repaired. +" << gain
              << " stability, -" << clean << " interference.\n";
}

void Game::boostHub() {
    if (batteryPacks_ <= 0 || energy_ < 10) {
        std::cout << "Cannot boost: need 1 battery pack and 10 energy.\n";
        return;
    }

    std::uniform_int_distribution<int> chargeRoll(15, 30);
    const int gain = chargeRoll(rng_);

    --batteryPacks_;
    energy_ -= 10;
    hubCharge_ = clamp100(hubCharge_ + gain);

    std::cout << "Hub boosted by +" << gain << ".\n";
}

void Game::rest() {
    std::uniform_int_distribution<int> recoverRoll(12, 22);
    std::uniform_int_distribution<int> hubLossRoll(3, 8);

    const int recover = recoverRoll(rng_);
    const int hubLoss = hubLossRoll(rng_);
    energy_ = clamp100(energy_ + recover);
    hubCharge_ = clamp100(hubCharge_ - hubLoss);

    std::cout << "You rest. +" << recover << " energy, -" << hubLoss
              << " hub charge.\n";
}

void Game::randomEvent() {
    std::uniform_int_distribution<int> eventRoll(0, 99);
    std::uniform_int_distribution<int> relayIndexRoll(0, static_cast<int>(relays_.size()) - 1);
    std::uniform_int_distribution<int> interfRoll(5, 14);
    std::uniform_int_distribution<int> stabLossRoll(0, 6);
    std::uniform_int_distribution<int> hubLossRoll(4, 12);

    const int event = eventRoll(rng_);
    if (event < 50) {
        Relay& relay = relays_[relayIndexRoll(rng_)];
        const int interfGain = interfRoll(rng_);
        const int stabLoss = stabLossRoll(rng_);
        relay.interference = clamp100(relay.interference + interfGain);
        relay.stability = clamp100(relay.stability - stabLoss);
        if (relay.stability < 25) {
            relay.online = false;
        }
        std::cout << "[Event] Distortion spike at " << relay.name
                  << " (+" << interfGain << " interference).\n";
    } else if (event < 80) {
        const int loss = hubLossRoll(rng_);
        hubCharge_ = clamp100(hubCharge_ - loss);
        std::cout << "[Event] Central hub leak (-" << loss << " charge).\n";
    } else if (event < 92) {
        auto weakest = std::min_element(
            relays_.begin(),
            relays_.end(),
            [](const Relay& a, const Relay& b) { return a.stability < b.stability; });
        weakest->stability = clamp100(weakest->stability + 10);
        if (weakest->stability >= 35) {
            weakest->online = true;
        }
        std::cout << "[Event] Civilians patched " << weakest->name << ".\n";
    } else {
        ++spareParts_;
        std::cout << "[Event] You found an abandoned spare part cache.\n";
    }
}

int Game::computeSignal() const {
    int stabilitySum = 0;
    int onlineCount = 0;

    for (const Relay& relay : relays_) {
        const int effectiveStability = clamp100(relay.stability - (relay.interference / 2));
        stabilitySum += effectiveStability;
        if (relay.online) {
            ++onlineCount;
        }
    }

    const int averageRelay = stabilitySum / static_cast<int>(relays_.size());
    int signal = (averageRelay * 65 + hubCharge_ * 35) / 100;

    
    signal -= (static_cast<int>(relays_.size()) - onlineCount) * 7;
    return clamp100(signal);
}

bool Game::checkEndConditions() {
    const int signal = computeSignal();
    const int onlineCount = static_cast<int>(std::count_if(
        relays_.begin(),
        relays_.end(),
        [](const Relay& relay) { return relay.online; }));

    if (signal >= 85 && onlineCount >= 3) {
        std::cout << "\nVictory: the city is back online.\n";
        running_ = false;
        return true;
    }

    if (turn_ > maxTurns_) {
        std::cout << "\nDefeat: time is up. The network collapsed.\n";
        running_ = false;
        return true;
    }

    if (signal <= 10 && turn_ > 3) {
        std::cout << "\nDefeat: signal fully lost. No recovery possible.\n";
        running_ = false;
        return true;
    }

    if (energy_ <= 0 && spareParts_ <= 0 && batteryPacks_ <= 0) {
        std::cout << "\nDefeat: no resources left to continue.\n";
        running_ = false;
        return true;
    }

    return false;
}
