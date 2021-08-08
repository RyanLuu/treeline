#include <optional>

struct CSprite {
    unsigned int textureId;
    std::optional<unsigned int> width;
    std::optional<unsigned int> height;
};
