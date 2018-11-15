#pragma once

#include <obs-module.h>
#include "obsqtwebengine-manager.hpp"

class ObsQtWebEngineSource {
 public:
  ObsQtWebEngineSource(obs_source_t *source);
  ~ObsQtWebEngineSource();
 public:
  uint32_t GetWidth() const { return width; }
  uint32_t GetHeight() const { return height; }
  uint32_t GetFps() const { return fps; }
  void UpdateSettings(obs_data_t *settings);
  void RenderTexture(gs_effect_t *effect);
  void PrepareTexture();
  void LockTexture();
  void UnlockTexture();
  void Reload();
 private:
  void UpdatePage(bool resize, bool fps_change);
 public:
  obs_hotkey_id reload_key;
 private:
  bool isLocalFile;
  char *url;
  uint32_t width;
  uint32_t height;
  uint32_t fps;
  char *css;
  bool interactive;
  obs_source_t *source;
  gs_texture_t *activeTexture;
  pthread_mutex_t textureLock;
  ObsQtWebEngineManager *manager;
};
