#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad pico_keypad;

struct SimonGame
{
  // Definition of the patterns for each level
  // Patterns are terminated with a zero
  static constexpr uint8_t num_levels = 6;
  static constexpr uint16_t patterns[num_levels][8] = {
    { 0x0001, 0x0002, 0x0001, 0 },
    { 0x0020, 0x0080, 0x0100, 0x0400, 0},
    { 0x1000, 0x2000, 0x0004, 0x0200, 0},
    { 0x0008, 0x0200, 0x1000, 0x0800, 0x0100, 0},
    { 0x0002, 0x1000, 0x0400, 0x0040, 0x0001, 0x0002, 0},
    { 0x1000, 0x0010, 0x2000, 0x8000, 0x0040, 0x0020, 0x8000, 0},
  };

  // If you fail a level this many times, go back to the baginning
  static constexpr uint8_t max_fails = 2;

  // Game state
  uint8_t level = 0;
  uint8_t fails = 0;
  uint16_t pattern_delay = 700;

  // LED state for rendering
  uint16_t lit = 0;
  uint8_t colour_index = 0;

  // Display the pattern for the current level
  void show_pattern() {
    const uint16_t* pattern = patterns[level];
    while (1) {
      lit = *pattern;
      render();
      if (*pattern++ == 0) break;

      sleep_ms(pattern_delay);
    } 
  }

  // Check the player entering the pattern
  // Retrun true on success, false on error
  bool check_pattern() {
    uint16_t last_button_states = 0;
    uint16_t pattern_time_allowed = pattern_delay / 10;
    uint16_t timeout = pattern_time_allowed * 2;
    const uint16_t* pattern = patterns[level];

    while (timeout) {
      uint16_t button_states = pico_keypad.get_button_states();
      if (last_button_states != button_states && button_states) {
	last_button_states = button_states;

        if (button_states == *pattern) {
          // Correct button press
          lit = *pattern++;
          render();
          if (*pattern == 0) {
            // Got to end of pattern
            return true;
          }
          timeout = pattern_time_allowed;
        } else {
          // Incorect press
          return false;
        }
      }
      sleep_ms(20);
      timeout--;
    }
    return false;
  }

  // Run one cycle of the game
  void play() {
    colour_index = level % 5;

    show_pattern();

    if (check_pattern()) {
      // Player matched the pattern, flash for success
      sleep_ms(100);
      for (int i = 0; i < 3; ++i) {
        lit = 0xffff;
        render();
        sleep_ms(120);
        lit = 0;
        render();
        sleep_ms(150);
      }
      fails = 0;

      // Go to the next level.  Speed up if all levels done.
      level++;
      if (level >= num_levels) {
	level = 0;
	pattern_delay = (pattern_delay * 4) / 5;
      }
    }
    else
    {
      // Player failed the level, flash for failure
      lit = 0;
      render();
      sleep_ms(100);
      lit = 0xffff;
      colour_index = 5;
      render();
      sleep_ms(200);
      lit = 0;
      render();
      if (++fails == max_fails) {
	// Too many fails, reset to first level and flash a second time.
	level = 0;
	fails = 0;
	
        sleep_ms(100);
	lit = 0xffff;
        render();
        sleep_ms(200);
        lit = 0;
        render();
      }	
    }
    sleep_ms(pattern_delay);
  }

  void render() {
    // Render out the current state, you might recognise this code from the
    // example project :)
    for(uint8_t i = 0; i < PicoRGBKeypad::NUM_PADS; i++) {
      if ((lit >> i) & 1) {
        switch(colour_index)
        {
          case 0: pico_keypad.illuminate(i, 0x00, 0x20, 0x00);  break;
          case 1: pico_keypad.illuminate(i, 0x20, 0x20, 0x00);  break;
          case 2: pico_keypad.illuminate(i, 0x20, 0x00, 0x20);  break;
          case 3: pico_keypad.illuminate(i, 0x00, 0x00, 0x20);  break;
          case 4: pico_keypad.illuminate(i, 0x00, 0x20, 0x20);  break;
          case 5: pico_keypad.illuminate(i, 0x20, 0x00, 0x00);  break;
        }
      }
      else
      {
        pico_keypad.illuminate(i, 0, 0, 0);
      }
    }
    pico_keypad.update();
  }
};

SimonGame simon;

int main() {
  pico_keypad.init();
  pico_keypad.set_brightness(1.0f);

  while(true) {
    simon.play();
  }

  return 0;
}
