#include "PR/os_internal.h"
#include "../io/controller.h"
#include "PR/os_voice.h"

// macro hell

#define IS_HIRAGANA_VOWEL(s) (((s) == 'あ') || ((s) == 'い') || ((s) == 'う') || ((s) == 'え') || ((s) == 'お'))
#define IS_HIRAGANA_VOWEL_SMALL(s) (((s) == 'ぁ') || ((s) == 'ぃ') || ((s) == 'ぅ') || ((s) == 'ぇ') || ((s) == 'ぉ'))
#define IS_KATAKANA_VOWEL(s) (((s) == 'ア') || ((s) == 'イ') || ((s) == 'ウ') || ((s) == 'エ') || ((s) == 'オ'))
#define IS_KATAKANA_VOWEL_SMALL(s) (((s) == 'ァ') || ((s) == 'ィ') || ((s) == 'ゥ') || ((s) == 'ェ') || ((s) == 'ォ'))
#define IS_VOWEL(s) (IS_HIRAGANA_VOWEL((s)) || IS_KATAKANA_VOWEL((s)))

// The manual describes these cases as "Romanized by k, t, c, or p"
#define IS_HIRAGANA_K(s) (((s) == 'か') || ((s) == 'き') || ((s) == 'く') || ((s) == 'け') || ((s) == 'こ'))
#define IS_KATAKANA_K(s) (((s) == 'カ') || ((s) == 'キ') || ((s) == 'ク') || ((s) == 'ケ') || ((s) == 'コ'))
#define IS_HIRAGANA_T(s) (((s) == 'た') || ((s) == 'ち') || ((s) == 'つ') || ((s) == 'て') || ((s) == 'と'))
#define IS_KATAKANA_T(s) (((s) == 'タ') || ((s) == 'チ') || ((s) == 'ツ') || ((s) == 'テ') || ((s) == 'ト'))
#define IS_HIRAGANA_P(s) (((s) == 'ぱ') || ((s) == 'ぴ') || ((s) == 'ぷ') || ((s) == 'ぺ') || ((s) == 'ぽ'))
#define IS_KATAKANA_P(s) (((s) == 'パ') || ((s) == 'ピ') || ((s) == 'プ') || ((s) == 'ペ') || ((s) == 'ポ'))

// Includes ゎ/ヮ, if theres a better way to categorize those pls let me know
#define IS_HIRAGANA_Y_SMALL(s) (((s) == 'ゃ') || ((s) == 'ゅ') || ((s) == 'ょ') || ((s) == 'ゎ'))
#define IS_KATAKANA_Y_SMALL(s) (((s) == 'ャ') || ((s) == 'ュ') || ((s) == 'ョ') || ((s) == 'ヮ'))

#define IS_HARD_CONSONANT(s) (IS_HIRAGANA_K((s)) || IS_HIRAGANA_T((s)) || IS_HIRAGANA_P((s)) || IS_KATAKANA_K((s)) || IS_KATAKANA_T((s)) || IS_KATAKANA_P((s)))

// This definitely has a better name
#define IS_ENDING_GLYPH(s) (((s) == 'ん') || ((s) == 'ー') || ((s) == 'っ') || ((s) == 'ン') || ((s) == 'ッ'))

#define IS_DIPTHONG(s) (IS_HIRAGANA_VOWEL_SMALL((s)) || IS_HIRAGANA_Y_SMALL((s)) || IS_KATAKANA_VOWEL_SMALL((s)) || IS_KATAKANA_Y_SMALL((s)) || ((s) == 0x8395) || ((s) == 0x8396))

// The VRU can store only 880 semisyllables of words in its dictionary.
// Use this function to determine the semisyllable count before adding a word with osVoiceSetWord,
//  to properly see whether it will fit.

void osVoiceCountSyllables(u8 *data, u32 *syllable) {
    s32 k;
    u16 sjis;
    u16 old;

    old = 0;
    //   - Assumes all words have at least 1 syllable
    //!  - Actually counts semisyllables, but is named syllable for some reason
    *syllable = 1;
    for (k = 0; data[k] != 0; k += 2) {
        sjis = data[k + 1] + (data[k] << 8);
        if (IS_VOWEL(sjis)) {
            // Vowels have 2 semisyllables at the start of the word, and 1 otherwise
            if (k == 0) {
                *syllable += 2;
            } else {
                *syllable += 1;
            }
        } else if (IS_HARD_CONSONANT(sjis)) {
            // Consonants generally have 3 semisyllables, unless emphasized with a previous っ/ッ,
            //  or if they occur at the start of a word
            if (k == 0) {
                *syllable += 2;
            } else if ((old == 'っ') || (old == 'ッ')) {
                *syllable += 2;
            } else {
                *syllable += 3;
            }
        } else if (IS_DIPTHONG(sjis)) {
            // Dipthongs don't increase the syllable count if preceded by a consonant
            if (IS_VOWEL(old)) {
                *syllable += 1;
            }
        } else if (IS_ENDING_GLYPH(sjis)) {
            // ん, っ, and ー all have one semisyllable
            *syllable += 1;
        } else {
            // all other cases add 2 semisyllables
            *syllable += 2;
        }
        old = sjis;
    }
}