/*
 * MIT License
 *
 * Copyright (c) 2022 chris0z
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef WORKOUT_TIMER_FACE_H_
#define WORKOUT_TIMER_FACE_H_

#include "movement.h"

typedef enum {
    workout_active,
    workout_break,
    workout_waiting,
    workout_setting
} workout_timer_mode_t;

typedef struct {
    // Anything you need to keep track of, put it here!
    uint8_t workout_rep;
    uint8_t action_sec;
    uint8_t break_sec;
    
    uint32_t target_ts;
    uint32_t now_ts;
    uint8_t selection;
    workout_timer_mode_t mode;
} workout_timer_state_t;

void workout_timer_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr);
void workout_timer_face_activate(movement_settings_t *settings, void *context);
bool workout_timer_face_loop(movement_event_t event, movement_settings_t *settings, void *context);
void workout_timer_face_resign(movement_settings_t *settings, void *context);

#define workout_timer_face ((const watch_face_t){ \
    workout_timer_face_setup, \
    workout_timer_face_activate, \
    workout_timer_face_loop, \
    workout_timer_face_resign, \
    NULL, \
})

#endif // WORKOUT_TIMER_FACE_H_

