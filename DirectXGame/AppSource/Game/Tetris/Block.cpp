#include "Block.h"

void Block::Draw(Render* render) {
	render->Draw(model.get());
}
