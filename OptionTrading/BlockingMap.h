#pragma once
#include <condition_variable>
#include <map>

template <typename Key, typename Value>
class BlockingMap
{
public:
	BlockingMap() :mutex_(), condvar_(), map_() {

	}
	~BlockingMap() {};
	BlockingMap(const BlockingMap&) = delete;
	BlockingMap& operator=(const BlockingMap&) = delete;
	Value& at(Key key) {
		std::unique_lock<std::mutex> lck(mutex_);
		while (map_.find(key) == map_.end()) {
			condvar_.wait(lck);
		}
		return map_.at(key);
	}
	void insert(Key key, Value value) {
		std::lock_guard<std::mutex> lck(mutex_);
		map_[key] = value;
		condvar_.notify_all();
	}
private:
	mutable std::mutex mutex_;
	std::condition_variable condvar_;
	std::map<Key, Value> map_;
};

