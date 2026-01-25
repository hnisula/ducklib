#ifndef DUCKLIB_INPUT_H
#define DUCKLIB_INPUT_H
#include <vector>

#include "ducklib/core/platform/app_window.h"

namespace ducklib {
struct InputButtonState {
    bool mouse_buttons[5];

    void clear();
};

struct InputState {
    static constexpr auto MAX_TEXT_INPUTS = 128;

    std::vector<char8_t> text_inputs;
    int mouse_x, mouse_y;
    
    InputButtonState current;
    InputButtonState previous;

    // TODO: DRAG START INFO so that dragging from outside control wouldn't select control when released

    bool mouse_pressed(uint8_t button) const;
    bool mouse_released(uint8_t button) const;
};

void initialize_raw_input(AppWindow& window);
void shutdown_raw_input();
void frame_input_reset(InputState& input_state);
}

#endif //DUCKLIB_INPUT_H