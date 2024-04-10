#include "Object/ObjectManager.h"
#include "Object/SwcObject.h"
#include "UI/Application.h"

int main() {
    auto& app = Application::getInstance();

    SwcObject swcObject;
    swcObject.load("/home/seele/Desktop/Swc/ExportSwc/18454_00019.ano.eswc.ano.eswc");
    ObjectManager::getInstance().addObject(&swcObject);

    try {
        app.run();
    } catch (std::exception &e) {
        SEELE_ERROR_TAG(__func__, "{}", e.what());
        return -1;
    }

    return 0;
}
