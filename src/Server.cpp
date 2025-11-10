#include "Server.hpp"
#include "Game/Game.hpp"
#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close(s) closesocket(s)
    #define socklen_t int
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

Server::Server(int port) : port(port), running(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1; // option pour le socket qui permet de réutiliser l'adresse
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    
    sockaddr_in address; // structure pour l'adresse du serveur
    address.sin_family = AF_INET; // famille d'adresse (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // adresse IP du serveur
    address.sin_port = htons(port); // port du serveur
    
    if (bind(serverSocket, (sockaddr*)&address, sizeof(address)) < 0) {
        // erreur lors du bind du socket à l'adresse
    }
    listen(serverSocket, 30); // écoute les connexions des clients (30 max)
}

Server::~Server() {
    stop(); // arrête le serveur
    lobbyManager.stop(); // arrête le gestionnaire de lobbies
    close(serverSocket); // ferme le socket
#ifdef _WIN32 
    WSACleanup(); // libère les ressources WSADATA
#endif
}

void Server::start() {
    running = true; // le serveur est en cours d'exécution
    std::thread acceptThread(&Server::acceptConnections, this); // crée un thread pour accepter les connexions des clients
    acceptThread.detach(); // détache le thread pour que le serveur puisse continuer à fonctionner
    
    std::thread gameUpdateThread(&Server::gameUpdateLoop, this); // crée un thread pour la mise à jour des jeux
    gameUpdateThread.detach(); // détache le thread pour que le serveur puisse continuer à fonctionner
    
    lobbyManager.startPeriodicUpdates([this](const LobbyListPacket& packet) {
        for (auto& player : players) {
            // if (player->lobbyId == -1) { // si le joueur n'est pas assigné à un lobby
            Packet::sendPacket(player->connection, PacketType::LOBBY_LIST, &packet, sizeof(LobbyListPacket)); // envoie la liste des lobbies au joueur
            // }
        }
    });
}

void Server::stop() {
    running = false; // le serveur n'est plus en cours d'exécution
}

void Server::acceptConnections() {
    while (running) {
        sockaddr_in clientAddress; // structure pour l'adresse du client
        socklen_t clientLen = sizeof(clientAddress); // taille de l'adresse du client
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientLen); // accepte la connexion du client
        
        if (clientSocket < 0) {
            continue; // si la connexion échoue, on continue la boucle
        }
        
        auto player = std::make_unique<Player>("", 0, clientSocket); // crée un joueur pour le client
        players.push_back(std::move(player)); // ajoute le joueur à la liste des joueurs
        
        std::thread clientThread(&Server::handleClient, this, clientSocket); // crée un thread pour gérer le client
        clientThread.detach(); // détache le thread pour que le serveur puisse continuer à fonctionner
    }
}

