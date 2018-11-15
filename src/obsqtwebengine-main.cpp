#include <unistd.h>
#include <sys/syscall.h>

#include <obs-module.h>
#include "obsqtwebengine-source.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-qtwebengine", "en-US")

static const char *qtwebengine_get_name(void*) {
 return obs_module_text("QtWebEngine");
}

static void reload_hotkey_pressed(void *data, obs_hotkey_id id, obs_hotkey_t *key, bool pressed) {
 UNUSED_PARAMETER(id);
 UNUSED_PARAMETER(key);
 UNUSED_PARAMETER(pressed);
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 ws->Reload();
}

static bool is_local_file_modified(obs_properties_t *props, obs_property_t *prop, obs_data_t *settings) {
 UNUSED_PARAMETER(prop);
 bool enabled = obs_data_get_bool(settings, "is_local_file");
 obs_property_t *url = obs_properties_get(props, "url");
 obs_property_t *local_file = obs_properties_get(props, "local_file");
 obs_property_set_visible(url, !enabled);
 obs_property_set_visible(local_file, enabled);
 return true;
}

static bool reload_button_clicked(obs_properties_t *props, obs_property_t *property, void *data) {
 UNUSED_PARAMETER(props);
 UNUSED_PARAMETER(property);
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 ws->Reload();
 return true;
}

static obs_properties_t *qtwebengine_get_properties(void *) {
 obs_properties_t *props = obs_properties_create();
 obs_property_t *prop = obs_properties_add_bool(props, "is_local_file", obs_module_text("LocalFile"));
 obs_property_set_modified_callback(prop, is_local_file_modified);
 obs_properties_add_path(props, "local_file", obs_module_text("LocalFile"), OBS_PATH_FILE, "*.*", nullptr);
 obs_properties_add_text(props, "url", obs_module_text("URL"), OBS_TEXT_DEFAULT);
 obs_properties_add_int(props, "width", obs_module_text("Width"), 1, 4096, 1);
 obs_properties_add_int(props, "height", obs_module_text("Height"), 1, 4096, 1);
 obs_properties_add_int(props, "fps", obs_module_text("FPS"), 1, 60, 1);
 obs_properties_add_path(props, "css_file", obs_module_text("CustomCSS"), OBS_PATH_FILE, "*.css", nullptr);
 obs_properties_add_button(props, "reload", obs_module_text("Reload"), reload_button_clicked);
 obs_properties_add_bool(props, "interactive", obs_module_text("Interactive"));
 return props;
}

static void qtwebengine_get_defaults(obs_data_t *settings) {
 obs_data_set_default_string(settings, "url", "http://www.obsproject.com");
 obs_data_set_default_int(settings, "width", 800);
 obs_data_set_default_int(settings, "height", 600);
 obs_data_set_default_int(settings, "fps", 30);
 obs_data_set_default_bool(settings, "interactive", false);
}

static void qtwebengine_tick(void *data, float seconds) {
 UNUSED_PARAMETER(seconds);
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 ws->PrepareTexture();
}

static void qtwebengine_render(void *data, gs_effect_t *effect) {
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 ws->RenderTexture(effect);
}

static void *qtwebengine_create(obs_data_t *settings, obs_source_t *source) {
 UNUSED_PARAMETER(settings);
 ObsQtWebEngineSource *ws = new ObsQtWebEngineSource(source);
 ws->UpdateSettings(settings);
 ws->reload_key = obs_hotkey_register_source(source, "obs-qtwebengine.reload", obs_module_text("Reload"), reload_hotkey_pressed, ws);
 return ws;
}

static void qtwebengine_destroy(void *data) {
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 obs_hotkey_unregister(ws->reload_key);
 delete ws;
}

static void qtwebengine_update(void *data, obs_data_t *settings) {
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 ws->UpdateSettings(settings);
}

static uint32_t qtwebengine_get_width(void *data) {
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 return ws->GetWidth();
}

static uint32_t qtwebengine_get_height(void *data) {
 ObsQtWebEngineSource *ws = static_cast<ObsQtWebEngineSource *>(data);
 return ws->GetHeight();
}

bool obs_module_load(void) {
 struct obs_source_info info = {};
 info.id             = "obsqtwebengine-source";
 info.type           = OBS_SOURCE_TYPE_INPUT;
 info.output_flags   = OBS_SOURCE_VIDEO;
 info.get_name       = qtwebengine_get_name;
 info.create         = qtwebengine_create;
 info.destroy        = qtwebengine_destroy;
 info.update         = qtwebengine_update;
 info.get_width      = qtwebengine_get_width;
 info.get_height     = qtwebengine_get_height;
 info.get_properties = qtwebengine_get_properties;
 info.get_defaults   = qtwebengine_get_defaults;
 info.video_tick     = qtwebengine_tick;
 info.video_render   = qtwebengine_render;
 obs_register_source(&info);
 return true;
}
