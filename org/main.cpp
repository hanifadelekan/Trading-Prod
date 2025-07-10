#include <iostream>
#include "gui_app.hpp"


int main() {
    try {

        GuiApp app;
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
