#include "oxygine-framework.h"
#include "ChessController.h"

using namespace oxygine;

int main(int argc, char* argv[]) {
    core::init_desc desc;
    desc.title = "Chess Game";
    desc.w = 800; desc.h = 600;
    core::init(&desc);

    Stage::instance = new Stage(true);

    // Запускаем игровой контроллер
    ChessController::instance().run();

    core::release();
    return 0;
}
