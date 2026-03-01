#pragma once

#include "engine/particle.hpp"

namespace blit {

class EngineParticleGenerator : public ParticleGenerator {
public:
  EngineParticleGenerator();

  // update with current ship position
  void update(uint32_t time_ms, const Vec2 ship_pos);
};

}
