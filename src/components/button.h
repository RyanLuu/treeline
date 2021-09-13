#pragma once

#include "assets/assets.h"
#include "io/keyboard.h"

struct CButton {
    Key key;
    assets::AssetId activeSprite;
    assets::AssetId idleSprite;
};

