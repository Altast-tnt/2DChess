#include "oxygine-framework.h"
#include "Piece.h"
#include <map>
#include <string>

using namespace oxygine;

// Хранилище всех текстур фигур
std::map<std::string, spResAnim> pieceRes;
// Список всех живых фигур на сцене
std::vector<spPiece> allPieces;

// Размеры и отступы (под deck.png)
float boardScale = 1.0f;
float boardX = 400, boardY = 300; // Центр
float cellSize = 60.0f;     // Размер клетки внутри картинки deck.png
float boardPadding = 40.0f; // Отступ от края доски до начала первой клетки 

spPiece selectedPiece = nullptr;
PColor currentTurn = PColor::White; // Белые ходят первыми

spResAnim loadManual(const std::string& fileName) {
    file::buffer fb;
    if (!file::read("data/images/" + fileName, fb)) return nullptr;
    Image img; img.init(fb);
    spNativeTexture texture = IVideoDriver::instance->createTexture();
    texture->init(img.lock());
    spResAnim rs = new ResAnim();
    rs->init(texture, img.getSize());
    return rs;
}

// Функция перевода координат доски (0-7) в координаты экрана (пиксели)
Vector2 boardToPixels(int x, int y) {
    // 560.0f - оригинальный размер deck.png
    float halfBoard = 560.0f / 2.0f;
    
    // Верхний левый угол доски в пикселях экрана
    float startX = boardX - (halfBoard * boardScale) + (boardPadding * boardScale);
    float startY = boardY - (halfBoard * boardScale) + (boardPadding * boardScale);
    
    // Считаем позицию центра клетки
    float px = startX + (x * cellSize + cellSize / 2.0f) * boardScale;
    float py = startY + (y * cellSize + cellSize / 2.0f) * boardScale;
    
    return Vector2(px, py);
}

Point pixelsToBoard(Vector2 pos) {
    float halfBoard = 560.0f / 2.0f;
    float startX = boardX - (halfBoard * boardScale) + (boardPadding * boardScale);
    float startY = boardY - (halfBoard * boardScale) + (boardPadding * boardScale);

    int x = (pos.x - startX) / (cellSize * boardScale);
    int y = (pos.y - startY) / (cellSize * boardScale);

    return Point(x, y);
}

bool isMoveValid(spPiece p, int targetX, int targetY) {
    int dx = abs(targetX - p->gridX);
    int dy = abs(targetY - p->gridY);

    switch (p->type) {
        case PType::Pawn:
            // Упрощенно: пешка ходит на 1 клетку вперед
            if (p->color == PColor::White) return (targetX == p->gridX && targetY == p->gridY - 1);
            else return (targetX == p->gridX && targetY == p->gridY + 1);
            
        case PType::Rook:
            // Ладья: только по горизонтали или вертикали
            return (dx == 0 || dy == 0);
            
        case PType::Bishop:
            // Слон: только по диагонали
            return (dx == dy);
            
        case PType::Knight:
            // Конь: буквой Г
            return (dx * dy == 2);
            
        case PType::Queen:
            // Ферзь: ладья + слон
            return (dx == 0 || dy == 0 || dx == dy);
            
        case PType::King:
            // Король: на 1 клетку в любую сторону
            return (dx <= 1 && dy <= 1);
    }
    return false;
}

void onBoardClick(Event* ev) {
    if (!selectedPiece) return;

    TouchEvent* te = static_cast<TouchEvent*>(ev);
    Point targetCell = pixelsToBoard(te->position);

    // Проверка границ и правил хода
    if (targetCell.x >= 0 && targetCell.x <= 7 && targetCell.y >= 0 && targetCell.y <= 7) {
        if (isMoveValid(selectedPiece, targetCell.x, targetCell.y)) {
            
            Vector2 newPos = boardToPixels(targetCell.x, targetCell.y);
            selectedPiece->addTween(Actor::TweenPosition(newPos), 300);
            
            selectedPiece->gridX = targetCell.x;
            selectedPiece->gridY = targetCell.y;

            selectedPiece->setSelected(false);
            selectedPiece = nullptr;
            
            // Смена хода
            currentTurn = (currentTurn == PColor::White) ? PColor::Black : PColor::White;
            logs::messageln("Move success. Turn: %s", (currentTurn == PColor::White ? "White" : "Black"));
            return;
        }
    }
    
    // Если ход неверный - просто снимаем выделение
    selectedPiece->setSelected(false);
    selectedPiece = nullptr;
}

