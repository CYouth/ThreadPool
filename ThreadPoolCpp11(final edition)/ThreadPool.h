#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

// C++11 版的 线程池
namespace zl
{
    //线程数组管理类，RAII
    class ThreadsGuard
    {
    public:
        ThreadsGuard(std::vector<std::thread>& v)
            : threads_(v)
        {
            
        }

        ~ThreadsGuard()
        {
            for (size_t i = 0; i != threads_.size(); ++i)
            {
                if (threads_[i].joinable())
                {
                    threads_[i].join();//回收线程资源
                }
            }
        }
    private:
        //移动
        ThreadsGuard(ThreadsGuard&& tg) = delete;
        ThreadsGuard& operator = (ThreadsGuard&& tg) = delete;
        //默认
        ThreadsGuard(const ThreadsGuard&) = delete;
        ThreadsGuard& operator = (const ThreadsGuard&) = delete;
    private:
        std::vector<std::thread>& threads_;
    };


    class ThreadPool
    {
    public:
        typedef std::function<void()> task_type;//任务

    public:
        explicit ThreadPool(int n = 0);//显式调用

        ~ThreadPool()
        {
            stop();
            cond_.notify_all();
        }

        void stop()
        {
            stop_.store(true, std::memory_order_release);
        }

        template<class Function, class... Args>
        std::future<typename std::result_of<Function(Args...)>::type> add(Function&&, Args&&...);

    private:
        //移动
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator = (ThreadPool&&) = delete;
        //默认
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;

    private:
        std::atomic<bool> stop_;//是否运行，原子变量，虽然不在多线程环境中
        std::mutex mtx_;//互斥量
        std::condition_variable cond_;//条件变量

        std::queue<task_type> tasks_;//任务队列
        std::vector<std::thread> threads_;//线程组
        zl::ThreadsGuard tg_;//线程组管理类
    };


    inline ThreadPool::ThreadPool(int n)
        : stop_(false)
        , tg_(threads_)
    {
        int nthreads = n;
        if (nthreads <= 0)
        {
            //获取硬件支持的并发线程数,正常返回支持的并发线程数，若值非良定义或不可计算，则返回​0​
            nthreads = std::thread::hardware_concurrency();
            nthreads = (nthreads == 0 ? 2 : nthreads);
        }

        for (int i = 0; i != nthreads; ++i)
        {
            threads_.push_back(std::thread([this]{
                while (!stop_.load(std::memory_order_acquire))
                {
                    task_type task;
                    {
                        std::unique_lock<std::mutex> ulk(this->mtx_);
                        this->cond_.wait(ulk, [this]{ return stop_.load(std::memory_order_acquire) || !this->tasks_.empty(); });
                        if (stop_.load(std::memory_order_acquire))//线程池stop
                            return;//线程结束
                        task = std::move(this->tasks_.front());//移动
                        this->tasks_.pop();
                    }
                    task();//执行任务
                }
            }));
        }
    }

    //添加任务，返回future
    template<class Function, class... Args>//变长参数模板
    std::future<typename std::result_of<Function(Args...)>::type>
        ThreadPool::add(Function&& fcn, Args&&... args)//完美转发？
    {
        typedef typename std::result_of<Function(Args...)>::type return_type;//函数返回类型
        typedef std::packaged_task<return_type()> task;//类型定义，异步任务

        auto t = std::make_shared<task>(std::bind(std::forward<Function>(fcn), std::forward<Args>(args)...));//本质的实体是bind的内容
        auto ret = t->get_future();//get future
        {
            std::lock_guard<std::mutex> lg(mtx_);
            if (stop_.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");
            tasks_.emplace([t]{(*t)(); });//添加任务队列
        }
        cond_.notify_one();//这个不用加锁
        return ret;
    }
}

#endif  /* THREAD_POOL_H */