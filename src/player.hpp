#pragma once

#include "32blit.hpp"
#include "engine_particle_generator.hpp"
#include "ship.hpp"
#include <cstdlib>
#include "assets.hpp"

using namespace blit;


class Player : public Ship {

public:
    Player();
    //~Player();

    void move(Vec2 movement_vector, uint32_t dt) override;
    void draw() override;
    ShipControls get_ship_controls() override;
};
