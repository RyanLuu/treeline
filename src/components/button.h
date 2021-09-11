#pragma once

#include "src/assets.h"
#include "src/keyboard.h"

struct CButton {
    Key key;
    AssetId activeSprite;
    AssetId idleSprite;
};

