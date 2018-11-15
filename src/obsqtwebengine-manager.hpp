#pragma once

#include <obs-module.h>

struct shared_data {
 pthread_mutex_t mutex;
 uint8_t data;
};

class ObsQtWebEngineManager {
 public:
  ObsQtWebEngineManager(char *url, uint32_t width, uint32_t height, uint32_t fps, char *css, bool interactive);
  ~ObsQtWebEngineManager();
  uint8_t *GetData() { return &data->data; }
  void Lock();
  void UnLock();
  void SetUrl(char *url, char *css);
 private:
  void KillRenderer();
  void SpawnRenderer(char *url, char *css);
 private:
  int fd = -1;
  int pid = 0;
  int uid = 0;
  uint32_t width, height, fps;
  bool interactive;
  struct shared_data *data = NULL;
};
