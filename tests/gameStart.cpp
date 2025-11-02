#include <gtest/gtest.h>
#include "Server.hpp"
#include "Client.hpp"
#include "Packet.hpp"
#include <thread>
#include <atomic>
#include <cstring>

TEST(GameStart, TwoPlayersJoinLobbyReceiveGameStartPacket) {
    const int TEST_PORT = 12347;
    
    Server server(TEST_PORT);
    
    auto handleConnectRequest = [&server](Player* player, const void* data, size_t size) {
        const ConnectRequestPacket* packet = (const ConnectRequestPacket*)data;
        ConnectResponsePacket response;
        memset(&response, 0, sizeof(response));
        
        Lobby* lobby = server.getLobbyManager().findLobbyById(packet->lobbyId);
        if (!lobby) {
            response.accepted = false;
            strncpy(response.reason, "Lobby not found", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        if (lobby->isFull()) {
            response.accepted = false;
            strncpy(response.reason, "Lobby is full", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        if (lobby->hasPlayerName(packet->playerName)) {
            response.accepted = false;
            strncpy(response.reason, "Name already used in this lobby", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        if (lobby->addConnection(player->connection, packet->playerName)) {
            strncpy(player->playerName, packet->playerName, sizeof(player->playerName) - 1);
            player->playerName[sizeof(player->playerName) - 1] = '\0';
            player->lobbyId = packet->lobbyId;
            
            response.accepted = true;
            response.lobbyId = packet->lobbyId;
            strncpy(response.reason, "Connection accepted", sizeof(response.reason) - 1);
            
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            
            if (lobby->isFull()) {
                lobby->gameStarted = true;
                GameStartPacket gameStartPacket;
                gameStartPacket.lobbyId = packet->lobbyId;
                
                lobby->broadcast(PacketType::GAME_START, &gameStartPacket, sizeof(GameStartPacket));
            }
        } else {
            response.accepted = false;
            strncpy(response.reason, "Error adding to lobby", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        }
    };
    
    server.getCallbackManager().registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest);
    server.start();
    
    Client client1;
    Client client2;
    
    std::atomic<bool> client1Connected{false};
    std::atomic<bool> client2Connected{false};
    std::atomic<bool> client1GameStart{false};
    std::atomic<bool> client2GameStart{false};
    GameStartPacket client1GameStartPacket;
    GameStartPacket client2GameStartPacket;
    
    for (int i = 0; i < 50; ++i) {
        if (client1.connect("127.0.0.1", TEST_PORT)) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client1.isConnected());
    
    for (int i = 0; i < 50; ++i) {
        if (client2.connect("127.0.0.1", TEST_PORT)) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client2.isConnected());
    
    client1.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE,
        [&client1Connected](const void* data, size_t size) {
            const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;
            if (packet->accepted) {
                client1Connected = true;
            }
        }
    );
    
    client2.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE,
        [&client2Connected](const void* data, size_t size) {
            const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;
            if (packet->accepted) {
                client2Connected = true;
            }
        }
    );
    
    client1.getCallbackManager().registerCallback(PacketType::GAME_START,
        [&client1GameStart, &client1GameStartPacket](const void* data, size_t size) {
            if (size == sizeof(GameStartPacket)) {
                std::memcpy(&client1GameStartPacket, data, sizeof(GameStartPacket));
                client1GameStart = true;
            }
        }
    );
    
    client2.getCallbackManager().registerCallback(PacketType::GAME_START,
        [&client2GameStart, &client2GameStartPacket](const void* data, size_t size) {
            if (size == sizeof(GameStartPacket)) {
                std::memcpy(&client2GameStartPacket, data, sizeof(GameStartPacket));
                client2GameStart = true;
            }
        }
    );
    
    client1.startReceiving();
    client2.startReceiving();
    
    client1.sendConnectRequest("Player1", 1);
    
    for (int i = 0; i < 1000 && !client1Connected; ++i) {
        std::this_thread::yield();
    }
    ASSERT_TRUE(client1Connected);
    
    client2.sendConnectRequest("Player2", 1);
    
    for (int i = 0; i < 1000 && !client2Connected; ++i) {
        std::this_thread::yield();
    }
    ASSERT_TRUE(client2Connected);
    
    for (int i = 0; i < 1000 && (!client1GameStart || !client2GameStart); ++i) {
        std::this_thread::yield();
    }
    
    EXPECT_TRUE(client1GameStart);
    EXPECT_TRUE(client2GameStart);
    
    if (client1GameStart && client2GameStart) {
        EXPECT_EQ(client1GameStartPacket.lobbyId, 1);
        EXPECT_EQ(client2GameStartPacket.lobbyId, 1);
    }
    
    client1.stopReceiving();
    client2.stopReceiving();
    client1.disconnect();
    client2.disconnect();
    server.stop();
}

