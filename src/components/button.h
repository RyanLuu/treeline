#pragma once

#include "assets/assets.h"
#include "io/keyboard.h"

struct CButton {
    Key key;
    AssetId activeSprite;
    AssetId idleSprite;
};

