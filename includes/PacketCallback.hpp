#ifndef PACKET_CALLBACK_HPP
#define PACKET_CALLBACK_HPP

#include <unordered_map>

enum class PacketType;

template<typename CallbackType>
class PacketCallbackManager {
private:
    std::unordered_map<PacketType, CallbackType> callbacks; // map des callbacks pour chaque type de paquet

public:
    void registerCallback(PacketType type, CallbackType callback) { // enregistre un callback pour un type de paquet
        callbacks[type] = callback;
    }

    void unregisterCallback(PacketType type) { // supprime un callback pour un type de paquet
        callbacks.erase(type);
    }

    bool hasCallback(PacketType type) const { // vérifie si un callback existe pour un type de paquet
        return callbacks.find(type) != callbacks.end();
    }

    CallbackType* getCallback(PacketType type) { // récupère un callback pour un type de paquet (retourne nullptr si non trouvé)
        auto it = callbacks.find(type);
        if (it != callbacks.end()) {
            return &(it->second);
        }
        return nullptr;
    }
};

#endif
