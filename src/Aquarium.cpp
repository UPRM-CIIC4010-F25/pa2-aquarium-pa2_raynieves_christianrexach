#include "Aquarium.h"
#include <cstdlib>
PlayerCreature* g_player = nullptr;

string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::GoldFish:
            return "GoldFish";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {
    baseSpeed = speed;
}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
    this->updatePowerUp();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0 && isDebuffed) {
        ofSetColor(ofColor::mediumVioletRed); // Flash slightly purple if in damage debounce whilst debuffed
    }
    else if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x- m_sprite->getWidth() / 2.0f, m_y- m_sprite->getHeight() / 2.0f);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

void PlayerCreature::powerUp(std::shared_ptr<GameSprite> goldSprite) {
    isPoweredUp = true;
    powerUpTimer = 60 * 10; // 10 seconds at 60fps
    isDebuffed = false;
    debuffTimer = 0;

    m_sprite = goldSprite;
    m_speed = baseSpeed * 2.5f;
}

void PlayerCreature::updatePowerUp() {
    if (isPoweredUp) {
        --powerUpTimer;
        if (powerUpTimer <= 0) {
            isPoweredUp = false;
            m_speed = baseSpeed;
            m_sprite = std::make_shared<GameSprite>("base-fish.png", 100, 100); // revert to normal
        }
    }
    
    if (isDebuffed) {
        --debuffTimer;

        if (debuffTimer <= 0) {
            isDebuffed = false;

            if (isPoweredUp)
                m_speed = baseSpeed * 2.5f;
            else
                m_speed = baseSpeed;
        }
    }
}
    
// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x- m_sprite->getWidth() / 2.0f, m_y- m_sprite->getHeight() / 2.0f);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(m_x- m_sprite->getWidth() / 2.0f, m_y- m_sprite->getHeight() / 2.0f);
}

void GoldFish::move() {
    // Gold fish moves faster
    m_x += m_dx * (m_speed * 1.25); // Moves at 1.25 speed
    m_y += m_dy * (m_speed * 1.25);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}
void GoldFish::draw() const {
    ofLogVerbose() << "GoldFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(m_x- m_sprite->getWidth() / 2.0f, m_y- m_sprite->getHeight() / 2.0f);
}

void LionFish::move() {
    if (g_player) {
        float dx = g_player->getX() - m_x;
        float dy = g_player->getY() - m_y;
        float len = sqrt(dx*dx + dy*dy);
        if (len > 0) {
            dx /= len;
            dy /= len;
        }
        m_dx = dx;
        m_dy = dy;
    }

    m_x += m_dx * (m_speed * 0.8); // slightly faster than before
    m_y += m_dy * (m_speed * 0.8);

    m_sprite->setFlipped(m_dx < 0);
    bounce();
}
void LionFish::draw() const {
    ofLogVerbose() << "LionFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(m_x- m_sprite->getWidth() / 2.0f, m_y- m_sprite->getHeight() / 2.0f);
}

// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_gold_fish = std::make_shared<GameSprite>("goldFish.png", 110, 55);
    this->m_gold_fish_npc = std::make_shared<GameSprite>("goldFish.png", 60, 40);
    this->m_lion_fish = std::make_shared<GameSprite>("lionFish.png",100,100);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);

        case AquariumCreatureType::GoldFish:
            return std::make_shared<GameSprite>(*this->m_gold_fish);
        
        case AquariumCreatureType::GoldFishNPC:
            return std::make_shared<GameSprite>(*this->m_gold_fish_npc);

        case AquariumCreatureType::LionFish:
            return std::make_shared<GameSprite>(*this->m_lion_fish);

        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }

    //NPC Collision//

    for (size_t i = 0; i < m_creatures.size(); ++i) {
        auto npc1 = m_creatures[i];

        // Skip player
        if (std::dynamic_pointer_cast<PlayerCreature>(npc1)) continue;

        for (size_t j = i + 1; j < m_creatures.size(); ++j) {
            auto npc2 = m_creatures[j];
            if (std::dynamic_pointer_cast<PlayerCreature>(npc2)) continue;

            // Check bounding box intersection to push apart
            if (npc1->getBoundingBox().intersects(npc2->getBoundingBox())) {
                npc1->bounceFrom(npc2);
                npc2->bounceFrom(npc1);
            }
        }
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::GoldFish:
            this->addCreature(std::make_shared<GoldFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::GoldFishNPC)));
            break;
        case AquariumCreatureType::LionFish:
            this->addCreature(std::make_shared<LionFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::LionFish)));
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");

    // lets NOT make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);

    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
    // small random chance for GoldFish to appear
    if (ofRandom(1.0f) < 0.05f) { // 5% chance
        this->SpawnCreature(AquariumCreatureType::GoldFish);
    }

    // Increase LionFish spawn rate based on level:
    float lionChance = 0.05f + (this->currentLevel * 0.02f); // starts 5%, increases 2% per level
    lionChance = std::min(lionChance, 0.60f); // cap at 60%

    if (ofRandom(1.0f) < lionChance) {
        this->SpawnCreature(AquariumCreatureType::LionFish);
    }
}



// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();

    if (this->updateControl.tick()) {
        auto event = DetectAquariumCollisions(this->m_aquarium, this->m_player);

        if (event && event->isCollisionEvent() && event->creatureB) {
            if (this->m_player->getPower() < event->creatureB->getValue()) {
                this->m_player->loseLife(3 * 60);
                this->m_player->setDirection(-this->m_player->getDx(), -this->m_player->getDy());
                this->m_player->move();
                event->creatureB->bounceFrom(this->m_player);
            } else {   

                float fx = event->creatureB->getX() + event->creatureB->getBoundingBox().getWidth() / 2;
                float fy = event->creatureB->getY() + event->creatureB->getBoundingBox().getHeight() / 2;

                // spawn small cluster of squares
                for (int i = 0; i < 6; ++i) {
                    float offsetX = ofRandom(-8, 8);
                    float offsetY = ofRandom(-8, 8);
                    redSquares.push_back(glm::vec3(fx + offsetX, fy + offsetY, 30));
                }

                AquariumCreatureType type = std::static_pointer_cast<NPCreature>(event->creatureB)->GetType();

                // --- special case: GoldFish gives power-up ---
                if (type == AquariumCreatureType::GoldFish) {
                    auto goldSprite = this->m_aquarium->GetSprite(AquariumCreatureType::GoldFish);
                    this->m_player->powerUp(goldSprite);
                    this->m_aquarium->removeCreature(event->creatureB);
                }
                else if (type == AquariumCreatureType::LionFish) {
                    this->m_player->applyDebuff(5 * 60, 0.5f); // 5 seconds 
                    this->m_player->loseLife(2 * 60);
                    this->m_player->setDirection(-this->m_player->getDx(), -this->m_player->getDy());
                }
                else{
                    this->m_aquarium->removeCreature(event->creatureB);
                }

                this->m_player->addToScore(1, event->creatureB->getValue());
                if (this->m_player->getScore() % 25 == 0) {
                    this->m_player->increasePower(1);
                }
            }
        }
        this->m_aquarium->update();

        // Add red squares following player when debuffed
        if (m_player->isDebuffed) {
            for (int i = 0; i < 3; ++i) {
                float offsetX = ofRandom(-15, 15);
                float offsetY = ofRandom(-15, 15);
                redSquares.push_back(glm::vec3(m_player->getX() + offsetX, m_player->getY() + offsetY, 30));
            }
        }
        // Update red square lifetimes
        for (auto& sq : redSquares) {
            sq.z -= 1; // reduce lifetime
            }

        // Remove expired squares
        redSquares.erase(
            std::remove_if(redSquares.begin(), redSquares.end(), [](const glm::vec3& sq) {
                return sq.z <= 0;
            }),

            redSquares.end()
        );
    }
        // Game Over update
    if (this->m_player->getLives() <= 0) {

        auto gameOverEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, nullptr, nullptr);
        this->SetLastEvent(gameOverEvent); // store it so ofApp can detect it
        return; // early return to stop further updates
    }
}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

    ofSetColor(ofColor::red);

    for (const auto& sq : redSquares) {
        float alpha = ofMap(sq.z, 0, 15, 0, 64, true); // starts at 1/4 opacity and goes to 0 in 15f.
        ofSetColor(255, 0, 0, alpha);
        float size = 10.0f;
        ofDrawRectangle(sq.x - size / 2, sq.y - size / 2, size, size);
    }
    ofSetColor(ofColor::white);
}


void AquariumGameScene::paintAquariumHUD(){

    hudFont.load("Monaco.ttf", 32, true, true);
    float panelHeight = ofGetWindowHeight() - 40;
    float panelWidth = ofGetWindowWidth() - 200;
    hudFont.drawString("Score:" + std::to_string(this->m_player->getScore()), panelWidth*0.4, panelHeight);
    hudFont.drawString("Power:" + std::to_string(this->m_player->getPower()), panelWidth*0.70, panelHeight);
    hudFont.drawString("Lives:", panelWidth*0.01, panelHeight);

    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth*0.01 + 180 + i * 40, panelHeight - 15, 15);
    }
    // Added Level Counter
    int currentLevel = this->m_aquarium->getCurrentLevel(); 
    ofSetColor(ofColor::white);
    hudFont.drawString("Level:" + std::to_string(currentLevel), panelWidth, panelHeight);

    ofSetColor(ofColor::white); // Reset color

}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
