#include "oxygine-framework.h"
#include <iostream> 
#include <string>

using namespace oxygine;

// Функция ручной загрузки 
spResAnim loadManual(const std::string& fileName) {
    file::buffer fb;
    std::string path = "data/images/" + fileName;
    
    if (!file::read(path, fb)) {
        logs::error("CANNOT READ FILE: %s", path.c_str());
        return nullptr;
    }

    Image img;
    img.init(fb);
    
    // Создание текстуры
    spNativeTexture texture = IVideoDriver::instance->createTexture();
    texture->init(img.lock());
    
    // Создание ResAnim
    spResAnim rs = new ResAnim();
    rs->init(texture, img.getSize()); 
    return rs;
}

void main_loop() {
    getStage()->update();
}

void run() {
    core::init_desc desc;
    desc.title = "Chess Game";
    desc.w = 800;
    desc.h = 600;
    core::init(&desc);

    Stage::instance = new Stage(true);
    Point displaySize = core::getDisplaySize();
    getStage()->setSize(displaySize);

    // Загрузка доски
    spResAnim boardRes = loadManual("deck.png");
    if (boardRes) {
        spSprite board = new Sprite();
        board->setResAnim(boardRes.get()); 
        board->setAnchor(0.5f, 0.5f);
        board->setPosition(displaySize.x / 2.0f, displaySize.y / 2.0f);
        
        float scale = (displaySize.y * 0.9f) / board->getHeight();
        board->setScale(scale);
        getStage()->addChild(board);
    }

    // Загрузка пешки
    spResAnim pawnRes = loadManual("w_pawn.png");
    if (pawnRes) {
        spSprite pawn = new Sprite();
        pawn->setResAnim(pawnRes.get());
        pawn->setAnchor(0.5f, 0.5f);
        pawn->setPosition(displaySize.x / 2.0f, displaySize.y / 2.0f);
        
        // Находим доску (первый ребенок сцены) и копируем её масштаб
        spActor board = getStage()->getFirstChild();
        if (board) pawn->setScale(board->getScale());
        
        getStage()->addChild(pawn);
    }

    while (1) {
        int done = core::update();
        if (done) break;
        main_loop();
        if (core::beginRendering()) {
           Color clearColor(32, 32, 32, 255);
           Rect viewport(Point(0, 0), displaySize);
           getStage()->render(clearColor, viewport);
           core::swapDisplayBuffers();
        }
    }
    
    getStage()->removeChildren();
    core::release();
}

int main(int argc, char* argv[]) {
    run();
    return 0;
}
