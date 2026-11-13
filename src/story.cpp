#include "story.hpp"
#include "audio.hpp"
#include <cstring>

namespace story {

namespace {
    constexpr int STATIC_ROW = 10;
    constexpr uint32_t INPUT_DELAY_MS = 250;  // so a player still mashing fire doesn't skip a page unread
    constexpr uint32_t CHAR_MS = 28;          // text speed
    constexpr uint32_t MOUTH_MS = 110;        // talking animation speed
    constexpr int32_t STATIC_MS = 420;        // comms crackle when someone joins or the scene ends
    constexpr int FRAMES = 5;                 // portrait frames per character

    // while the text appears the face goes round all its frames, then rests on frame 0 (mouth closed)
    int face_frame(const Speech &sp, bool talking, uint32_t shown_ms) {
        return talking ? (sp.frame + 1 + (int)(shown_ms / MOUTH_MS)) % FRAMES : 0;
    }

    void draw_face(const Speech &sp, int frame, const Rect &face) {
        screen.pen = Pen(90, 80, 200);
        screen.rectangle(Rect(face.x - 1, face.y - 1, face.w + 2, face.h + 2));
        screen.pen = Pen(20, 16, 50);
        screen.rectangle(face);
        screen.stretch_blit(sheets.characters, Rect(1 + frame * 8, sp.row * 8, 8, 8), face);
    }

    // snow in the avatar box
    void draw_static(const Rect &face) {
        for (int y = 0; y < face.h; y += 2) {
            for (int x = 0; x < face.w; x += 2) {
                uint8_t v = 30 + (blit::random() & 0xdf);
                screen.pen = Pen(v, v, std::min(255, v + 30));
                screen.rectangle(Rect(face.x + x, face.y + y, 2, 2));
            }
        }
    }

    // box along the bottom: 16x16 face on the left, name and three lines of text on the right
    constexpr int BOX_Y = 89;
    constexpr int TEXT_X = 22;
    constexpr int TEXT_W = 104;
    constexpr int LINE_H = 8;

    bool just_pressed(uint32_t b) { return buttons.pressed & b; }

    std::vector<std::string> split_lines(const std::string &text) {
        std::vector<std::string> out;
        size_t from = 0;
        while (from <= text.size()) {
            size_t nl = text.find('\n', from);
            if (nl == std::string::npos) nl = text.size();
            out.push_back(text.substr(from, nl - from));
            from = nl + 1;
        }
        return out;
    }

    // "One. Two, three." split after any of `ends` followed by a space -> "One.", "Two, three."
    std::vector<std::string> split_after(const std::string &text, const char *ends) {
        std::vector<std::string> out;
        size_t from = 0;
        for (size_t i = 0; i + 1 < text.size(); i++) {
            if (strchr(ends, text[i]) && text[i + 1] == ' ') {
                out.push_back(text.substr(from, i + 1 - from));
                from = i + 2;
            }
        }
        out.push_back(text.substr(from));
        return out;
    }

