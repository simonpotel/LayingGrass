#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstring>
#include <cstddef>

// enumération des types de paquets
enum class PacketType {
    CONNECT_REQUEST, // demande de connexion au serveur avec nom et lobby
    CONNECT_RESPONSE, // réponse du serveur à la demande de connexion
    LOBBY_LIST, // liste des lobbies disponibles
    GAME_START, // début de la partie
    GAME_END, // fin de la partie
    BOARD_UPDATE, // mise à jour de la grille du jeu
    CELL_CLICK, // clic sur une cellule de la grille
    START_GAME_REQUEST, // demande de lancer la partie
    TILE_PREVIEW // prévisualisation de placement de tuile
};

#include "PacketCallback.hpp"

struct PacketHeader { // structure pour le paquet de header
    PacketType type; // type du paquet
    int size; // taille du paquet
};

struct PlayerInfo {
    char playerName[256]; // nom du joueur
    int colorId; // identifiant de la couleur (0-8)
};

struct ConnectRequestPacket {
    char playerName[256]; // nom du joueur
    int lobbyId; // identifiant du lobby choisi
    int colorId; // identifiant de la couleur (0-8)
};

struct ConnectResponsePacket {
    bool accepted; // true si la connexion est acceptée
    char reason[256]; // raison du refus si accepted = false
    int lobbyId; // identifiant du lobby si accepté
};

struct LobbyInfo { // structure pour les informations d'un lobby
    int lobbyId; // identifiant du lobby
    int playerCount; // nombre de joueurs connectés au lobby
    bool gameStarted; // true si la partie est lancée, false sinon
    PlayerInfo players[9]; // liste des joueurs avec leurs noms et couleurs (max 9)
};

struct LobbyListPacket { // structure pour la liste des lobbies
    int lobbyCount; // nombre de lobbies dans la liste
    LobbyInfo lobbies[100]; // tableau des lobbies (max 100 lobbies)
};

struct GameStartPacket {
    int lobbyId; // identifiant du lobby
};

struct GameEndPacket {
    int lobbyId; // identifiant du lobby
    int winnerId; // identifiant de la couleur du gagnant (-1 si pas de gagnant)
    char winnerName[256]; // nom du joueur gagnant
};

struct BoardUpdatePacket {
    int lobbyId; // identifiant du lobby
    int size; // taille de la grille (20 ou 30)
    int grid[900]; // grille 30x30 max (900 cellules)
    int currentTurnColorId; // couleur du joueur dont c'est le tour
    int turnCount; // nombre de tours effectués
    bool gameOver; // true si la partie est terminée
    int winnerId; // identifiant de la couleur du gagnant (-1 si pas de gagnant)
    int currentPlayerTileId; // ID de la tuile du joueur dont c'est le tour (-1 si aucune tuile)
    int exchangeCoupons[9]; // coupons d'échange par couleur
};

struct CellClickPacket {
    int lobbyId; // identifiant du lobby
    int row; // ligne de la cellule cliquée
    int col; // colonne de la cellule cliquée
    int rotation; // rotation de la tuile (0-3 pour 0°, 90°, 180°, 270°)
    bool flippedH; // flip horizontal de la tuile
    bool flippedV; // flip vertical de la tuile
    bool useCoupon; // true si on souhaite utiliser un coupon
};

struct StartGameRequestPacket {
    int lobbyId; // identifiant du lobby
};

struct TilePreviewPacket {
    int lobbyId; // identifiant du lobby
    int row; // ligne de prévisualisation (-1 si pas de prévisualisation)
    int col; // colonne de prévisualisation (-1 si pas de prévisualisation)
    int rotation; // rotation de la tuile (0-3)
    bool flippedH; // flip horizontal
    bool flippedV; // flip vertical
    int colorId; // couleur du joueur qui prévisualise
};

namespace Packet {
    bool sendPacket(int socketFd, PacketType type, const void* data, size_t size); // envoie un paquet sur le socket
    bool receivePacket(int socket, PacketHeader& header, void*& data); // reçoit un paquet et le décode
}

#endif

