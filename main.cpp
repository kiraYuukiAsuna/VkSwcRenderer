#include "UI/Application.h"

int main() {
    Application app;

    try{
        app.run();
    }catch (std::exception& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    return 0;
}