void onPieceClick(Event* ev) {
    spPiece clickedPiece = safeSpCast<Piece>(ev->currentTarget);

    // Попытка съесть фигуру противника
    if (selectedPiece && selectedPiece->color != clickedPiece->color) {
        // Плавное исчезновение 
        spTween tween = clickedPiece->addTween(Actor::TweenAlpha(0), 300);
        
        // Когда анимация исчезновения закончится - удаляем объект со сцены
        tween->addEventListener(TweenEvent::DONE, [clickedPiece](Event*) {
            clickedPiece->detach();
        });
        
        // Плавное перемещение нашей фигуры на место съеденной
        Vector2 targetPos = clickedPiece->getPosition();
        selectedPiece->addTween(Actor::TweenPosition(targetPos), 300);
        
        // Обновляем логические координаты
        selectedPiece->gridX = clickedPiece->gridX;
        selectedPiece->gridY = clickedPiece->gridY;
        
        selectedPiece->setSelected(false);
        selectedPiece = nullptr;
        
        // Передаем ход
        currentTurn = (currentTurn == PColor::White) ? PColor::Black : PColor::White;
        return;
    }

    // Выбор своей фигуры (только если сейчас её ход)
    if (clickedPiece->color == currentTurn) {
        if (selectedPiece) selectedPiece->setSelected(false);
        selectedPiece = clickedPiece;
        selectedPiece->setSelected(true);
    }
}


void spawnPiece(PType type, PColor color, int x, int y, const std::string& resId) {
    spPiece p = new Piece(type, color, x, y, pieceRes[resId]);
    p->addEventListener(TouchEvent::CLICK, onPieceClick);
    // boardScale - масштаб всей доски. 
    // Умножаем его на 0.6, чтобы фигура занимала ~60% места в клетке
    p->setScale(boardScale * 0.6f); 
    
    p->setPosition(boardToPixels(x, y));
    getStage()->addChild(p);
    allPieces.push_back(p);
}

void initBoard() {
    // Расставляем пешки
    for (int i = 0; i < 8; ++i) {
        spawnPiece(PType::Pawn, PColor::Black, i, 1, "b_pawn");
        spawnPiece(PType::Pawn, PColor::White, i, 6, "w_pawn");
    }
    // Черные фигуры (Y = 0)
    spawnPiece(PType::Rook,   PColor::Black, 0, 0, "b_castle");
    spawnPiece(PType::Knight, PColor::Black, 1, 0, "b_knight");
    spawnPiece(PType::Bishop, PColor::Black, 2, 0, "b_bishop");
    spawnPiece(PType::Queen,  PColor::Black, 3, 0, "b_queen");
    spawnPiece(PType::King,   PColor::Black, 4, 0, "b_king");
    spawnPiece(PType::Bishop, PColor::Black, 5, 0, "b_bishop");
    spawnPiece(PType::Knight, PColor::Black, 6, 0, "b_knight");
    spawnPiece(PType::Rook,   PColor::Black, 7, 0, "b_castle");

    // Белые фигуры (Y = 7)
    spawnPiece(PType::Rook,   PColor::White, 0, 7, "w_castle");
    spawnPiece(PType::Knight, PColor::White, 1, 7, "w_knight");
    spawnPiece(PType::Bishop, PColor::White, 2, 7, "w_bishop");
    spawnPiece(PType::Queen,  PColor::White, 3, 7, "w_queen");
    spawnPiece(PType::King,   PColor::White, 4, 7, "w_king");
    spawnPiece(PType::Bishop, PColor::White, 5, 7, "w_bishop");
    spawnPiece(PType::Knight, PColor::White, 6, 7, "w_knight");
    spawnPiece(PType::Rook,   PColor::White, 7, 7, "w_castle");
}

void run() {
    core::init_desc desc;
    desc.title = "Chess Game";
    desc.w = 800; desc.h = 600;
    core::init(&desc);

    // Загружаем всё один раз
    pieceRes["w_pawn"] = loadManual("w_pawn.png");
    pieceRes["b_pawn"] = loadManual("b_pawn.png");
    pieceRes["w_castle"] = loadManual("w_castle.png");
    pieceRes["b_castle"] = loadManual("b_castle.png");
    pieceRes["w_knight"] = loadManual("w_knight.png");
    pieceRes["b_knight"] = loadManual("b_knight.png");
    pieceRes["w_bishop"] = loadManual("w_bishop.png");
    pieceRes["b_bishop"] = loadManual("b_bishop.png");
    pieceRes["w_queen"] = loadManual("w_queen.png");
    pieceRes["b_queen"] = loadManual("b_queen.png");
    pieceRes["w_king"] = loadManual("w_king.png");
    pieceRes["b_king"] = loadManual("b_king.png");

    spResAnim deckRes = loadManual("deck.png");

    Stage::instance = new Stage(true);
    Point displaySize = core::getDisplaySize();
    getStage()->setSize(displaySize);

    // Отрисовка доски
    spSprite board = new Sprite();
    board->setResAnim(deckRes.get());
    board->setAnchor(0.5f, 0.5f);
    board->setPosition(400, 300);
    boardScale = (displaySize.y * 0.9f) / board->getHeight();
    board->setScale(boardScale);
     board->setTouchEnabled(true); 
    board->addEventListener(TouchEvent::CLICK, onBoardClick);
    getStage()->addChild(board);

    // Расстановка фигур
    initBoard();

    while (1) {
        int done = core::update();
        if (done) break;
        getStage()->update();
        if (core::beginRendering()) {
           getStage()->render(Color(32, 32, 32, 255), Rect(0, 0, displaySize.x, displaySize.y));
           core::swapDisplayBuffers();
        }
    }
    core::release();
}

int main(int argc, char* argv[]) { run(); return 0; }
