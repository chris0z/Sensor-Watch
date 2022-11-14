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

#include <stdlib.h>
#include <string.h>
#include "workout_timer_face.h"
#include "watch.h"
#include "watch_utility.h"

#define W_SELECTIONS 3
#define REPS 8
#define ACTION_SEC 30
#define BREAK_SEC 15

static inline int32_t get_tz_offset(movement_settings_t *settings) {
    return movement_timezone_offsets[settings->bit.time_zone] * 60;
}

static void workout_start(workout_timer_state_t *state, movement_settings_t *settings) {
    watch_date_time now = watch_rtc_get_date_time();
    //state->mode = workout_active;
    if(state->mode == workout_active){
        state->now_ts = watch_utility_date_time_to_unix_time(now, get_tz_offset(settings));
        state->target_ts = watch_utility_offset_timestamp(state->now_ts, 0, 0,state->action_sec);
        watch_date_time target_dt = watch_utility_date_time_from_unix_time(state->target_ts, get_tz_offset(settings));
        movement_schedule_background_task(target_dt); 
    }
    if(state->mode == workout_break){
        state->now_ts = watch_utility_date_time_to_unix_time(now, get_tz_offset(settings));
        state->target_ts = watch_utility_offset_timestamp(state->now_ts, 0, 0,state->break_sec);
        watch_date_time target_dt = watch_utility_date_time_from_unix_time(state->target_ts, get_tz_offset(settings));
        movement_schedule_background_task(target_dt); 
    }
    
}

void workout_draw(workout_timer_state_t * state, uint8_t subsecond){
    char buf[16];
    uint32_t delta;
    div_t result;
    uint8_t sec;
    switch (state->mode) {
        case workout_active:
            delta = state->target_ts - state->now_ts;
            result = div(delta, 60);
            //min = result.quot;
            sec = result.rem;
            watch_clear_indicator(WATCH_INDICATOR_BELL);
            watch_set_indicator(WATCH_INDICATOR_SIGNAL);
            sprintf(buf, "WO%2d  %02d%02d",state->workout_rep,sec, state->break_sec);
            break;
        case workout_break:
            delta = state->target_ts - state->now_ts;
            result = div(delta, 60);
            //min = result.quot;
            sec = result.rem;
            watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
            watch_set_indicator(WATCH_INDICATOR_BELL);
            sprintf(buf, "WO%2d  %02d%02d", state->workout_rep,state->action_sec, sec);
            break;
        case workout_waiting:
            watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
            watch_set_indicator(WATCH_INDICATOR_BELL);
            sprintf(buf, "WO%2d  %02d%02d", state->workout_rep,state->action_sec, state->break_sec);
            break;
        case workout_setting:
            sprintf(buf, "WO%2d  %02d%02d", state->workout_rep,state->action_sec, state->break_sec);
            if (subsecond % 2) {
                switch(state->selection) {
                    case 0:
                        buf[2] = buf[3] = ' ';
                        break;
                    case 1:
                        buf[6] = buf[7] = ' ';
                        break;
                    case 2:
                        buf[8] = buf[9] = ' ';
                        break;
                    default:
                        break;
                }
            }
            break;
    }
    watch_display_string(buf, 0);
}

static void ring(workout_timer_state_t *state) {
    if(state->mode = workout_active){
        movement_play_alarm();
        //reset(state);
    }
   
}

static void settings_increment(workout_timer_state_t *state) {
    switch(state->selection) {
        case 0:
            state->workout_rep = (state->workout_rep + 1) % 60;
            break;
        case 1:
            state->action_sec = (state->action_sec + 1) % 60;
            break;
        case 2:
            state->break_sec = (state->break_sec + 1) % 60;
            break;
        default:
            // should never happen
            break;
    }
    return;
}

static void reset(workout_timer_state_t *state) {
    state->mode = workout_waiting;
    movement_cancel_background_task();
    watch_clear_indicator(WATCH_INDICATOR_BELL);
}

