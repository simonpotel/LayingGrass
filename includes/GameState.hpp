#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Packet.hpp"
#include <vector>
#include <string>

namespace sf {
    class Color;
}

// états possibles pour le client
enum class ClientState {
    WAITING_FOR_LOBBIES, // en attente de la liste des lobbies
    SELECTING_LOBBY, // sélection d'un lobby
    ENTERING_USERNAME, // saisie du nom d'utilisateur
    WAITING_FOR_RESPONSE, // en attente de la réponse du serveur
    IN_LOBBY, // dans un lobby
    IN_GAME, // en jeu
    GAME_END // fin de partie
};

// classe pour gérer l'état du client
class GameState {
public:
    static const sf::Color* PLAYERS_COLORS; // tableau des 9 couleurs distinctes pour les joueurs

    GameState();
    ~GameState();

    // gestion de l'état
    ClientState getState() const { return currentState; }
    void setState(ClientState state) { currentState = state; }

    // gestion des lobbies
    void updateLobbies(const LobbyListPacket& packet);
    const std::vector<LobbyInfo>& getLobbies() const { return lobbies; }

    // sélection du lobby
    void setSelectedLobby(int lobbyId) { selectedLobbyId = lobbyId; }
    int getSelectedLobby() const { return selectedLobbyId; }

    // gestion du nom d'utilisateur
    void setUsername(const std::string& name) { username = name; }
    const std::string& getUsername() const { return username; }

    // gestion de la couleur sélectionnée
    void setSelectedColor(int colorId) { selectedColorId = colorId; }
    int getSelectedColor() const { return selectedColorId; }

    // gestion du lobby actuel
    void setCurrentLobby(int lobbyId) { currentLobbyId = lobbyId; }
    int getCurrentLobby() const { return currentLobbyId; }

    // flag pour savoir si la requête de connexion a déjà été envoyée
    void setRequestSent(bool sent) { requestSent = sent; }
    bool isRequestSent() const { return requestSent; }

    // gestion du board
    void updateBoard(const BoardUpdatePacket& packet);
    const std::vector<std::vector<int>>& getBoard() const { return board; }
    int getBoardSize() const { return boardSize; }
    int getCurrentTurnColorId() const { return currentTurnColorId; }
    int getTurnCount() const { return turnCount; }
    bool isGameOver() const { return gameOver; }
    int getWinnerId() const { return winnerId; }
    
    // gestion du winnerId du GameEnd
    void setGameEndWinnerId(int winnerId) { gameEndWinnerId = winnerId; }
    int getGameEndWinnerId() const { return gameEndWinnerId; }
    void setGameEndLobbyId(int lobbyId) { gameEndLobbyId = lobbyId; }
    int getGameEndLobbyId() const { return gameEndLobbyId; }
    void setGameEndWinnerName(const std::string& name) { gameEndWinnerName = name; }
    const std::string& getGameEndWinnerName() const { return gameEndWinnerName; }

private:
    ClientState currentState; // état actuel du client
    std::vector<LobbyInfo> lobbies; // liste des lobbies disponibles
    int selectedLobbyId; // identifiant du lobby sélectionné
    int currentLobbyId; // identifiant du lobby actuel (quand connecté)
    std::string username; // nom d'utilisateur saisi
    int selectedColorId; // identifiant de la couleur sélectionnée (-1 si aucune)
    bool requestSent; // true si la requête de connexion a été envoyée
    
    std::vector<std::vector<int>> board; // grille du jeu
    int boardSize; // taille de la grille
    int currentTurnColorId; // couleur du joueur dont c'est le tour
    int turnCount; // nombre de tours effectués
    bool gameOver; // true si la partie est terminée
    int winnerId; // identifiant de la couleur du gagnant
    int gameEndWinnerId; // identifiant de la couleur du gagnant stocké pour l'affichage GameEnd
    int gameEndLobbyId; // identifiant du lobby stocké pour l'affichage GameEnd
    std::string gameEndWinnerName; // nom du joueur gagnant stocké pour l'affichage GameEnd
};

#endif
