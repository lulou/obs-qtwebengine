#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>

#include "obsqtwebengine-manager.hpp"

#define blog(level, msg, ...) blog(level, "obsqtwebengine-source: " msg, ##__VA_ARGS__)

#define SHM_NAME "/obsqtwebengine"

ObsQtWebEngineManager::ObsQtWebEngineManager(char *url, uint32_t width, uint32_t height, uint32_t fps, char *css, bool interactive)
: width(width), height(height), fps(fps), interactive(interactive) {
 pthread_mutexattr_t attrmutex;
 uid = rand();
 char shm_name[50];
 snprintf(shm_name, 50, "%s%d", SHM_NAME, uid);
 fd = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
 if (fd == -1) {
  blog(LOG_ERROR, "shm_open error");
  return;
 }
 size_t data_size = width * height * 4;
 if(ftruncate(fd, sizeof(struct shared_data) + data_size) == -1) {
  blog(LOG_ERROR, "ftruncate error");
  return;
 }
 data = (struct shared_data *) mmap(NULL, sizeof(struct shared_data) + data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
 if(data == MAP_FAILED) {
  blog(LOG_ERROR, "mmap error");
  return;
 }
 pthread_mutexattr_init(&attrmutex);
 pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
 pthread_mutex_init(&data->mutex, &attrmutex);
 SpawnRenderer(url, css);
}

ObsQtWebEngineManager::~ObsQtWebEngineManager() {
 size_t data_size = width * height * 4;
 KillRenderer();
 pthread_mutex_destroy(&data->mutex);
 if(data != NULL && data != MAP_FAILED)
  munmap(data, sizeof(struct shared_data) + data_size);
 if (fd != -1) {
  char shm_name[50];
  snprintf(shm_name, 50, "%s%d", SHM_NAME, uid);
  shm_unlink(shm_name);
 }
}

void ObsQtWebEngineManager::SetUrl(char *url, char *css) {
 KillRenderer();
 SpawnRenderer(url, css);
}

void ObsQtWebEngineManager::SpawnRenderer(char *url, char *css) {
 char renderer[512];
 char width_buf[32];
 char height_buf[32];
 char fps_buf[32];
 char uid_buf[32];
 char *s;
 const char *file = obs_get_module_binary_path(obs_current_module());
 char interactive_buf[3];
 strncpy(renderer, file, 512);
 s = strrchr(renderer, '/');
 if (s)
  *(s+1) = '\0';
 strcat(renderer, "obsqtwebengine");
 snprintf(width_buf, 32, "%d", width);
 snprintf(height_buf, 32, "%d", height);
 snprintf(fps_buf, 32, "%d", fps);
 snprintf(uid_buf, 32, "%d", uid);
 snprintf(interactive_buf, 3, "%d", interactive);
 if(!css) {
  css = (char *) "";
 }
 char *argv[] = { renderer, url, width_buf, height_buf, fps_buf, uid_buf, css, interactive_buf, NULL };
 pid = fork();
 if (pid == 0)
  execv(renderer, argv);
}

void ObsQtWebEngineManager::KillRenderer() {
 if (pid > 0) {
  kill(pid, SIGTERM);
  waitpid(pid, NULL, 0);
 }
}

void ObsQtWebEngineManager::Lock() {
 pthread_mutex_lock(&data->mutex);
}

void ObsQtWebEngineManager::UnLock() {
 pthread_mutex_unlock(&data->mutex);
}
