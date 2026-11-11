#pragma once

#include "common.hpp"
#include <string>
#include <vector>

// The Open Sky story. The dialogue itself lives in docs/story/script.md; tools/make_script.py turns
// it into the arrays below at build time (story_script.cpp in the build folder).
namespace story {

constexpr int CARD = -1;  // Speech::row for a full screen title card, text lines split by '\n'

struct Speech {
    const char *name;  // shown in the box, nullptr for the Static and for cards
    int8_t row;        // portrait row in assets/characters.png, or CARD
    int8_t frame;      // portrait frame 0 to 4
    const char *text;
};

struct Scene {
    const Speech *lines;
    int count;
};

struct Chapter {
    const char *card[3];  // "CHAPTER 1", title, place
    Scene start, radio, boss_warning, after_boss, game_over;
};

constexpr int CHAPTERS = 6;
constexpr int WAVES_PER_CHAPTER = 5;

extern const Scene intro, ending, free_lanes, free_lanes_game_over;
extern const Chapter chapters[CHAPTERS];

// Plays a scene in a text box along the bottom of the screen: A shows the next page, B skips the scene.
class Dialogue {
public:
    void start(const Scene &scene);
    bool active() const { return scene.lines && index < scene.count; }
    void update(uint32_t dt);
    void draw(uint32_t time);

private:
    static constexpr int LINES_PER_PAGE = 3;

    Scene scene{nullptr, 0};
    int index = 0;  // speech
    int page = 0;
    uint32_t shown_ms = 0;
    std::vector<std::string> lines;  // the current speech, wrapped

    void open_speech();
    int pages() const { return ((int)lines.size() + LINES_PER_PAGE - 1) / LINES_PER_PAGE; }
    void draw_card(uint32_t time);
};

}  // namespace story
