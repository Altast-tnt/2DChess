#pragma once
#include "oxygine-framework.h"
#include "Piece.h"
#include "BoardCell.h"
#include <map>
#include <vector>
#include <string>

using namespace oxygine;

class ChessController {
public:
    static ChessController& instance() {
        static ChessController inst;
        return inst;
    }

    void run();

private:
    ChessController() : currentTurn(PColor::White), selectedPiece(nullptr) {}
    // размер одной клетки относительно оригинального изображения deck.png
    const float STEP = 57.25f;   
    // координаты смещения для удобства отрисовки grid
    // изначальная картинка имеет рамки
    const float START_X = 51.0f;  
    const float START_Y = 51.0f;  

    // создание базовых списков и элементов для регулирования процесса игры
    std::map<std::string, spResAnim> pieceRes;
    std::vector<spPiece> allPieces; 
    spSprite board;
    spBoardCell grid[8][8];

    spPiece selectedPiece;
    PColor currentTurn;
  
    // инициализация всех объектов и их отчистка
    void init();
    void cleanup();
    
    // загрузка картинок через файлы в data/images
    spResAnim loadManual(const std::string& fileName);
    spPiece getPieceAt(int x, int y);
    bool isMoveValid(spPiece p, int targetX, int targetY, bool isTaking);
    void removePieceAt(int x, int y);
    void finalizeMove(spPiece p, int tx, int ty);
    // метод для расстановки фигур при инициализации доски
    void spawnPiece(PType type, PColor color, int x, int y, const std::string& resId);
    void initGrid();
    void initPieces();
    // обработчики нажатий на клетку доски и на фигуру соответственно
    void onCellClick(Event* ev);
    void onPieceClick(Event* ev);
};
