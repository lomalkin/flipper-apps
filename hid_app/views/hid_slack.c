#include "hid_slack.h"
#include "../hid.h"
#include <gui/elements.h>

#include "hid_icons.h"

#define TAG "HidSlack"

struct HidSlack {
    View* view;
    Hid* hid;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
    bool is_cursor_set;
    HidTransport transport;
} HidSlackModel;

static void hid_slack_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidSlackModel* model = context;

    // Header
    if(model->transport == HidTransportBle) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "SlackMute");
    canvas_set_font(canvas, FontSecondary);

    // Keypad circles
    canvas_draw_icon(canvas, 76, 8, &I_Circles_47x47);

    // Up
    // if(model->up_pressed) {
    //     canvas_set_bitmap_mode(canvas, 1);
    //     canvas_draw_icon(canvas, 93, 9, &I_Pressed_Button_13x13);
    //     canvas_set_bitmap_mode(canvas, 0);
    //     canvas_set_color(canvas, ColorWhite);
    // }
    // canvas_draw_icon(canvas, 96, 11, &I_Arr_up_7x9);
    // canvas_set_color(canvas, ColorBlack);

    // Down
    // if(model->down_pressed) {
    //     canvas_set_bitmap_mode(canvas, 1);
    //     canvas_draw_icon(canvas, 93, 41, &I_Pressed_Button_13x13);
    //     canvas_set_bitmap_mode(canvas, 0);
    //     canvas_set_color(canvas, ColorWhite);
    // }
    // canvas_draw_icon(canvas, 96, 44, &I_Arr_dwn_7x9);
    // canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 77, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 81, 29, &I_Voldwn_6x6);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 109, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 111, 29, &I_Volup_8x6);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->ok_pressed) {
        canvas_draw_icon(canvas, 91, 23, &I_Like_pressed_17x17);
    } else {
        canvas_draw_icon(canvas, 94, 27, &I_Like_def_11x9);
    }
    // Exit
    canvas_draw_icon(canvas, 0, 54, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
}

static void
    hid_slack_process_press(HidSlack* hid_slack, HidSlackModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = true;

        //hid_hal_consumer_key_press(hid_slack->hid, HID_CONSUMER_VOLUME_MUTE);
        //hid_hal_consumer_key_press(hid_slack->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyDown) {
        model->down_pressed = true;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = true;
        hid_hal_consumer_key_press(hid_slack->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = true;
        hid_hal_consumer_key_press(hid_slack->hid, HID_CONSUMER_VOLUME_INCREMENT);
    } else if(event->key == InputKeyOk) {
        //hid_hal_consumer_key_press(hid_slack->hid, HID_KEYBOARD_L_SHIFT | HID_KEYBOARD_J);
        //hid_hal_consumer_key_press(hid_slack->hid, HID_KEYBOARD_L_SHIFT);
        //hid_hal_keyboard_press(hid_slack->hid, HID_KEYBOARD_J);
        //hid_hal_keyboard_press(hid_slack->hid, HID_KEYBOARD_L_SHIFT | HID_KEYBOARD_J);
        hid_hal_keyboard_press(hid_slack->hid, KEY_MOD_LEFT_SHIFT | KEY_MOD_LEFT_GUI | HID_KEYBOARD_SPACEBAR);  // Cmd+Shift+Space
        model->ok_pressed = true;
    }
}

static void
    hid_slack_process_release(HidSlack* hid_slack, HidSlackModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = false;
        //hid_hal_consumer_key_release(hid_slack->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyDown) {
        model->down_pressed = false;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = false;
        hid_hal_consumer_key_release(hid_slack->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = false;
        hid_hal_consumer_key_release(hid_slack->hid, HID_CONSUMER_VOLUME_INCREMENT);
    } else if(event->key == InputKeyOk) {
        //hid_hal_consumer_key_release(hid_slack->hid, HID_KEYBOARD_L_SHIFT | HID_KEYBOARD_J);
        //hid_hal_keyboard_release(hid_slack->hid, HID_KEYBOARD_L_SHIFT | HID_KEYBOARD_J);
        // hid_hal_keyboard_release(hid_slack->hid, HID_KEYBOARD_J);
        // hid_hal_consumer_key_release(hid_slack->hid, HID_KEYBOARD_L_SHIFT);
        //hid_hal_keyboard_press(hid_slack->hid, HID_KEYBOARD_L_SHIFT | HID_KEYBOARD_J);
        hid_hal_keyboard_release(hid_slack->hid, KEY_MOD_LEFT_SHIFT | KEY_MOD_LEFT_GUI | HID_KEYBOARD_SPACEBAR);  // Cmd+Shift+Space
        model->ok_pressed = false;
    }
}

static bool hid_slack_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidSlack* hid_slack = context;
    bool consumed = false;

    with_view_model(
        hid_slack->view,
        HidSlackModel * model,
        {
            if(event->type == InputTypePress) {
                hid_slack_process_press(hid_slack, model, event);
                if(model->connected && !model->is_cursor_set) {
                    //hid_slack_reset_cursor(hid_slack);
                    model->is_cursor_set = true;
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                hid_slack_process_release(hid_slack, model, event);
                consumed = true;
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyOk) {
                    //
                    consumed = true;
                } else if(event->key == InputKeyUp) {
                    // 
                    // Unmute
                    //hid_hal_consumer_key_press(hid_slack->hid, HID_CONSUMER_VOLUME_MUTE);
                    consumed = true;
                } else if(event->key == InputKeyDown) {
                    // 
                    // Mute
                    //hid_hal_consumer_key_release(hid_slack->hid, HID_CONSUMER_VOLUME_MUTE);
                    consumed = true;
                } else if(event->key == InputKeyBack) {
                    hid_hal_consumer_key_release_all(hid_slack->hid);
                    consumed = true;
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyBack) {
                    hid_hal_consumer_key_release_all(hid_slack->hid);
                    model->is_cursor_set = false;
                    consumed = false;
                }
            }
        },
        true);

    return consumed;
}

HidSlack* hid_slack_alloc(Hid* bt_hid) {
    HidSlack* hid_slack = malloc(sizeof(HidSlack));
    hid_slack->hid = bt_hid;
    hid_slack->view = view_alloc();
    view_set_context(hid_slack->view, hid_slack);
    view_allocate_model(hid_slack->view, ViewModelTypeLocking, sizeof(HidSlackModel));
    view_set_draw_callback(hid_slack->view, hid_slack_draw_callback);
    view_set_input_callback(hid_slack->view, hid_slack_input_callback);

    with_view_model(
        hid_slack->view, HidSlackModel * model, { model->transport = bt_hid->transport; }, true);

    return hid_slack;
}

void hid_slack_free(HidSlack* hid_slack) {
    furi_assert(hid_slack);
    view_free(hid_slack->view);
    free(hid_slack);
}

View* hid_slack_get_view(HidSlack* hid_slack) {
    furi_assert(hid_slack);
    return hid_slack->view;
}

void hid_slack_set_connected_status(HidSlack* hid_slack, bool connected) {
    furi_assert(hid_slack);
    with_view_model(
        hid_slack->view,
        HidSlackModel * model,
        {
            model->connected = connected;
            model->is_cursor_set = false;
        },
        true);
}
