#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Packet.hpp"
#include <functional>

class Client {
public:
    Client();
    ~Client();

    // gestion du socket
    bool connect(const char* serverIp, int port); // connecte le client au serveur
    void disconnect(); // déconnecte le client du serveur
    int getSocketFd() const { return socketFd; } // retourne le descripteur de socket du client
    bool isConnected() const { return connected; } // retourne true si le client est connecté au serveur

    // packets
    bool sendConnectRequest(const char* playerName, int lobbyId, int colorId); // envoie une demande de connexion au serveur avec lobby et couleur
    bool sendCellClick(int lobbyId, int row, int col, int rotation, bool flippedH, bool flippedV, bool useCoupon = false, int couponChoice = -1); // envoie un clic sur une cellule (ou utilise un coupon)
    bool sendStartGameRequest(int lobbyId); // envoie une demande de lancer la partie
    bool sendTilePreview(int lobbyId, int row, int col, int rotation, bool flippedH, bool flippedV, int colorId); // envoie une prévisualisation de placement de tuile
    bool sendPlaceStone(int lobbyId, int row, int col); // envoie une demande de placer une pierre
    bool sendRobTile(int lobbyId, int targetPlayerColorId); // envoie une demande de voler une tuile
    bool sendDiscardTile(int lobbyId); // envoie une demande d'abandonner une tuile
    void startReceiving(); // démarre le thread de réception des paquets
    void stopReceiving(); // arrête le thread de réception des paquets

    // callbacks
    PacketCallbackManager<std::function<void(const void*, size_t)>>& getCallbackManager() { return callbackManager; }

private:
    int socketFd; // descripteur de socket du client
    bool connected; // true si le client est connecté au serveur
    bool receiving; // true si le thread de réception est actif
    PacketCallbackManager<std::function<void(const void*, size_t)>> callbackManager; // gestionnaire de callbacks pour les paquets
    void receiveLoop(); // boucle de réception des paquets dans un thread dédié
};

#endif
