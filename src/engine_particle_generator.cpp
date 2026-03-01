#include "engine_particle_generator.hpp"
#include <cstdlib>

#define PARTICLE_COUNT 50
#define PARTICLE_LIFETIME_MS 30
#define PARTICLE_VELOCITY 200.0f

using namespace blit;

// small helper to return a random angle in [-spread/2, spread/2]
static float random_angle(float spread) {
  return (static_cast<float>(rand()) / RAND_MAX - 0.5f) * spread;
}

EngineParticleGenerator::EngineParticleGenerator()
  : ParticleGenerator(PARTICLE_COUNT, PARTICLE_LIFETIME_MS, []() -> Particle* { return nullptr; }) {
  force = Vec2(0, 0);
}

void EngineParticleGenerator::update(uint32_t time_ms, const Vec2 ship_pos) {
  this->generate = [ship_pos]() -> Particle* {
    Vec2 particle_pos = ship_pos + Vec2(4, 4);

    Vec2 particle_vel = Vec2(0, 1.0f);
    float angle = random_angle(0.6f);
    particle_vel.rotate(angle);

    float mag = PARTICLE_VELOCITY + static_cast<float>(rand() % 30);
    particle_vel = particle_vel * mag;

    return new Particle(particle_pos, particle_vel);
  };

  ParticleGenerator::update(time_ms);
}
