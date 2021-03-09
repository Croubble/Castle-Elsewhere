
#include "Math.h"
namespace textureAssets {
enum FLOOR
{
    Exit,
    Floor,
    GrillCot,
    GrillHot,
    Outline,
    Start,
    Target,
    ZBlack
};

glm::vec4* FLOOR_positions(Memory* memory);
enum PIECE
{
    Crate,
    Crumble,
    CursedCrate,
    CursedPlayer,
    CursedPullCrate,
    Empty,
    Player,
    PullCrate,
    StopD,
    StopL,
    StopR,
    StopU,
    Wall,
    WallAlt
};

glm::vec4* PIECE_positions(Memory* memory);
enum SYMBOLS
{
    Circle,
    HalfCircle,
    Plus,
    Square,
    Triangle
};

glm::vec4* SYMBOLS_positions(Memory* memory);
enum UI
{
    ButtonCenter,
    ButtonLeftHalf,
    ButtonRightHalf,
    DownArrow,
    LeftArrow,
    RightArrow,
    UpArrow
};

glm::vec4* UI_positions(Memory* memory);
}