#include "UI/Application.h"

int main() {
    Application app;

    try {
        app.run();
    } catch (std::exception &e) {
        SEELE_ERROR_TAG(__func__, "{}", e.what());
        return -1;
    }

    return 0;
}
