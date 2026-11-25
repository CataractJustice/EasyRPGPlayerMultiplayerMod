// text_bubble.h
#ifndef TEXT_BUBBLE_H
#define TEXT_BUBBLE_H

#include "sprite.h"
#include "game_character.h"

class TextBubble {
public:
    TextBubble(const std::string& text, const Game_Character* character);
    ~TextBubble();

    void Update();
    bool GetShouldBeDestroyed();
    void FadeNow();
private:
    Sprite sprite;
    std::string text;
    int frame;
    int originalY;
    const Game_Character* character;
};

#endif // TEXT_BUBBLE_H