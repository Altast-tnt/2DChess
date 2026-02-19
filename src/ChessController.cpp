#include "ChessController.h"

spResAnim ChessController::loadManual(const std::string& fileName) {
    file::buffer fb;
    if (!file::read("data/images/" + fileName, fb)) return nullptr;
    Image img; img.init(fb);
    spNativeTexture texture = IVideoDriver::instance->createTexture();
    texture->init(img.lock());
    spResAnim rs = new ResAnim();
    rs->init(texture, img.getSize());
    return rs;
}

spPiece ChessController::getPieceAt(int x, int y) {
    for (auto& p : allPieces) {
        if (p->gridX == x && p->gridY == y) return p;
    }
    return nullptr;
}

bool ChessController::isMoveValid(spPiece p, int targetX, int targetY, bool isTaking) {
    int dx = std::abs(targetX - p->gridX);
    int dy = std::abs(targetY - p->gridY);
    int direction = (p->color == PColor::White) ? -1 : 1; 

    spPiece targetPiece = getPieceAt(targetX, targetY);
    if (targetPiece.get() && targetPiece->color == p->color) return false;

    switch (p->type) {
        case PType::Pawn:
            if (isTaking) return (dx == 1 && targetY == p->gridY + direction);
            else {
                if (targetPiece.get()) return false;
                if (dx == 0 && targetY == p->gridY + direction) return true;
                if (dx == 0 && targetY == p->gridY + (direction * 2)) {
                    if (getPieceAt(p->gridX, p->gridY + direction).get()) return false;
                    if (p->color == PColor::White && p->gridY == 6) return true;
                    if (p->color == PColor::Black && p->gridY == 1) return true;
                }
            }
            return false;
        case PType::Rook:   return (dx == 0 || dy == 0);
        case PType::Bishop: return (dx == dy);
        case PType::Knight: return (dx * dy == 2);
        case PType::Queen:  return (dx == 0 || dy == 0 || dx == dy);
        case PType::King:   return (dx <= 1 && dy <= 1);
    }
    return false;
}

void ChessController::removePieceAt(int x, int y) {
    for (auto it = allPieces.begin(); it != allPieces.end(); ) {
        spPiece p = *it;
        if (p->gridX == x && p->gridY == y) {
            p->addTween(Actor::TweenAlpha(0), 300)->addEventListener(TweenEvent::DONE, [p](Event*){ 
                p->detach(); 
            });
            it = allPieces.erase(it);
        } else ++it;
    }
}

void ChessController::finalizeMove(spPiece p, int tx, int ty) {
    p->addTween(Actor::TweenPosition(grid[tx][ty]->getPosition()), 300);
    p->gridX = tx;
    p->gridY = ty;
    p->setSelected(false);
    selectedPiece = nullptr;
    currentTurn = (currentTurn == PColor::White) ? PColor::Black : PColor::White;
}

void ChessController::onCellClick(Event* ev) {
    spBoardCell cell = safeSpCast<BoardCell>(ev->currentTarget);
    if (!selectedPiece.get()) return;

    spPiece target = getPieceAt(cell->gx, cell->gy);
    if (target.get()) {
        if (target->color != selectedPiece->color && isMoveValid(selectedPiece, cell->gx, cell->gy, true)) {
            removePieceAt(cell->gx, cell->gy);
            finalizeMove(selectedPiece, cell->gx, cell->gy);
        }
    } else if (isMoveValid(selectedPiece, cell->gx, cell->gy, false)) {
        finalizeMove(selectedPiece, cell->gx, cell->gy);
    }
}

void ChessController::onPieceClick(Event* ev) {
    spPiece clickedPiece = safeSpCast<Piece>(ev->currentTarget);
    // не передаем клик родительскому элементу (доске)
    ev->stopPropagation();

    if (selectedPiece.get() && selectedPiece->color != clickedPiece->color) {
        if (isMoveValid(selectedPiece, clickedPiece->gridX, clickedPiece->gridY, true)) {
            int tx = clickedPiece->gridX;
            int ty = clickedPiece->gridY;
            removePieceAt(tx, ty);
            finalizeMove(selectedPiece, tx, ty);
        }
        return;
    }

    if (clickedPiece->color == currentTurn) {
        if (selectedPiece.get()) selectedPiece->setSelected(false);
        selectedPiece = clickedPiece;
        selectedPiece->setSelected(true);
    }
}

