#pragma once
#include "oxygine-framework.h"
#include <string>

using namespace oxygine;

// Типы и цвета
enum class PType { Pawn, Rook, Knight, Bishop, Queen, King };
enum class PColor { White, Black };

class Piece : public Sprite {
public:
    PType type;
    PColor color;
    int gridX, gridY;

    Piece(PType t, PColor c, int x, int y, spResAnim res) 
        : type(t), color(c), gridX(x), gridY(y) 
    {
        setResAnim(res.get());
        setAnchor(0.5f, 0.5f); 
    }
    
    void setSelected(bool s) {
        if (s) setColor(Color::Yellow); 
        else setColor(Color::White);    
    }
};

// Умный указатель для фигур
DECLARE_SMART(Piece, spPiece);
