#pragma once
#include<queue>
#include<chrono>
#include<thread>
#include<mutex>
#include<condition_variable>
template<class Ctpye>
class ConnPool
{
public:
	 ConnPool(int min, int max);
	void start();
	std::shared_ptr<Ctpye> getConn();
	virtual ~ConnPool();
private:
	virtual Ctpye* connect() = 0;
	void moveManager();
	void addManager();

	std::queue < std::pair<std::chrono::steady_clock::time_point, Ctpye*>> _connQ;
	int _min;
	int _max;
	long long _maxIdleTime = 3000; 
	std::thread* _addManagerTh;
	std::thread* _moveManagerTh;
	std::mutex _mtx;
	std::condition_variable _cond;
	bool _flag;
};

template<class Ctpye>
inline ConnPool<Ctpye>::ConnPool(int min, int max):_min(min),_max(max),_flag(false)
{
	_addManagerTh = nullptr;
	_moveManagerTh = nullptr;
}

template<class Ctpye>
inline void ConnPool<Ctpye>::start()
{
	for (int i = 0; i < _min; i++)
	{
		_connQ.push(std::make_pair(std::chrono::steady_clock::now(), connect()));
	}

	_addManagerTh = new std::thread(&ConnPool<Ctpye>::addManager, this);
	_moveManagerTh = new std::thread(&ConnPool<Ctpye>::moveManager, this);
}

template<class Ctpye>
inline std::shared_ptr<Ctpye> ConnPool<Ctpye>::getConn()
{
	std::unique_lock<std::mutex> locker(_mtx);
	while (_connQ.empty())
	{
		if (std::cv_status::timeout == _cond.wait_for(locker, std::chrono::milliseconds(1000)))
		{
			if (_connQ.empty())
			{
				continue;
			}
		}
	}

	Ctpye* conn = _connQ.front().second;
	_connQ.pop();
	return std::shared_ptr<Ctpye>(conn, [this](Ctpye* con) {
		_connQ.push(std::make_pair(std::chrono::steady_clock::now(),con));
		});
}

template<class Ctpye>
inline ConnPool<Ctpye>::~ConnPool()
{
	_flag = true;
	if (_moveManagerTh)
	{
		_moveManagerTh->join();
	}
	if (_addManagerTh)
	{
		_addManagerTh->join();
	}
	while(!_connQ.empty())
	{
		delete _connQ.front().second;
		_connQ.pop();
	}
}



template<class Ctpye>
inline void ConnPool<Ctpye>::moveManager()
{
	while (!_flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(700));
		std::lock_guard<std::mutex> locker(_mtx);
		while (_connQ.size() > _min)
		{
			auto res = std::chrono::steady_clock::now() - _connQ.front().first;
			auto aliveTime = std::chrono::duration_cast<std::chrono::milliseconds>(res);
			if (aliveTime.count() >= _maxIdleTime)
			{
				delete _connQ.front().second;
				_connQ.pop();
			}
			else
			{
				break;
			}
		}
	}
}

template<class Ctpye>
inline void ConnPool<Ctpye>::addManager()
{
	while (!_flag)
	{
		std::unique_lock<std::mutex> locker(_mtx);
		while (_connQ.size() >= _max)
		{
			_cond.wait(locker);
		}
		_connQ.push(std::make_pair(std::chrono::steady_clock::now(), connect()));
		_cond.notify_all();
	}
}
