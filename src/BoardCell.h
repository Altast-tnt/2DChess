#pragma once
#include "oxygine-framework.h"

using namespace oxygine;

class BoardCell : public ColorRectSprite {
public:
    int gx, gy;
    BoardCell(int x, int y, float size) : gx(x), gy(y) {
        setSize(size, size);
        setAnchor(0.5f, 0.5f);
        setAlpha(1); 
        // убирает артефакты прозрачности .png
        setBlendMode(blend_alpha);
        setTouchEnabled(true);
    }
};

DECLARE_SMART(BoardCell, spBoardCell);