void workout_timer_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr) {
    (void) settings;
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(workout_timer_state_t));
        workout_timer_state_t *state = (workout_timer_state_t*)*context_ptr;
        memset(*context_ptr, 0, sizeof(workout_timer_state_t));
        state->workout_rep = REPS;
        state->action_sec = ACTION_SEC;
        state->break_sec = BREAK_SEC;
        watch_set_indicator(WATCH_INDICATOR_LAP);

        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void workout_timer_face_activate(movement_settings_t *settings, void *context) {
    (void) settings;
    workout_timer_state_t *state = (workout_timer_state_t *)context;
    if(state->mode == workout_active){
        watch_date_time now = watch_rtc_get_date_time();
        state->now_ts = watch_utility_date_time_to_unix_time(now, get_tz_offset(settings));
    }
    // Handle any tasks related to your watch face coming on screen.
}

bool workout_timer_face_loop(movement_event_t event, movement_settings_t *settings, void *context) {
    workout_timer_state_t *state = (workout_timer_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            workout_draw(state,event.subsecond);
            // Show your initial UI here.
            break;
        case EVENT_TICK:
            if (state->mode == workout_active) {
                state->now_ts++;
            }
            workout_draw(state,event.subsecond);
            // If needed, update your display here.
            break;
        case EVENT_MODE_BUTTON_UP:
            // You shouldn't need to change this case; Mode almost always moves to the next watch face.
            movement_move_to_next_face();
            break;
        case EVENT_LIGHT_BUTTON_UP:
            // If you have other uses for the Light button, you can opt not to illuminate the LED for this event.
            movement_illuminate_led();
            switch(state->mode) {
                case workout_active:
                    movement_illuminate_led();
                    break;
                case workout_break:
                    movement_illuminate_led();
                    break;
                case workout_waiting:
                    state->mode = workout_setting;
                    movement_request_tick_frequency(4);
                    break;
                case workout_setting:
                    state->selection++;
                    if(state->selection >= W_SELECTIONS) {
                        state->selection = 0;
                        state->mode = workout_waiting;
                        movement_request_tick_frequency(1);
                    }
                    break;
            }
            workout_draw(state,event.subsecond);

            break;
        case EVENT_ALARM_BUTTON_UP:
            // Just in case you have need for another button.
            switch(state->mode) {
                case workout_active:
                    reset(state);
                    break;
                case workout_break:
                    reset(state);
                    break;
                case workout_waiting:
                    if (!(state->action_sec == 0 && state->break_sec == 0)) {
                        // Only start the timer if we have a valid time.
                        state->mode = workout_active;
                        workout_start(state, settings);
                    }
                    break;
                case workout_setting:
                    settings_increment(state);
                    break;
            }
            workout_draw(state,event.subsecond);
            break;
        case EVENT_BACKGROUND_TASK:
            ring(state);             //Buzzer
            if(state->mode == workout_active){
                state->workout_rep -= 1; //Decrease Reps
                state->mode = workout_break;
            }
            else{
                state->mode = workout_active;
            }

            break;
        case EVENT_TIMEOUT:
            // Your watch face will receive this event after a period of inactivity. If it makes sense to resign,
            // you may uncomment this line to move back to the first watch face in the list:
            movement_move_to_face(0);
            break;
        case EVENT_LOW_ENERGY_UPDATE:
            // If you did not resign in EVENT_TIMEOUT, you can use this event to update the display once a minute.
            // Avoid displaying fast-updating values like seconds, since the display won't update again for 60 seconds.
            // You should also consider starting the tick animation, to show the wearer that this is sleep mode:
            // watch_start_tick_animation(500);
            break;
        default:
            break;
    }

    // return true if the watch can enter standby mode. If you are PWM'ing an LED or buzzing the buzzer here,
    // you should return false since the PWM driver does not operate in standby mode.
    return true;
}

void workout_timer_face_resign(movement_settings_t *settings, void *context) {
    (void) settings;
    (void) context;

    // handle any cleanup before your watch face goes off-screen.
}

