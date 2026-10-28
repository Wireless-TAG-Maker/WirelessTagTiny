#pragma once
#define ESP_OK 0
using esp_ldo_channel_handle_t=void*;
struct esp_ldo_channel_config_t { int chan_id;int voltage_mv;struct {bool adjustable;} flags; };
extern int acquireCalls,releaseCalls;extern bool acquireOK,releaseOK;
inline int esp_ldo_acquire_channel(const esp_ldo_channel_config_t *c,esp_ldo_channel_handle_t *h) { ++acquireCalls; if(!acquireOK)return -1;if(c->chan_id!=1||c->voltage_mv!=1800||c->flags.adjustable)return -2;*h=(void*)1;return 0; }
inline int esp_ldo_release_channel(esp_ldo_channel_handle_t h) { ++releaseCalls;return releaseOK?0:-1; }
