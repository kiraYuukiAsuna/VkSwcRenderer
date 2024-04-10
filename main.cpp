#include "Object/ObjectManager.h"
#include "Object/SwcObject.h"
#include "UI/Application.h"

int main() {
    auto& app = Application::getInstance();

    SwcObject swcObject;
    swcObject.load("../brainID_18865_X_7924.17_Y_4242.94_Z_2736.16.v3dpbd.ano.eswc");
    ObjectManager::getInstance().addObject(&swcObject);

    try {
        app.run();
    } catch (std::exception &e) {
        SEELE_ERROR_TAG(__func__, "{}", e.what());
        return -1;
    }

    return 0;
}
