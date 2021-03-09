#include "Memory.h"
#include "textureAssets.h"

glm::vec4* textureAssets::FLOOR_positions(Memory* memory) {
glm::vec4* result = (glm::vec4*) memory_alloc(memory,sizeof(glm::vec4) * 8);
 result[0] = glm::vec4(0.000977,0.000977,0.063477,0.063477);
 result[1] = glm::vec4(0.065430,0.000977,0.127930,0.063477);
 result[2] = glm::vec4(0.129883,0.000977,0.192383,0.063477);
 result[3] = glm::vec4(0.194336,0.000977,0.256836,0.063477);
 result[4] = glm::vec4(0.258789,0.000977,0.321289,0.063477);
 result[5] = glm::vec4(0.323242,0.000977,0.385742,0.063477);
 result[6] = glm::vec4(0.387695,0.000977,0.450195,0.063477);
 result[7] = glm::vec4(0.452148,0.000977,0.514648,0.063477);return result;
}

glm::vec4* textureAssets::PIECE_positions(Memory* memory) {
glm::vec4* result = (glm::vec4*) memory_alloc(memory,sizeof(glm::vec4) * 14);
 result[0] = glm::vec4(0.000977,0.000977,0.063477,0.063477);
 result[1] = glm::vec4(0.065430,0.000977,0.127930,0.063477);
 result[2] = glm::vec4(0.129883,0.000977,0.192383,0.063477);
 result[3] = glm::vec4(0.194336,0.000977,0.256836,0.063477);
 result[4] = glm::vec4(0.258789,0.000977,0.321289,0.063477);
 result[5] = glm::vec4(0.323242,0.000977,0.385742,0.063477);
 result[6] = glm::vec4(0.387695,0.000977,0.450195,0.063477);
 result[7] = glm::vec4(0.452148,0.000977,0.514648,0.063477);
 result[8] = glm::vec4(0.516602,0.000977,0.579102,0.063477);
 result[9] = glm::vec4(0.581055,0.000977,0.643555,0.063477);
 result[10] = glm::vec4(0.645508,0.000977,0.708008,0.063477);
 result[11] = glm::vec4(0.709961,0.000977,0.772461,0.063477);
 result[12] = glm::vec4(0.774414,0.000977,0.836914,0.063477);
 result[13] = glm::vec4(0.838867,0.000977,0.901367,0.063477);return result;
}

glm::vec4* textureAssets::SYMBOLS_positions(Memory* memory) {
glm::vec4* result = (glm::vec4*) memory_alloc(memory,sizeof(glm::vec4) * 5);
 result[0] = glm::vec4(0.000977,0.000977,0.063477,0.063477);
 result[1] = glm::vec4(0.065430,0.000977,0.127930,0.063477);
 result[2] = glm::vec4(0.129883,0.000977,0.192383,0.063477);
 result[3] = glm::vec4(0.194336,0.000977,0.256836,0.063477);
 result[4] = glm::vec4(0.258789,0.000977,0.321289,0.063477);return result;
}

glm::vec4* textureAssets::UI_positions(Memory* memory) {
glm::vec4* result = (glm::vec4*) memory_alloc(memory,sizeof(glm::vec4) * 7);
 result[0] = glm::vec4(0.000977,0.000977,0.032227,0.063477);
 result[1] = glm::vec4(0.034180,0.000977,0.065430,0.063477);
 result[2] = glm::vec4(0.067383,0.000977,0.098633,0.063477);
 result[3] = glm::vec4(0.100586,0.000977,0.163086,0.063477);
 result[4] = glm::vec4(0.165039,0.000977,0.227539,0.063477);
 result[5] = glm::vec4(0.229492,0.000977,0.291992,0.063477);
 result[6] = glm::vec4(0.293945,0.000977,0.356445,0.063477);return result;
}
