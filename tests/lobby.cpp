#include <gtest/gtest.h>
#include "Server.hpp"
#include "Client.hpp"
#include "Packet.hpp"
#include <thread>
#include <atomic>
#include <cstring>
#include <chrono>

TEST(Lobby, ReceiveLobbyListPacket) {
    const int TEST_PORT = 12345;
    std::atomic<bool> packetReceived{false};

    Server server(TEST_PORT);
    server.start();

    Client client;

    for (int i = 0; i < 50; ++i) {
        if (client.connect("127.0.0.1", TEST_PORT)) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client.isConnected());

    client.getCallbackManager().registerCallback(PacketType::LOBBY_LIST,
        [&packetReceived](const void* data, size_t size) {
            if (size == sizeof(LobbyListPacket)) {
                packetReceived = true;
            }
        }
    );

    client.startReceiving();

    auto startTime = std::chrono::steady_clock::now();
    const auto maxWaitTime = std::chrono::seconds(5);
    
    while (!packetReceived.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > maxWaitTime) {
            break;
        }
        std::this_thread::yield();
    }

    EXPECT_TRUE(packetReceived.load());

    client.stopReceiving();
    client.disconnect();
    server.stop();
}

TEST(Lobby, FullLobbyRejectionAndNewLobbyCreation) {
    const int TEST_PORT = 12346;
    
    Server server(TEST_PORT); // crée le serveur sur le port de test
    
    // fonction de gestion des demandes de connexion pour le test
    auto handleConnectRequest = [&server](Player* player, const void* data, size_t size) {
        const ConnectRequestPacket* packet = (const ConnectRequestPacket*)data;
        ConnectResponsePacket response;
        memset(&response, 0, sizeof(response));
        
        // vérifie que le lobby existe
        Lobby* lobby = server.getLobbyManager().findLobbyById(packet->lobbyId);
        if (!lobby) {
            response.accepted = false;
            strncpy(response.reason, "Lobby not found", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        // vérifie que le lobby n'est pas plein
        if (lobby->isFull()) {
            response.accepted = false;
            strncpy(response.reason, "Lobby is full", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        // vérifie que le nom n'est pas déjà utilisé dans ce lobby
        if (lobby->hasPlayerName(packet->playerName)) {
            response.accepted = false;
            strncpy(response.reason, "Name already used in this lobby", sizeof(response.reason) - 1);
            Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
            return;
        }
        
        // ajoute le joueur au lobby
        if (lobby->addConnection(player->connection, packet->playerName, packet->colorId)) {
            strncpy(player->playerName, packet->playerName, sizeof(player->playerName) - 1);
            player->playerName[sizeof(player->playerName) - 1] = '\0';
            player->lobbyId = packet->lobbyId;
            
            response.accepted = true;
            response.lobbyId = packet->lobbyId;
            strncpy(response.reason, "Connection accepted", sizeof(response.reason) - 1);
        } else {
            response.accepted = false;
            strncpy(response.reason, "Error adding to lobby", sizeof(response.reason) - 1);
        }
        
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
    };
    
    server.getCallbackManager().registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest); // enregistre le callback pour gérer les demandes de connexion
    server.start(); // démarre le serveur
    
    Client client1; // crée le premier client
    Client client2; // crée le deuxième client
    Client client3; // crée le troisième client
    
    std::atomic<bool> client1Connected{false}; // flag atomique pour la connexion du premier client
    std::atomic<bool> client2Connected{false}; // flag atomique pour la connexion du deuxième client
    std::atomic<bool> client3Rejected{false}; // flag atomique pour le rejet du troisième client
    std::atomic<bool> lobbyListReceived{false}; // flag atomique pour la réception de la liste des lobbies
    LobbyListPacket receivedLobbyList; // liste des lobbies reçue
    
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
    
    for (int i = 0; i < 50; ++i) {
        if (client3.connect("127.0.0.1", TEST_PORT)) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client3.isConnected());
    
    // enregistrement des callbacks pour le premier client
    client1.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE,
        [&client1Connected](const void* data, size_t size) {
            const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;
            if (packet->accepted) {
                client1Connected = true; // marque la connexion comme acceptée (atomique)
            }
        }
    );
    
    // enregistrement des callbacks pour le deuxième client
    client2.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE,
        [&client2Connected](const void* data, size_t size) {
            const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;
            if (packet->accepted) {
                client2Connected = true; // marque la connexion comme acceptée (atomique)
            }
        }
    );
    
    // enregistrement des callbacks pour le troisième client
    client3.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE,
        [&client3Rejected](const void* data, size_t size) {
            const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;
            if (!packet->accepted) {
                client3Rejected = true; // marque le rejet de la connexion (atomique)
            }
        }
    );
    
    // enregistrement du callback pour recevoir la liste des lobbies
    client3.getCallbackManager().registerCallback(PacketType::LOBBY_LIST,
        [&lobbyListReceived, &receivedLobbyList](const void* data, size_t size) {
            if (size == sizeof(LobbyListPacket)) {
                std::memcpy(&receivedLobbyList, data, sizeof(LobbyListPacket)); // copie la liste des lobbies
                lobbyListReceived = true; // marque la liste comme reçue (atomique)
            }
        }
    );
    
    // démarre la réception des packets pour tous les clients
    client1.startReceiving(); // démarre la réception des packets du premier client
    client2.startReceiving(); // démarre la réception des packets du deuxième client
    client3.startReceiving(); // démarre la réception des packets du troisième client
    
    client1.sendConnectRequest("Player1", 1, 0);
    
    auto startTime = std::chrono::steady_clock::now();
    const auto maxWaitTime = std::chrono::seconds(3);
    while (!client1Connected.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > maxWaitTime) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client1Connected.load());
    
    client2.sendConnectRequest("Player2", 1, 1);
    
    startTime = std::chrono::steady_clock::now();
    while (!client2Connected.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > maxWaitTime) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(client2Connected.load());
    
    startTime = std::chrono::steady_clock::now();
    const auto maxWaitTimeLobby = std::chrono::seconds(5);
    while (!lobbyListReceived.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > maxWaitTimeLobby) {
            break;
        }
        std::this_thread::yield();
    }
    ASSERT_TRUE(lobbyListReceived.load());
    
    // vérifie qu'il y a au moins 2 lobbies (le premier plein et un nouveau créé)
    EXPECT_GE(receivedLobbyList.lobbyCount, 2); // vérifie qu'il y a au moins 2 lobbies
    
    // vérifie que le premier lobby a 2 joueurs (plein)
    bool foundFullLobby = false;
    for (int i = 0; i < receivedLobbyList.lobbyCount; ++i) {
        if (receivedLobbyList.lobbies[i].lobbyId == 1) {
            EXPECT_EQ(receivedLobbyList.lobbies[i].playerCount, 2); // vérifie que le lobby 1 a 2 joueurs
            foundFullLobby = true;
            break;
        }
    }
    ASSERT_TRUE(foundFullLobby); // vérifie que le lobby 1 est présent dans la liste
    
    client3.sendConnectRequest("Player3", 1, 2);
    
    startTime = std::chrono::steady_clock::now();
    const auto maxWaitTimeRejection = std::chrono::seconds(3);
    while (!client3Rejected.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - startTime > maxWaitTimeRejection) {
            break;
        }
        std::this_thread::yield();
    }
    EXPECT_TRUE(client3Rejected.load());
    
    client1.stopReceiving();
    client2.stopReceiving();
    client3.stopReceiving();
    client1.disconnect();
    client2.disconnect();
    client3.disconnect();
    server.stop();
}
