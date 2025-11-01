#include "Server.hpp"
#include "Packet.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

void handleConnectRequest(Player* player, const void* data, size_t size) {
    const ConnectRequestPacket* packet = (const ConnectRequestPacket*)data;
    strncpy(player->playerName, packet->playerName, sizeof(player->playerName) - 1);
    player->playerName[sizeof(player->playerName) - 1] = '\0';
    std::cout << "Player connected: " << player->playerName << std::endl;
}

int main() {
    Server server(5555);
    server.registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest);
    server.start();
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

