/*
LEETCODE 355 — Design Twitter

다음 연산을 지원하는 Twitter 클래스를 구현하라.

- postTweet(userId, tweetId): 새 트윗을 게시한다. tweetId는 고유하다.
- getNewsFeed(userId): 자신과 followee의 최신 트윗 ID를 최대 10개 반환한다.
- follow(followerId, followeeId): follow 관계를 추가한다.
- unfollow(followerId, followeeId): follow 관계를 제거한다.
- 모든 public 멤버 함수는 thread-safe해야 한다.

사용자별 트윗 목록은 이미 시간순으로 정렬되어 있다. 피드를 만들 때 각 작성자의
최신 트윗만 최대 힙에 넣고, 하나를 꺼낼 때 같은 작성자의 바로 이전 트윗을 넣는
k-way merge를 사용한다.
*/

#include <cstddef>
#include <vector>
#include <cassert>
#include <barrier>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <array>

class Twitter {
private:
    using Tweet = std::pair<int, int>; //id, time
    int now_ = 0;
    std::unordered_map<int, std::vector<Tweet>> tweets_; // userId, Tweet
    std::unordered_map<int, std::unordered_set<int>> followings_; // follower / followings

    std::mutex mutex_;

  public:
    void postTweet(int userId, int tweetId) {
        std::lock_guard lock(mutex_);

        tweets_[userId].emplace_back(tweetId, now_++);
    }

    std::vector<int> getNewsFeed(int userId) {
        std::lock_guard lock(mutex_);

        std::priority_queue<std::array<int, 4>> heap; // {time, tweetId, author, index}

        auto add_latest = [&](int userId) -> void {
            auto found = tweets_.find(userId);
            if(found == tweets_.end() || found->second.empty()) {
                return;
            }

            const int latest_index = found->second.size() - 1;
            const auto [tweetId, time] = found->second[latest_index];

            heap.push({time, tweetId, userId, latest_index});
        };

        add_latest(userId);
        for(const int followeeId: followings_[userId]) {
            add_latest(followeeId);
        }

        std::vector<int> result;

        while(!heap.empty() && result.size() < 10) {
            const auto [time, tweet_id, author, index] = heap.top();
            heap.pop();
            result.push_back(tweet_id);

            if(index > 0) {
                const auto [next_tweet_id, next_time] = tweets_.at(author)[index - 1];
                heap.push({next_time, next_tweet_id, author, index - 1});
            }
        }

        return result;
    }

    void follow(int followerId, int followeeId) {
        std::lock_guard lock(mutex_);

        if(followerId == followeeId) {
            return;
        }

        followings_[followerId].emplace(followeeId);
    }

    void unfollow(int followerId, int followeeId) {
        std::lock_guard lock(mutex_);

        auto found = followings_.find(followerId);
        if(found == followings_.end()) {
            return;
        }

        found->second.erase(followeeId);
    }
};

void testBasicFlow() {
    Twitter twitter;

    twitter.postTweet(1, 5);
    assert(twitter.getNewsFeed(1) == std::vector<int>{5});

    twitter.follow(1, 2);
    twitter.postTweet(2, 6);
    assert((twitter.getNewsFeed(1) == std::vector<int>{6, 5}));

    twitter.unfollow(1, 2);
    assert(twitter.getNewsFeed(1) == std::vector<int>{5});
}

void testConcurrentPosts() {
    constexpr int threadCount = 10;
    Twitter twitter;
    std::barrier startLine(threadCount);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i] {
            startLine.arrive_and_wait();
            twitter.postTweet(1, 100 + i);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const auto feed = twitter.getNewsFeed(1);
    const std::unordered_set<int> uniqueTweets(feed.begin(), feed.end());
    assert(feed.size() == threadCount);
    assert(uniqueTweets.size() == threadCount);
}

int main() {
    testBasicFlow();
    testConcurrentPosts();
}
