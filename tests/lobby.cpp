#include <gtest/gtest.h>
#include "Server.hpp"
#include "Client.hpp"
#include "Packet.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <cstring>

// test de réception du packet de liste des lobbies depuis le serveur
TEST(Lobby, ReceiveLobbyListPacket) {
    const int TEST_PORT = 12345;
    std::atomic<bool> packetReceived{false}; // flag atomique (thread-safe)

    Server server(TEST_PORT); // crée le serveur sur le port de test
    server.start(); // démarre le serveur

    Client client; // crée le client

    // tente de se connecter au serveur (retry si pas encore prêt)
    bool connected = false;
    for (int i = 0; i < 10; ++i) {
        if (client.connect("127.0.0.1", TEST_PORT)) {
            connected = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_TRUE(connected); // vérifie que la connexion a réussi

    client.getCallbackManager().registerCallback(PacketType::LOBBY_LIST,
        [&packetReceived](const void* data, size_t size) {
            if (size == sizeof(LobbyListPacket)) {
                packetReceived = true; // marque le packet comme reçu (atomique)
            }
        }
    );

    client.startReceiving(); // démarre la réception des packets

    // attend que le packet soit reçu (max 3 secondes)
    for (int i = 0; i < 60 && !packetReceived; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    EXPECT_TRUE(packetReceived); // vérifie que le packet a été reçu

    client.disconnect(); // déconnecte le client
    server.stop(); // arrête le serveur
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
        if (lobby->addConnection(player->connection, packet->playerName)) {
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
    
    // connexion du premier client au serveur
    bool connected1 = false;
    for (int i = 0; i < 10; ++i) {
        if (client1.connect("127.0.0.1", TEST_PORT)) {
            connected1 = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_TRUE(connected1); // vérifie que la connexion du premier client a réussi
    
    // connexion du deuxième client au serveur
    bool connected2 = false;
    for (int i = 0; i < 10; ++i) {
        if (client2.connect("127.0.0.1", TEST_PORT)) {
            connected2 = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_TRUE(connected2); // vérifie que la connexion du deuxième client a réussi
    
    // connexion du troisième client au serveur
    bool connected3 = false;
    for (int i = 0; i < 10; ++i) {
        if (client3.connect("127.0.0.1", TEST_PORT)) {
            connected3 = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_TRUE(connected3); // vérifie que la connexion du troisième client a réussi
    
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
    
    // envoie la demande de connexion au lobby 1 pour le premier client
    client1.sendConnectRequest("Player1", 1);
    
    // attend que le premier client soit connecté au lobby
    for (int i = 0; i < 60 && !client1Connected; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_TRUE(client1Connected); // vérifie que le premier client est connecté au lobby
    
    // envoie la demande de connexion au lobby 1 pour le deuxième client
    client2.sendConnectRequest("Player2", 1);
    
    // attend que le deuxième client soit connecté au lobby
    for (int i = 0; i < 60 && !client2Connected; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_TRUE(client2Connected); // vérifie que le deuxième client est connecté au lobby
    
    // attend que la liste des lobbies soit reçue (le serveur envoie périodiquement toutes les 2 secondes)
    for (int i = 0; i < 80 && !lobbyListReceived; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_TRUE(lobbyListReceived); // vérifie que la liste des lobbies a été reçue
    
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
    
    // tente de connecter le troisième client au lobby 1 (qui est plein)
    client3.sendConnectRequest("Player3", 1);
    
    // attend que le rejet soit reçu
    for (int i = 0; i < 60 && !client3Rejected; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    EXPECT_TRUE(client3Rejected); // vérifie que le troisième client a été rejeté du lobby plein
    
    // déconnecte tous les clients
    client1.disconnect(); // déconnecte le premier client
    client2.disconnect(); // déconnecte le deuxième client
    client3.disconnect(); // déconnecte le troisième client
    
    server.stop(); // arrête le serveur
}
