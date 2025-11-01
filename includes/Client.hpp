#ifndef CLIENT_HPP
#define CLIENT_HPP

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
    bool sendConnectRequest(const char* playerName); // envoie une demande de connexion au serveur

private:
    int socketFd; // descripteur de socket du client
    bool connected; // true si le client est connecté au serveur
};

#endif
