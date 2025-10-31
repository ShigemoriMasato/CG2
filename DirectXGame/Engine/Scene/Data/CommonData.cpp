#include "CommonData.h"

CommonData::CommonData() {
	keyManager_ = std::make_shared<KeyManager>();
	keyManager_->Initialize();
}

CommonData::~CommonData() {

}