void Server::handleClient(int clientSocket) {
    Player* player = nullptr; // pointeur vers le joueur
    for (auto& p : players) {
        if (p->connection == clientSocket) {
            player = p.get(); // on récupère le joueur
            break;
        }
    }

    if (!player) return; // si le joueur n'est pas trouvé, on continue la boucle

    while (running) {
        PacketHeader header; // structure pour le paquet de header
        void* data = nullptr;

        if (!Packet::receivePacket(clientSocket, header, data)) {
            break; // si le paquet n'est pas reçu, on arrête la boucle
        }

        // appelle le callback correspondant au type de paquet via le callback manager
        auto* callback = callbackManager.getCallback(header.type);
        if (callback) {
            // filtre le type 8 (TILE_PREVIEW) pour éviter le spam dans les logs
            if (static_cast<int>(header.type) != 8) {
                std::cout << "[SERVER] [PACKET] Type: " << static_cast<int>(header.type);
                switch (header.type) {
                    case PacketType::CONNECT_REQUEST: std::cout << " (CONNECT_REQUEST)"; break;
                    case PacketType::CONNECT_RESPONSE: std::cout << " (CONNECT_RESPONSE)"; break;
                    case PacketType::LOBBY_LIST: std::cout << " (LOBBY_LIST)"; break;
                    case PacketType::GAME_START: std::cout << " (GAME_START)"; break;
                    case PacketType::GAME_END: std::cout << " (GAME_END)"; break;
                    case PacketType::BOARD_UPDATE: std::cout << " (BOARD_UPDATE)"; break;
                    case PacketType::CELL_CLICK: std::cout << " (CELL_CLICK)"; break;
                    case PacketType::START_GAME_REQUEST: std::cout << " (START_GAME_REQUEST)"; break;
                    case PacketType::TILE_PREVIEW: std::cout << " (TILE_PREVIEW)"; break;
                    case PacketType::PLACE_STONE: std::cout << " (PLACE_STONE)"; break;
                    case PacketType::ROB_TILE: std::cout << " (ROB_TILE)"; break;
                    case PacketType::DISCARD_TILE: std::cout << " (DISCARD_TILE)"; break;
                }
                std::cout << " Size: " << header.size;
                if (player) {
                    std::cout << " From: " << player->connection << " (" << player->playerName << ")";
                }
                std::cout << std::endl;
            }
            (*callback)(player, data, header.size);
        } else {
            std::cout << "[SERVER] [PACKET] Type: " << static_cast<int>(header.type) << " Size: " << header.size;
            if (player) {
                std::cout << " From: " << player->connection << " (" << player->playerName << ")";
            }
            std::cout << " - NO CALLBACK!" << std::endl;
        }

        if (data) {
            delete[] (char*)data; // on libère la mémoire allouée pour le paquet
        }
    }

    // le joueur s'est déconnecté, on le retire du lobby et de la liste
    removePlayer(clientSocket);
    close(clientSocket); // ferme le socket du client
}

void Server::removePlayer(int clientSocket) {
    // trouve le joueur et retire-le du lobby
    for (auto& player : players) {
        if (player->connection == clientSocket) {
            // retire le joueur du lobby s'il y est
            if (player->lobbyId != -1) {
                Lobby* lobby = lobbyManager.findLobbyById(player->lobbyId);
                if (lobby) {
                    // retire la connexion du lobby
                    lobby->removeConnection(clientSocket);

                    // si une partie est en cours dans ce lobby, on met fin à la partie sans vainqueur
                    if (lobby->gameStarted && lobby->getGame() != nullptr) {
                        Game* game = lobby->getGame();
                        int boardSize = game->getBoard()->getSize();

                        // diffuse un board vide et un GameEnd avec winnerId = -1
                        BoardUpdatePacket emptyBoardPacket;
                        memset(&emptyBoardPacket, 0, sizeof(BoardUpdatePacket));
                        emptyBoardPacket.lobbyId = lobby->lobbyId;
                        emptyBoardPacket.size = boardSize;
                        emptyBoardPacket.currentTurnColorId = -1;
                        emptyBoardPacket.turnCount = 0;
                        emptyBoardPacket.gameOver = true;
                        emptyBoardPacket.winnerId = -1;
                        emptyBoardPacket.currentPlayerTileId = -1;
                        for (int i = 0; i < 900; ++i) {
                            emptyBoardPacket.grid[i] = -1;
                        }
                        lobby->broadcast(PacketType::BOARD_UPDATE, &emptyBoardPacket, sizeof(BoardUpdatePacket));

                        GameEndPacket gameEndPacket;
                        memset(&gameEndPacket, 0, sizeof(GameEndPacket));
                        gameEndPacket.lobbyId = lobby->lobbyId;
                        gameEndPacket.winnerId = -1; // pas de vainqueur
                        gameEndPacket.winnerName[0] = '\0';
                        lobby->broadcast(PacketType::GAME_END, &gameEndPacket, sizeof(GameEndPacket));

                        // nettoie le lobby et réinitialise l'état des joueurs
                        clearLobbyAndRemovePlayers(lobby->lobbyId);
                    }
                }
            }
            break;
        }
    }

    // retire le joueur de la liste des joueurs
    players.erase(
        std::remove_if(players.begin(), players.end(),
            [clientSocket](const auto& p) { return p->connection == clientSocket; }),
        players.end()
    );
}

