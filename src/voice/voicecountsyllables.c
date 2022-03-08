#include "PR/os_internal.h"
#include "io/controller.h"
#include "PR/os_voice.h"

// macro hell

#define IS_HIRAGANA_VOWEL(s) ((s == 'あ') || (s == 'い') || (s == 'う') || (s == 'え') || (s == 'お'))
#define IS_HIRAGANA_VOWEL_SMALL(s) ((s == 'ぁ') || (s == 'ぃ') || (s == 'ぅ') || (s == 'ぇ') || (s == 'ぉ'))
#define IS_KATAKANA_VOWEL(s) ((s == 'ア') || (s == 'イ') || (s == 'ウ') || (s == 'エ') || (s == 'オ'))
#define IS_KATAKANA_VOWEL_SMALL(s) ((s == 'ァ') || (s == 'ィ') || (s == 'ゥ') || (s == 'ェ') || (s == 'ォ'))
#define IS_VOWEL(s) (IS_HIRAGANA_VOWEL((s)) || IS_KATAKANA_VOWEL((s)))

// The manual describes these cases as "Romanized by k, t, c, or p"
#define IS_HIRAGANA_K(s) ((s == 'か') || (s == 'き') || (s == 'く') || (s == 'け') || (s == 'こ'))
#define IS_KATAKANA_K(s) ((s == 'カ') || (s == 'キ') || (s == 'ク') || (s == 'ケ') || (s == 'コ'))
#define IS_HIRAGANA_T(s) ((s == 'た') || (s == 'ち') || (s == 'つ') || (s == 'て') || (s == 'と'))
#define IS_KATAKANA_T(s) ((s == 'タ') || (s == 'チ') || (s == 'ツ') || (s == 'テ') || (s == 'ト'))
#define IS_HIRAGANA_P(s) ((s == 'ぱ') || (s == 'ぴ') || (s == 'ぷ') || (s == 'ぺ') || (s == 'ぽ'))
#define IS_KATAKANA_P(s) ((s == 'パ') || (s == 'ピ') || (s == 'プ') || (s == 'ペ') || (s == 'ポ'))

// Includes ゎ/ヮ, if theres a better way to categorize those pls let me know
#define IS_HIRAGANA_Y_SMALL(s) ((s == 'ゃ') || (s == 'ゅ') || (s == 'ょ') || (s == 'ゎ'))
#define IS_KATAKANA_Y_SMALL(s) ((s == 'ャ') || (s == 'ュ') || (s == 'ョ') || (s == 'ヮ'))

#define IS_HARD_CONSONANT(s) (IS_HIRAGANA_K((s)) || IS_HIRAGANA_T((s)) || IS_HIRAGANA_P((s)) || IS_KATAKANA_K((s)) || IS_KATAKANA_T((s)) || IS_KATAKANA_P((s)))

// This definitely has a better name
#define IS_ENDING_GLYPH(s) ((s == 'ん') || (s == 'ー') || (s == 'っ') || (s == 'ン') || (s == 'ッ'))

#define IS_DIPTHONG(s) (IS_HIRAGANA_VOWEL_SMALL((s)) || IS_HIRAGANA_Y_SMALL((s)) || IS_KATAKANA_VOWEL_SMALL((s)) || IS_KATAKANA_Y_SMALL((s)) || (s == 0x8395) || (s == 0x8396))

// The VRU can store only 880 semisyllables of words in its dictionary.
// Use this function to determine the semisyllable count before adding a word with osVoiceSetWord,
//  to properly see whether it will fit.


void osVoiceCountSyllables(u8 *data, u32 *syllable) {
    s32 k;
    u16 sjis;
    u16 old;

    old = 0;
    *syllable = 1;
    for (k = 0; data[k] != 0; k += 2) {
        sjis = data[k + 1] + (data[k] << 8);
        if ((sjis == 0x82A0) || (sjis == 0x82A2) || (sjis == 0x82A4) || (sjis == 0x82A6) || (sjis == 0x82A8) || (sjis == 0x8341) || (sjis == 0x8343) || (sjis == 0x8345) || (sjis == 0x8347) || (sjis == 0x8349)) {
            if (k == 0) {
                *syllable += 2;
            } else {
                *syllable += 1;
            }
        } else if ((sjis == 0x82A9) || (sjis == 0x82AB) || (sjis == 0x82AD) || (sjis == 0x82AF) || (sjis == 0x82B1) || (sjis == 0x82BD) || (sjis == 0x82BF) || (sjis == 0x82C2) || (sjis == 0x82C4) || (sjis == 0x82C6) || (sjis == 0x82CF) || (sjis == 0x82D2) || (sjis == 0x82D5) || (sjis == 0x82D8) || (sjis == 0x82DB) || (sjis == 0x834A) || (sjis == 0x834C) || (sjis == 0x834E) || (sjis == 0x8350) || (sjis == 0x8352) || (sjis == 0x835E) || (sjis == 0x8360) || (sjis == 0x8363) || (sjis == 0x8365) || (sjis == 0x8367) || (sjis == 0x8370) || (sjis == 0x8373) || (sjis == 0x8376) || (sjis == 0x8379) || (sjis == 0x837C)) {
            if (k == 0) {
                *syllable += 2;
            } else if ((old == 0x82C1) || (old == 0x8362)) {
                *syllable += 2;
            } else {
                *syllable += 3;
            }
        } else if ((sjis == 0x829F) || (sjis == 0x82A1) || (sjis == 0x82A3) || (sjis == 0x82A5) || (sjis == 0x82A7) || (sjis == 0x82E1) || (sjis == 0x82E3) || (sjis == 0x82E5) || (sjis == 0x82EC) || (sjis == 0x8340) || (sjis == 0x8342) || (sjis == 0x8344) || (sjis == 0x8346) || (sjis == 0x8348) || (sjis == 0x8383) || (sjis == 0x8385) || (sjis == 0x8387) || (sjis == 0x838E) || (sjis == 0x8395) || (sjis == 0x8396)) {
            if ((old == 0x82A0) || (old == 0x82A2) || (old == 0x82A4) || (old == 0x82A6) || (old == 0x82A8) || (old == 0x8341) || (old == 0x8343) || (old == 0x8345) || (old == 0x8347) || (old == 0x8349)) {
                *syllable += 1;
            }
        } else if ((sjis == 0x82F1) || (sjis == 0x815B) || (sjis == 0x82C1) || (sjis == 0x8393) || (sjis == 0x8362)) {
            *syllable += 1;
        } else {
            *syllable += 2;
        }
        old = sjis;
    }
}

