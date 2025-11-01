#include "Client.hpp"
#include "Render.hpp"
#include "Packet.hpp"
#include <iostream>

void handleLobbyList(const void* data, size_t size) {
    const LobbyListPacket* packet = (const LobbyListPacket*)data;
    std::cout << "Lobbies availables: " << packet->lobbyCount << std::endl;
}

int main() {
    Client client;
    client.connect("127.0.0.1", 5555);
    client.sendConnectRequest("test");

    client.getCallbackManager().registerCallback(PacketType::LOBBY_LIST, handleLobbyList);
    client.startReceiving();
    
    auto window = Render::createWindow();
    if (window && window->isOpen()) {
        Render::run(*window);
    }
}

