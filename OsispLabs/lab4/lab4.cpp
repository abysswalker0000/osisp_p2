#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace {
volatile sig_atomic_t g_counter = 0;

void on_signal(int signal) {
  std::cout << "\nОбработка сигнала " << signal << ". Запуск нового процесса..."
            << std::endl;

  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Ошибка при создании процесса: " << strerror(errno)
              << std::endl;
    std::exit(EXIT_FAILURE);
  } else if (pid > 0) {
    std::cout << "Завершение родительского процесса." << std::endl;
    std::exit(EXIT_SUCCESS);
  } else {
    std::cout << "Дочерний процесс запущен. Текущее значение счетчика: "
              << g_counter << std::endl;
  }
}

void setup_signal_handlers() {
  struct sigaction action = {};
  action.sa_handler = on_signal;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);

  if (sigaction(SIGTERM, &action, nullptr) == -1) {
    std::cerr << "Ошибка настройки обработчика SIGTERM: " << strerror(errno)
              << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if (sigaction(SIGINT, &action, nullptr) == -1) {
    std::cerr << "Ошибка настройки обработчика SIGINT: " << strerror(errno)
              << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void log_counter() {
  std::ofstream file("counter.txt", std::ios::app);
  if (file) {
    file << "Счетчик: " << g_counter << "\n";
  } else {
    std::cerr << "Ошибка открытия файла counter.txt: " << strerror(errno)
              << std::endl;
  }
}
}

int main() {
  setup_signal_handlers();

  while (true) {
    ++g_counter;
    std::cout << "Текущий счетчик: " << g_counter << std::endl;
    log_counter();
    sleep(1);
  }

  return 0;
}