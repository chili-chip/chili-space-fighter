#include "story.hpp"
#include "audio.hpp"

namespace story {

namespace {
    constexpr int STATIC_ROW = 10;
    constexpr uint32_t INPUT_DELAY_MS = 250;  // so a player still mashing fire doesn't skip a page unread
    constexpr uint32_t CHAR_MS = 28;          // text speed
    constexpr uint32_t MOUTH_MS = 110;        // talking animation speed

    // the frame each character's face opens its mouth with (see docs/story/characters.md)
    int talk_frame(int row, int frame) {
        static const int8_t TALK[11] = {1, -1, 1, 1, 1, 3, 1, 2, 1, 3, 0};
        if (row == 1) return (frame + 1) % 4;  // AUX-7 / Kit: the eye moves
        int t = TALK[row];
        return t == frame ? 0 : t;
    }

    // box along the bottom: 16x16 face on the left, name and three lines of text on the right
    constexpr int BOX_Y = 89;
    constexpr int TEXT_X = 22;
    constexpr int TEXT_W = 104;
    constexpr int LINE_H = 8;

    bool just_pressed(uint32_t b) { return buttons.pressed & b; }
}

void Dialogue::start(const Scene &s) {
    scene = s;
    index = 0;
    open_speech();
}

int Dialogue::page_chars() const {
    if (!active() || scene.lines[index].row == CARD) return 0;
    int n = 0;
    for (int i = page * LINES_PER_PAGE; i < (int)lines.size() && i < (page + 1) * LINES_PER_PAGE; i++) n += lines[i].size();
    return n;
}

void Dialogue::open_page() {
    shown_ms = 0;
    done_ms = 0;
    revealed = 0;
}

void Dialogue::open_speech() {
    page = 0;
    open_page();
    lines.clear();
    if (!active()) return;
    const Speech &sp = scene.lines[index];
    std::string text = sp.row == CARD ? std::string(sp.text) : screen.wrap_text(sp.text, TEXT_W, minimal_font);
    size_t from = 0;
    while (from <= text.size()) {
        size_t nl = text.find('\n', from);
        if (nl == std::string::npos) nl = text.size();
        lines.push_back(text.substr(from, nl - from));
        from = nl + 1;
    }
}

void Dialogue::update(uint32_t dt) {
    if (!active()) return;
    shown_ms += dt;
    const Speech &sp = scene.lines[index];

    if (talking()) {
        // reveal the letters that are due, with a blip every other letter
        int due = std::min(page_chars(), (int)(shown_ms / CHAR_MS));
        if (due / 2 > revealed / 2) audio::talk(sp.row);
        revealed = due;
    } else {
        done_ms += dt;
    }

    if (shown_ms < INPUT_DELAY_MS) return;
    if (just_pressed(Button::B)) {
        audio::play(audio::Sfx::MenuBack);
        scene = Scene{nullptr, 0};
        return;
    }
    if (!just_pressed(Button::A)) return;
    if (talking()) {
        revealed = page_chars();  // show the rest of the page at once
        return;
    }
    if (done_ms < INPUT_DELAY_MS) return;
    audio::play(audio::Sfx::MenuMove);
    // a card is one page however many lines it has
    if (sp.row != CARD && page + 1 < pages()) {
        page++;
        open_page();
    } else {
        index++;
        open_speech();
    }
}

void Dialogue::draw_card(uint32_t time) {
    // "TURBO" is drawn as the show's own logo, as on the title screen
    Surface *logo = sheets.logo;
    auto height = [&](const std::string &l) { return l == "TURBO" ? logo->bounds.h + 3 : 11; };
    int h = 8;
    for (auto &l : lines) h += height(l);
    int y = (SCREEN_H - h) / 2;
    screen.pen = Pen(0, 0, 0, 190);
    screen.rectangle(Rect(0, y, SCREEN_W, h));
    y += 5;
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i] == "TURBO") {
            screen.blit(logo, Rect(Point(0, 0), logo->bounds), Point((SCREEN_W - logo->bounds.w) / 2, y));
        } else {
            bool last = i + 1 == lines.size();
            shadow_text(lines[i], minimal_font, Point(SCREEN_W / 2, y), last ? rainbow(time * 0.006f) : Pen(255, 255, 255),
                        TextAlign::top_center);
        }
        y += height(lines[i]);
    }
}

void Dialogue::draw(uint32_t time) {
    if (!active()) return;
    const Speech &sp = scene.lines[index];
    if (sp.row == CARD) {
        draw_card(time);
        return;
    }

    screen.pen = Pen(6, 4, 30, 225);
    screen.rectangle(Rect(0, BOX_Y, SCREEN_W, SCREEN_H - BOX_Y));
    screen.pen = Pen(90, 80, 200);
    screen.h_span(Point(0, BOX_Y), SCREEN_W);

    // face, drawn at 2x; it talks while the text appears. The Static's frame flickers through its five frames.
    int frame = sp.frame;
    bool mouth_open = talking() && (shown_ms / MOUTH_MS) % 2;
    if (sp.row == STATIC_ROW) frame = (int)((time / 90) % 5);
    else if (mouth_open) frame = talk_frame(sp.row, sp.frame);
    Rect face(2, BOX_Y + 4 - (mouth_open ? 1 : 0), 16, 16);
    screen.pen = Pen(90, 80, 200);
    screen.rectangle(Rect(face.x - 1, face.y - 1, face.w + 2, face.h + 2));
    screen.pen = Pen(20, 16, 50);
    screen.rectangle(face);
    screen.stretch_blit(sheets.characters, Rect(1 + frame * 8, sp.row * 8, 8, 8), face);

    int y = BOX_Y + 3;
    if (sp.name) shadow_text(sp.name, minimal_font, Point(TEXT_X, y), Pen(255, 210, 90));
    y += LINE_H + 1;
    int left = revealed;
    for (int i = page * LINES_PER_PAGE; i < (int)lines.size() && i < (page + 1) * LINES_PER_PAGE && left > 0; i++, y += LINE_H) {
        int n = std::min(left, (int)lines[i].size());
        shadow_text(std::string_view(lines[i]).substr(0, n), minimal_font, Point(TEXT_X, y), Pen(230, 230, 245));
        left -= n;
    }

    // a small arrow next to the name, bobbing, when A goes on
    if (!talking() && done_ms >= INPUT_DELAY_MS) {
        Point a(SCREEN_W - 6, BOX_Y + 4 + (int)((time / 250) % 2));
        screen.pen = Pen(255, 230, 120);
        screen.h_span(a + Point(-2, 0), 5);
        screen.h_span(a + Point(-1, 1), 3);
        screen.pixel(a + Point(0, 2));
    }
}

}  // namespace story
