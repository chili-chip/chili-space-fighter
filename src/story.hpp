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
constexpr int WAVES_PER_CHAPTER = 10;  // the last wave of a chapter is its boss
constexpr int RADIO_WAVE = 5;          // wave of the chapter where the radio scene plays

extern const Scene intro, ending, free_lanes, free_lanes_game_over;
extern const Chapter chapters[CHAPTERS];

// Splits a speech into pages of at most `lines` lines wrapped to `width`. A page ends at the end of a
// sentence. A sentence too long for one page is split after a comma, and only a part still too long
// is split between words.
std::vector<std::vector<std::string>> paginate(const std::string &text, int width, int lines);

// Plays a scene in a text box along the bottom of the screen. The text appears letter by letter while
// the face talks and blips; A shows the whole page, then the next one. B skips the scene.
// When someone new comes on comms, and when the scene ends, the face is static and the name arrives
// (or leaves) as scrambled letters.
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
    uint32_t shown_ms = 0;   // since the page appeared
    uint32_t done_ms = 0;    // since the whole page was shown
    int revealed = 0;        // letters of the page shown so far
    int32_t static_ms = 0;   // > 0 while the comms crackle (someone joins, or the scene ends)
    bool closing = false;    // the last speech is done, the static plays out before the box closes
    std::vector<std::vector<std::string>> pages;  // the current speech: pages of wrapped lines

    void open_speech();
    void open_page();
    void crackle();
    int page_chars() const;
    bool talking() const { return revealed < page_chars(); }
    void draw_card(uint32_t time);
    void draw_static(const Rect &face);
};

}  // namespace story
