#include "oxygine-framework.h"

using namespace oxygine;


void main_loop()
{
    // обновление состояния движка каждый кадр
    getStage()->update();
}

void run()
{
    // инициализация окна
    core::init_desc desc;
    desc.title = "Chess Game";
    desc.w = 800;
    desc.h = 600;

    core::init(&desc);

    Stage::instance = new Stage(true);
    Point size = core::getDisplaySize();
    getStage()->setSize(size);
 
    while (1)
    {
        // обработка системных событий
        bool done = core::update();
        if (done) break;

        main_loop();
        // отрисовка сцены
        if (core::beginRendering())
        {
           Color clearColor(32, 32, 32, 255);
           Rect viewport(Point(0, 0), core::getDisplaySize());
           getStage()->render(clearColor, viewport);
           core::swapDisplayBuffers();
        }
    }

    ObjectBase::dumpCreatedObjects();
    core::release();
}


int main(int argc, char* argv[])
{
    run();
    return 0;
}
