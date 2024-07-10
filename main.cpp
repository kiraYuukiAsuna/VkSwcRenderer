#include "Object/ObjectManager.h"
#include "Object/SwcObject.h"
#include "UI/Application.h"
#include "Util/Logger/Log.h"

int main() {
    Seele::Log::Init();

    auto& app = Application::getInstance();

    SwcObject swcObject;
    swcObject.load("../brainID_18865_X_7924.17_Y_4242.94_Z_2736.16.v3dpbd.ano.eswc");
    ObjectManager::getInstance().addObject(&swcObject);

    try {
        app.run();
    } catch (std::exception &e) {
        SeeleErrorTag(__func__, "{}", e.what());
        return -1;
    }

    Seele::Log::Shutdown();
}
