#include <pthread.h>
#include "obsqtwebengine-source.hpp"

#define blog(level, msg, ...) blog(level, "qtwebengine-source: " msg, ##__VA_ARGS__)

ObsQtWebEngineSource::ObsQtWebEngineSource(obs_source_t *s) {
 source = s;
 activeTexture = nullptr;
 manager = nullptr;
 pthread_mutex_init(&textureLock, nullptr);
}

ObsQtWebEngineSource::~ObsQtWebEngineSource() {
 LockTexture();
 if(activeTexture) {
  obs_enter_graphics();
  gs_texture_destroy(activeTexture);
  activeTexture = nullptr;
  obs_leave_graphics();
 }
 UnlockTexture();
 pthread_mutex_destroy(&textureLock);
 if(manager)
  delete manager;
}

void ObsQtWebEngineSource::LockTexture() {
 pthread_mutex_lock(&textureLock);
}

void ObsQtWebEngineSource::UnlockTexture() {
 pthread_mutex_unlock(&textureLock);
}

void ObsQtWebEngineSource::UpdatePage(bool resize, bool fps_change) {
 LockTexture();
 obs_enter_graphics();
 if(resize || !activeTexture) {
  if(activeTexture)
   gs_texture_destroy(activeTexture);
  activeTexture = gs_texture_create(width, height, GS_RGBA, 1, nullptr, GS_DYNAMIC);
 }
 obs_leave_graphics();
 UnlockTexture();
 if(!manager || resize || fps_change) {
  if(manager)
   delete manager;
  manager = new ObsQtWebEngineManager(url, width, height, fps, css, interactive);
 } else {
  manager->SetUrl(url, css);
 }
}

void ObsQtWebEngineSource::UpdateSettings(obs_data_t *settings) {
 uint32_t old_width = width;
 uint32_t old_height = height;
 uint32_t old_fps = fps;
 isLocalFile = obs_data_get_bool(settings, "is_local_file");
 url = (char *) obs_data_get_string(settings, isLocalFile ? "local_file" : "url");
 width = (uint32_t) obs_data_get_int(settings, "width");
 height = (uint32_t) obs_data_get_int(settings, "height");
 fps = (uint32_t) obs_data_get_int(settings, "fps");
 css = (char *) obs_data_get_string(settings, "css_file");
 interactive = (bool) obs_data_get_bool(settings, "interactive");
 UpdatePage(old_width != width || old_height != height, old_fps != fps);
}

void ObsQtWebEngineSource::PrepareTexture() {
 LockTexture();
 if(!activeTexture || !obs_source_showing(source)) {
  UnlockTexture();
  return;
 }
 manager->Lock();
 obs_enter_graphics();
 gs_texture_set_image(activeTexture, manager->GetData(), width * 4, false);
 obs_leave_graphics();
 manager->UnLock();
 UnlockTexture();
}

void ObsQtWebEngineSource::RenderTexture(gs_effect_t *effect) {
 LockTexture();
 if (!activeTexture) {
  UnlockTexture();
  return;
 }
 gs_reset_blend_state();
 gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), activeTexture);
 gs_draw_sprite(activeTexture, 0, width, height);
 UnlockTexture();
}

void ObsQtWebEngineSource::Reload() {
 if (manager)
  delete manager;
 manager = new ObsQtWebEngineManager(url, width, height, fps, css, interactive);
}
