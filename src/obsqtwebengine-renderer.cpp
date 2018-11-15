#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <pthread.h>

#include <QApplication>
#include <QGraphicsView>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QPainter>

#define SHM_NAME "/obsqtwebengine"

struct shared_data {
 pthread_mutex_t mutex;
 uint8_t data;
};

static volatile sig_atomic_t done = 0;
static int fd;
static struct shared_data *data;
static int in_fd;
static char buf[sizeof(struct inotify_event)];
static volatile sig_atomic_t refresh = 0;

void term(int signum) {
 done = 1;
}

void file_changed(int signum) {
 read(in_fd, (void *) buf, sizeof(buf)); // right now don't really care what we get
 refresh = 1;
}

void init_shared_data(int width, int height, char *suffix) {
 char shm_name[50];
 snprintf(shm_name, 50, "%s%s", SHM_NAME, suffix);
 size_t data_size = width * height * 4;
 fd = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
 data = (struct shared_data *) mmap(NULL, sizeof(struct shared_data) + data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void uninit_shared_data(int width, int height) {
 size_t data_size = width * height * 4;
 munmap(data, sizeof(struct shared_data) + data_size);
}

void init_inotify(char *file) {
 in_fd = inotify_init1(IN_NONBLOCK);
 inotify_add_watch(in_fd, file, IN_MODIFY);
 fcntl(in_fd, F_SETFL, O_ASYNC);
 fcntl(in_fd, F_SETOWN, getpid());
 struct sigaction action;
 memset(&action, 0, sizeof(struct sigaction));
 action.sa_handler = file_changed;
 sigaction(SIGIO, &action, NULL);
}

void uninit_inotify() {
 close(in_fd);
}

int main(int argc, char *argv[]) {
 struct sigaction action;
 memset(&action, 0, sizeof(struct sigaction));
 action.sa_handler = term;
 sigaction(SIGTERM, &action, NULL);
 // shutdown if parent process dies
 prctl(PR_SET_PDEATHSIG, SIGTERM);
 int width = atoi(argv[2]);
 int height = atoi(argv[3]);
 int fps = atoi(argv[4]);
 char *suffix = argv[5];
 init_shared_data(atoi(argv[2]), atoi(argv[3]), suffix);
 QApplication app(argc, argv);
 //QGraphicsView gview;
 QWebEngineView view;
 //gview->setViewport(view);
 if(argv[7] != "1") {
  view.setAttribute(Qt::WA_DontShowOnScreen);
 }
 //connect(&page, SIGNAL (urlChanged(const QUrl&)), SLOT (mySlotName()));
 //view.page()->setBackgroundColor(Qt::transparent);
 //view.page()->settings()->setUserStyleSheetUrl(QUrl::fromUserInput(argv[6])); //Deprecated
 view.page()->profile()->setHttpCacheType(QWebEngineProfile::NoCache);
 const QUrl url = QUrl::fromUserInput(argv[1]);
 //view.page()->setViewportSize(QSize(width, height));
 view.resize(width, height);
 view.settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
 view.load(url);
 view.show();
 //view.hide();
 pthread_mutex_lock(&data->mutex);
 QImage image(&data->data, width, height, QImage::Format_RGBA8888);
 image.fill(0);
 QPainter painter(&image);
 pthread_mutex_unlock(&data->mutex);
 if (url.isLocalFile()) {
  init_inotify(argv[1]);
 }
 while (!done) {
  app.processEvents();
  pthread_mutex_lock(&data->mutex);
  image.fill(0);
  //page.mainFrame()->render(&painter, QWebFrame::ContentsLayer);
  view.page()->view()->render(&painter);
  pthread_mutex_unlock(&data->mutex);
  // reload file if changed
  if (refresh) {
   refresh = 0;
   view.load(url);
  }
  usleep(1000000 / fps);
 }
 if (url.isLocalFile()) {
  uninit_inotify();
 }
 uninit_shared_data(atoi(argv[2]), atoi(argv[3]));
 return 0;
}
