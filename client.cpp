#include "Client.hpp"
#include "Render.hpp"

int main() {
    Client client;
    client.connect("127.0.0.1", 5555);
    client.sendConnectRequest("test");
    
    auto window = Render::createWindow();
    if (window && window->isOpen()) {
        Render::run(*window);
    }
}