void Server::clearLobbyAndRemovePlayers(int lobbyId) {
    Lobby* lobby = lobbyManager.findLobbyById(lobbyId);
    if (!lobby) {
        return;
    }

    for (auto& player : players) {
        if (player->lobbyId == lobbyId) {
            player->lobbyId = -1;
        }
    }

    lobby->clear();
}

void Server::gameUpdateLoop() {
    while (running) { // tant que le serveur est en cours d'exécution
        const auto& lobbies = lobbyManager.getLobbies(); // obtient la liste des lobbies
        for (const auto& lobby : lobbies) {
            Game* game = lobby->getGame(); // obtient le jeu associé au lobby
            if (game) { 
                game->update(); // met à jour le jeu
                
                if (game->isGameOver()) { // si la partie est terminée
                    if (game->hasRemainingCoupons()) {
                        continue; // attendre que tous les coupons soient utilisés/écartés
                    }

                    int winnerId = game->getWinner(); // récupère le winnerId avant de nettoyer
                    int boardSize = game->getBoard()->getSize(); // récupère la taille du board
                    
                    std::string winnerName = "";
                    for (int conn : lobby->connections) {
                        auto colorIt = lobby->playerColors.find(conn);
                        if (colorIt != lobby->playerColors.end() && colorIt->second == winnerId) {
                            auto nameIt = lobby->playerNames.find(conn);
                            if (nameIt != lobby->playerNames.end()) {
                                winnerName = nameIt->second;
                                break;
                            }
                        }
                    }
                    
                    BoardUpdatePacket emptyBoardPacket; // paquet de mise à jour de la grille vide
                    memset(&emptyBoardPacket, 0, sizeof(BoardUpdatePacket));
                    emptyBoardPacket.lobbyId = lobby->lobbyId;
                    emptyBoardPacket.size = boardSize;
                    emptyBoardPacket.currentTurnColorId = -1;
                    emptyBoardPacket.turnCount = 0;
                    emptyBoardPacket.gameOver = true;
                    emptyBoardPacket.winnerId = -1;
                    
                    for (int i = 0; i < 900; ++i) {
                        emptyBoardPacket.grid[i] = -1; // initialise toutes les cellules à -1 (vide)
                    }
                    for (int i = 0; i < 9; ++i) {
                        emptyBoardPacket.exchangeCoupons[i] = 0;
                        emptyBoardPacket.pendingStoneBonus[i] = false;
                        emptyBoardPacket.pendingRobberyBonus[i] = false;
                    }
                    for (int i = 0; i < 5; ++i) {
                        emptyBoardPacket.upcomingTiles[i] = -1;
                    }
                    
                    lobby->broadcast(PacketType::BOARD_UPDATE, &emptyBoardPacket, sizeof(BoardUpdatePacket)); // envoie le board vide à tous les joueurs du lobby
                    
                    GameEndPacket gameEndPacket; // paquet de fin de partie
                    memset(&gameEndPacket, 0, sizeof(GameEndPacket));
                    gameEndPacket.lobbyId = lobby->lobbyId;
                    gameEndPacket.winnerId = winnerId;
                    strncpy(gameEndPacket.winnerName, winnerName.c_str(), sizeof(gameEndPacket.winnerName) - 1);
                    gameEndPacket.winnerName[sizeof(gameEndPacket.winnerName) - 1] = '\0';
                    
                    lobby->broadcast(PacketType::GAME_END, &gameEndPacket, sizeof(GameEndPacket)); // envoie le paquet de fin de partie à tous les joueurs du lobby
                    
                    clearLobbyAndRemovePlayers(lobby->lobbyId); // vide le lobby et retire les joueurs du lobby
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1)); // attend 1 seconde avant de continuer la boucle (peut être inutile)
    }
}

