#include "Client.hpp"

int main() {
    auto window = createWindow();
    if (window && window->isOpen()) {
        run(*window);
    }
    return 0;
}

