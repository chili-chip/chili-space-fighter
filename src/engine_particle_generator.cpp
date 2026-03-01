#include "engine_particle_generator.hpp"
#include <cstdlib>

#define PARTICLE_COUNT 50
#define PARTICLE_LIFETIME_MS 20
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

std::function<Particle*(void)> EngineParticleGenerator::forward_thrust(const Vec2 &ship_pos) {
  return [ship_pos]() -> Particle* {
    Vec2 particle_pos = ship_pos + Vec2(4, 4);

    Vec2 particle_vel = Vec2(0, 1.0f);
    float angle = random_angle(0.6f);
    particle_vel.rotate(angle);

    float mag = PARTICLE_VELOCITY + static_cast<float>(rand() % 30);
    particle_vel = particle_vel * mag;

    return new Particle(particle_pos, particle_vel);
  };
}

void EngineParticleGenerator::update(uint32_t time_ms, const Vec2 ship_pos, const Vec2 ship_movement_vector) {
  if (ship_movement_vector.y < 0) {
    generate = forward_thrust(ship_pos);
  } else {
    generate = []() -> Particle* { return new Particle(Vec2(-10,0), Vec2(0,0)); };
  }
  ParticleGenerator::update(time_ms);
}