#pragma once

#include "32blit.hpp"
#include "engine_particle_generator.hpp"
#include "ship.hpp"
#include <cstdlib>

using namespace blit;


class Player : public Ship {

public:
    Player();
    //~Player();

    InputState get_input() override;
};