    bool same_speaker(const Speech &a, const Speech &b) {
        return a.row == b.row && (a.name == b.name || (a.name && b.name && !strcmp(a.name, b.name)));
    }
}

std::vector<std::vector<std::string>> paginate(const std::string &text, int width, int max_lines) {
    auto wrap = [&](const std::string &t) { return split_lines(screen.wrap_text(t, width, minimal_font)); };
    auto fits = [&](const std::string &t) { return (int)wrap(t).size() <= max_lines; };

    std::vector<std::string> texts;
    std::string page;
    auto next_page = [&]() {
        if (!page.empty()) texts.push_back(page);
        page.clear();
    };
    // adds a piece to this page, or starts the next page with it; false if it is longer than a page
    auto add = [&](const std::string &piece) {
        std::string joined = page.empty() ? piece : page + " " + piece;
        if (fits(joined)) {
            page = joined;
            return true;
        }
        if (!fits(piece)) return false;
        next_page();
        page = piece;
        return true;
    };

    for (auto &sentence : split_after(text, ".!?")) {
        if (add(sentence)) continue;
        for (auto &part : split_after(sentence, ",;:")) {
            if (add(part)) continue;
            // no comma to split at: fill pages word by word
            next_page();
            auto lines = wrap(part);
            for (size_t i = 0; i < lines.size(); i++) {
                page += (page.empty() ? "" : " ") + lines[i];
                if ((int)(i % max_lines) == max_lines - 1 && i + 1 < lines.size()) next_page();
            }
        }
    }
    next_page();

    std::vector<std::vector<std::string>> out;
    for (auto &t : texts) out.push_back(wrap(t));
    return out;
}

void Dialogue::start(const Scene &s) {
    scene = s;
    index = 0;
    closing = false;
    open_speech();
}

int Dialogue::page_chars() const {
    if (!active() || closing || scene.lines[index].row == CARD || page >= (int)pages.size()) return 0;
    int n = 0;
    for (auto &l : pages[page]) n += l.size();
    return n;
}

void Dialogue::crackle() {
    static_ms = STATIC_MS;
    audio::play(audio::Sfx::Static);
}

void Dialogue::open_page() {
    shown_ms = 0;
    done_ms = 0;
    revealed = 0;
}

void Dialogue::open_speech() {
    page = 0;
    open_page();
    pages.clear();
    static_ms = 0;
    if (!active()) return;
    const Speech &sp = scene.lines[index];
    if (sp.row == CARD) {
        pages.push_back(split_lines(sp.text));  // a card is one page however many lines it has
        return;
    }
    pages = paginate(sp.text, TEXT_W, LINES_PER_PAGE);
    // someone new on comms
    if (index == 0 || !same_speaker(sp, scene.lines[index - 1])) crackle();
}

void Dialogue::update(uint32_t dt) {
    if (!active()) return;
    if (static_ms > 0) {
        static_ms -= dt;
        if (static_ms <= 0 && closing) scene = Scene{nullptr, 0};
        return;
    }
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
    if (page + 1 < (int)pages.size()) {
        page++;
        open_page();
    } else if (index + 1 == scene.count && sp.row != CARD) {
        // the last speaker signs off
        closing = true;
        crackle();
    } else {
        index++;
        open_speech();
    }
}

void Dialogue::draw_card(uint32_t time) {
    // "TURBO" is drawn as the show's own logo, as on the title screen
    const auto &lines = pages[0];
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
    // face, drawn at 2x; it talks (and bobs) while the text appears. The Static's frame flickers through its five frames.
    int frame = face_frame(sp, talking(), shown_ms);
    bool bob = talking() && (shown_ms / MOUTH_MS) % 2;
    if (sp.row == STATIC_ROW) frame = (int)((time / 90) % FRAMES);
    Rect face(2, BOX_Y + 4 - (bob ? 1 : 0), 16, 16);
    draw_face(sp, frame, face);
    if (static_ms > 0) draw_static(face);

    int y = BOX_Y + 3;
    if (sp.name) {
        // while the comms crackle the name arrives as scrambled data, settling letter by letter
        // (and falls apart the same way when the last speaker signs off)
        std::string name = sp.name;
        if (static_ms > 0) {
            static const char GLYPHS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#%&?";
            int len = (int)name.size();
            int settled = len - (len + 1) * static_ms / STATIC_MS;
            if (closing) settled = len - 1 - settled;
            uint32_t seed = static_ms / 40;  // new noise every 40 ms
            for (int i = std::max(settled, 0); i < len; i++) {
                seed = seed * 1103515245u + 12345u + i;
                name[i] = GLYPHS[(seed >> 16) % (sizeof(GLYPHS) - 1)];
            }
        }
        shadow_text(name, minimal_font, Point(TEXT_X, y), static_ms > 0 ? Pen(160, 200, 255) : Pen(255, 210, 90));
    }
    y += LINE_H + 1;
    int left = revealed;
    for (auto &line : pages[page]) {
        if (left <= 0) break;
        int n = std::min(left, (int)line.size());
        shadow_text(std::string_view(line).substr(0, n), minimal_font, Point(TEXT_X, y), Pen(230, 230, 245));
        left -= n;
        y += LINE_H;
    }

    // a small arrow next to the name, bobbing, when A goes on
    if (!talking() && !closing && done_ms >= INPUT_DELAY_MS) {
        Point a(SCREEN_W - 6, BOX_Y + 4 + (int)((time / 250) % 2));
        screen.pen = Pen(255, 230, 120);
        screen.h_span(a + Point(-2, 0), 5);
        screen.h_span(a + Point(-1, 1), 3);
        screen.pixel(a + Point(0, 2));
    }
}

void Comms::say(const Speech &sp, int32_t ms) {
    speech = &sp;
    lines = split_lines(screen.wrap_text(sp.text, TEXT_W, minimal_font));
    lines.resize(std::min<size_t>(lines.size(), 2));
    shown_ms = 0;
    left_ms = ms;
    revealed = 0;
}

int Comms::chars() const {
    int n = 0;
    for (auto &l : lines) n += l.size();
    return n;
}

void Comms::update(uint32_t dt) {
    if (!speech) return;
    shown_ms += dt;
    left_ms -= dt;
    if (left_ms <= 0) {
        speech = nullptr;
        return;
    }
    int due = std::min(chars(), (int)(shown_ms / CHAR_MS));
    if (due / 2 > revealed / 2) audio::talk(speech->row);
    revealed = due;
}

void Comms::draw(int y) {
    if (!speech) return;
    screen.pen = Pen(6, 4, 30, 200);
    screen.rectangle(Rect(0, y, SCREEN_W, 20));
    screen.pen = Pen(90, 80, 200);
    screen.h_span(Point(0, y + 20), SCREEN_W);
    bool talking = revealed < chars();
    draw_face(*speech, face_frame(*speech, talking, shown_ms), Rect(2, y + 2, 16, 16));
    int left = revealed;
    for (size_t i = 0; i < lines.size() && left > 0; i++) {
        int n = std::min(left, (int)lines[i].size());
        shadow_text(std::string_view(lines[i]).substr(0, n), minimal_font, Point(TEXT_X, y + 3 + i * LINE_H), Pen(230, 230, 245));
        left -= n;
    }
}

}  // namespace story
