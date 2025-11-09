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
            return static_cast<int>(CellType::EMPTY);
        case CellType::STONE:
            return static_cast<int>(CellType::STONE);
        case CellType::BONUS_EXCHANGE:
            return static_cast<int>(CellType::BONUS_EXCHANGE);
        case CellType::BONUS_STONE:
            return static_cast<int>(CellType::BONUS_STONE);
        case CellType::BONUS_ROBBERY:
            return static_cast<int>(CellType::BONUS_ROBBERY);
        case CellType::GRASS:
            // Pour GRASS, on retourne le playerId (0-8)
            return playerId;
        default:
            return static_cast<int>(CellType::EMPTY);
    }
}

Cell Cell::fromInt(int value) {
    if (value == static_cast<int>(CellType::EMPTY)) {
        return Cell(CellType::EMPTY);
    } else if (value == static_cast<int>(CellType::STONE)) {
        return Cell(CellType::STONE);
    } else if (value == static_cast<int>(CellType::BONUS_EXCHANGE)) {
        return Cell(CellType::BONUS_EXCHANGE);
    } else if (value == static_cast<int>(CellType::BONUS_STONE)) {
        return Cell(CellType::BONUS_STONE);
    } else if (value == static_cast<int>(CellType::BONUS_ROBBERY)) {
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

