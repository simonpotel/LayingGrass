#include "Game/Cell.hpp"

Cell::Cell() : type(CellType::EMPTY), playerId(-1) {
}

Cell::Cell(int playerId) : type(CellType::GRASS), playerId(playerId) {
}

Cell::Cell(CellType type) : type(type), playerId(-1) {
    // Les bonus et pierres n'ont pas de joueur associé
    if (type == CellType::GRASS) {
        // Si on crée une GRASS sans joueur, c'est une erreur
        this->type = CellType::EMPTY;
    }
}

void Cell::setPlayer(int newPlayerId) {
    playerId = newPlayerId;
    type = CellType::GRASS;
}

void Cell::setType(CellType newType) {
    type = newType;
    if (newType != CellType::GRASS) {
        playerId = -1; // Les cellules non-GRASS n'ont pas de joueur
    }
}

void Cell::clear() {
    type = CellType::EMPTY;
    playerId = -1;
}

int Cell::toInt() const {
    switch (type) {
        case CellType::EMPTY:
            return -1;
        case CellType::STONE:
            return 99;
        case CellType::BONUS_EXCHANGE:
            return 100;
        case CellType::BONUS_STONE:
            return 101;
        case CellType::BONUS_ROBBERY:
            return 102;
        case CellType::GRASS:
            // Pour GRASS, on retourne le playerId (0-8)
            return playerId;
        default:
            return -1;
    }
}

Cell Cell::fromInt(int value) {
    if (value == -1) {
        return Cell(CellType::EMPTY);
    } else if (value == 99) {
        return Cell(CellType::STONE);
    } else if (value == 100) {
        return Cell(CellType::BONUS_EXCHANGE);
    } else if (value == 101) {
        return Cell(CellType::BONUS_STONE);
    } else if (value == 102) {
        return Cell(CellType::BONUS_ROBBERY);
    } else if (value >= 0 && value < 9) {
        // C'est un joueur (0-8)
        return Cell(value);
    } else {
        // Valeur inconnue, on retourne vide
        return Cell(CellType::EMPTY);
    }
}

bool Cell::operator==(const Cell& other) const {
    return type == other.type && playerId == other.playerId;
}

bool Cell::operator!=(const Cell& other) const {
    return !(*this == other);
}

