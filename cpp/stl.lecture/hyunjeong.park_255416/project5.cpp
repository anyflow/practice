#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace placeholders;

#define NEW_THREAD true

using HANDLER = function<void(void*)>;

struct priority_handler {
  int priority;
  HANDLER handler;
  bool newThread;

  priority_handler(HANDLER& handler, int priority, bool newThread)
      : handler(handler), priority(priority), newThread(newThread) {}

  bool operator<(const priority_handler& rhs) const { return priority < rhs.priority; }

  void handle(void* param) const {
    if (newThread) {
      future<void> ft = async(launch::async, bind(handler, param));
      ft.get();
    } else {
      handler(param);
    }
  }
};

class NotificationCenter {
  map<string, priority_queue<priority_handler>> notif_map;

public:
  void Register(string key, HANDLER h, int priority, bool newThread = false) {
    notif_map[key].push(priority_handler(h, priority, newThread));
  }

  void Notify(string key, void* param) {
    auto q = notif_map[key];

    while (!q.empty()) {
      q.top().handle(param);
      q.pop();
    }
  }
};

void foo(void* p, string name) {
  cout << name << " : " << this_thread::get_id() << endl;

  this_thread::sleep_for(3s);

  cout << name << " end" << endl;
}

int main() {
  NotificationCenter nc;

  // Register 의 3번째 인자는 우선순위, 4번째 인자는 스레드사용여부 입니다.
  // 우선순위 1일 제일 낮고, 3일 제일 높다가 가정합니다.
  // NEW_THREAD 지정시 해당 함수는 새로운 스레드로 동작해야 합니다.
  nc.Register("TEST_EVENT", bind(&foo, _1, "one"s), 1);
  nc.Register("TEST_EVENT", bind(&foo, _1, "two"s), 2, NEW_THREAD);
  nc.Register("TEST_EVENT", bind(&foo, _1, "three"s), 3);

  nc.Notify("TEST_EVENT", (void*)0); // 등록된 순서가 아닌 우선순위가
                                     // 높은 순서로 실행되게 해주세요
                                     // NEW_THREAD 옵션을 가진 경우. 새로운
                                     // 스레드로 해당 함수를 동작되게 해주세요.

  getchar();
}