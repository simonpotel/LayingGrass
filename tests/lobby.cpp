#include <gtest/gtest.h>
#include "Server.hpp"
#include "Client.hpp"
#include <thread>
#include <chrono>
#include <atomic>

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
