#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidSlack HidSlack;

HidSlack* hid_slack_alloc(Hid* bt_hid);

void hid_slack_free(HidSlack* hid_slack);

View* hid_slack_get_view(HidSlack* hid_slack);

void hid_slack_set_connected_status(HidSlack* hid_slack, bool connected);