void ChessController::initGrid() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            spBoardCell c = new BoardCell(x, y, STEP);
            c->setPosition(START_X + x * STEP + STEP / 2.0f, START_Y + y * STEP + STEP / 2.0f);
            c->addEventListener(TouchEvent::CLICK, CLOSURE(this, &ChessController::onCellClick));
            board->addChild(c);
            grid[x][y] = c;
        }
    }
}

void ChessController::spawnPiece(PType type, PColor color, int x, int y, const std::string& resId) {
    spPiece p = new Piece(type, color, x, y, pieceRes[resId]);
    p->addEventListener(TouchEvent::CLICK, CLOSURE(this, &ChessController::onPieceClick));
    p->setPriority(10);
    p->setPosition(grid[x][y]->getPosition());
    p->setScale(0.65f); 
    board->addChild(p);
    allPieces.push_back(p);
}

void ChessController::initPieces() {
    for (int i = 0; i < 8; ++i) {
        spawnPiece(PType::Pawn, PColor::Black, i, 1, "b_pawn");
        spawnPiece(PType::Pawn, PColor::White, i, 6, "w_pawn");
    }
    spawnPiece(PType::Rook, PColor::Black, 0, 0, "b_castle");
    spawnPiece(PType::Knight, PColor::Black, 1, 0, "b_knight");
    spawnPiece(PType::Bishop, PColor::Black, 2, 0, "b_bishop");
    spawnPiece(PType::Queen, PColor::Black, 3, 0, "b_queen");
    spawnPiece(PType::King, PColor::Black, 4, 0, "b_king");
    spawnPiece(PType::Bishop, PColor::Black, 5, 0, "b_bishop");
    spawnPiece(PType::Knight, PColor::Black, 6, 0, "b_knight");
    spawnPiece(PType::Rook, PColor::Black, 7, 0, "b_castle");

    spawnPiece(PType::Rook, PColor::White, 0, 7, "w_castle");
    spawnPiece(PType::Knight, PColor::White, 1, 7, "w_knight");
    spawnPiece(PType::Bishop, PColor::White, 2, 7, "w_bishop");
    spawnPiece(PType::Queen, PColor::White, 3, 7, "w_queen");
    spawnPiece(PType::King, PColor::White, 4, 7, "w_king");
    spawnPiece(PType::Bishop, PColor::White, 5, 7, "w_bishop");
    spawnPiece(PType::Knight, PColor::White, 6, 7, "w_knight");
    spawnPiece(PType::Rook, PColor::White, 7, 7, "w_castle");
}

void ChessController::init() {
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

    Point ds = core::getDisplaySize();
    getStage()->setSize(ds);

    board = new Sprite();
    board->setResAnim(deckRes.get());
    board->setAnchor(0.5f, 0.5f);
    board->setPosition(ds.x / 2.0f, ds.y / 2.0f);
    board->setScale((ds.y * 0.9f) / board->getHeight());
    getStage()->addChild(board);

    initGrid();
    initPieces();
}

void ChessController::run() {
    init();
    while (1) {
        if (core::update()) break;
        getStage()->update();
        if (core::beginRendering()) {
           getStage()->render(Color(32, 32, 32, 255), Rect(0, 0, core::getDisplaySize().x, core::getDisplaySize().y));
           core::swapDisplayBuffers();
        }
    }
    cleanup();
}

void ChessController::cleanup() {
    getStage()->removeChildren();
    allPieces.clear();
    pieceRes.clear();
    for (int y = 0; y < 8; ++y) for (int x = 0; x < 8; ++x) grid[x][y] = nullptr;
    selectedPiece = nullptr;
    board = nullptr;
}
