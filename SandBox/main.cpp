#include <conio.h>

struct SharedResource
{
	SharedResource() :
		prms(), fut()

	{
		this->fut = this->prms.get_future();
	}
	//These are resources that are shared between threads
	std::promise<void> prms;
	std::future<void> fut;
};
struct ThreadCount
{
	ThreadCount()

	{

		std::lock_guard<std::mutex> lock(this->mtx);
		++this->counter;
		Debug::out("TC() %d \n", this->counter);

	}

	~ThreadCount()
	{

		std::lock_guard<std::mutex> lock(this->mtx);
		--this->counter;
		Debug::out("~TC() %d \n", this->counter);

		if (ThreadCount::counter == 0)
		{
			cv_thread_count.notify_one();
		}

	}
	/*
	The static variable "counter" in the TC class is a shared resource among the four threads,
	which can potentially result in a race condition. Therefore,
	it is necessary to implement protection mechanisms to ensure its proper synchronization.

	*/
	static size_t counter;
	static std::mutex mtx;
	static std::condition_variable  cv_thread_count;


};
size_t ThreadCount::counter = 0;
std::mutex ThreadCount::mtx;
std::condition_variable  ThreadCount::cv_thread_count;


class A : public BannerBase
{

public:
	A() = delete;
	A(const A&) = default;


	A& operator=(const A&) = default;
	~A() = default;
	A(const char* const name) :
		BannerBase(name), counter(0)
	{

	}
	A(A&&) = default;

	void operator ()(SharedResource& sharedResource) //Functor 
	{
		START_BANNER;
		ThreadCount tcA;
		while (true)
		{

			std::future_status status = sharedResource.fut.wait_for(1ms);
			if (status == std::future_status::ready)
			{
				break;
			}

			Debug::out("%d \n", this->counter);
			++(this->counter);


		}

	}

private:
	int counter;

};
class B : public BannerBase
{
public:
	B() = delete;
	B& operator=(const B&) = default;
	B(const B&) = default;
	~B() = default;
	B(const char* const name) :
		BannerBase(name), counter(0x10000)
	{

	}
	void operator()(SharedResource& sharedResource)
	{
		START_BANNER;
		ThreadCount tcB;
		while (true)
		{


			std::future_status status = sharedResource.fut.wait_for(1ms);
			if (status == std::future_status::ready)
			{
				break;
			}

			Debug::out("0x%x \n", this->counter);
			this->counter = this->counter - 1;

		}
	}

private:
	unsigned int counter;


};

class C : public BannerBase
{

public:

	C() = delete;
	C(const C&) = default;
	C& operator=(const C&) = default;
	~C() = default;
	C(const char* const name) :
		BannerBase(name), strings{ "apple", "orange" ,"banana", "lemon" }
	{

	}

	void operator() (SharedResource& sharedResource)
	{

		START_BANNER;
		ThreadCount tcC;
		int index{ 0 };
		while (true)
		{


			std::future_status status = sharedResource.fut.wait_for(1ms);
			if (status == std::future_status::ready)
			{

				break;
			}
			Debug::out("%s \n", this->strings[index]);
			++index;
			if (index > 3)
			{
				index = 0;
			}


		}
		std::this_thread::sleep_for(100ms);

	}

private:


	const char* const strings[4];
};

class D : public BannerBase
{
public:
	D() = delete;
	D(const D&) = default;
	D& operator=(const D&) = default;
	virtual ~D() = default;

	D(const char* const name) :
		BannerBase(name)
	{



	}
	void operator() (SharedResource& sharedReference)
	{
		START_BANNER;
		ThreadCount tcD;
		const char* const sourceStr = "<0><1><2><3><4><5><6><7><8>";
		char* destStr = new char[strlen(sourceStr) + 1];
		memcpy(destStr, sourceStr, strlen(sourceStr) + 1);
		size_t len{ strlen(sourceStr) };
		size_t  tempLen = len;
		while (true)
		{


			std::future_status status = sharedReference.fut.wait_for(1ms);
			if (status == std::future_status::ready)
			{

				break;
			}

			if (strlen(destStr) <= 0)
			{

				memcpy(destStr, sourceStr, len);
				tempLen = len;
			}
			Debug::out("%s \n", destStr);
			destStr[--tempLen] = '\0';
			destStr[--tempLen] = '\0';
			destStr[--tempLen] = '\0';
		}

		delete destStr;
	}

};

class Controller : public BannerBase
{
public:
	Controller() = delete;
	Controller(const Controller&) = default;
	Controller& operator=(const Controller&) = default;
	~Controller() = default;
	Controller(const char* const name) :
		BannerBase(name) {}
	void operator()
		(SharedResource& sharedResource)
	{

		START_BANNER;

		sharedResource.prms.set_value();
		std::unique_lock<std::mutex> lockTC(ThreadCount::mtx);
		ThreadCount::cv_thread_count.wait(lockTC);


	}
};

int main()
{
	START_BANNER_MAIN("Main");
	A oA("A");
	B oB("B");
	C oC("C");
	D oD("D");
	Controller controller("Controller");

	//Resources shared between threads 
	SharedResource sharedResource;


	std::future<void> taskA = std::async(std::launch::async, std::move(oA), std::ref(sharedResource));
	std::future<void> taskB = std::async(std::launch::async, std::move(oB), std::ref(sharedResource));
	std::future<void> taskC = std::async(std::launch::async, std::move(oC), std::ref(sharedResource));
	std::future<void> taskD = std::async(std::launch::async, std::move(oD), std::ref(sharedResource));

	//deferred task
	std::future<void> taskController =
		std::async(std::launch::deferred, controller, std::ref(sharedResource));


	//Key Press
	_getch();
	
	//Signal to the tasks
	taskController.get();

	/*
	When a thread is waiting using a condition variable,
	it is not checking the flag, but it is also not consuming any CPU time.
	Instead, the thread is in a sleep state,
	waiting for a notification from another thread that the condition has changed.

	*/

}



