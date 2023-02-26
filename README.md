# Multi-threading example with shared resources and synchronization in C++

# std::launch::deferred
Using the deferred option of std::sync allows us to delay the execution of the controller task until we receive input indicating a key has been hit. 
This eliminates the need for condition variables, mutexes, and other synchronization mechanisms in the controller task.


This is a C++ code that defines several classes and structs that demonstrate the use of multithreading and synchronization mechanisms in C++. The code defines the following classes and structs:

SharedResource: a struct that represents a shared resource between threads. It contains a std::promise<void> object called prms and a std::future<void> object called fut. The prms object is used to set the value of the fut object once some condition is met.
ThreadCount: a struct that represents a counter for the number of threads that are currently running. It contains a static variable counter that keeps track of the number of threads, a std::mutex object called mtx that provides mutual exclusion for accessing the counter, and a std::condition_variable object called cv_thread_count that is used to signal when the counter becomes zero.
A, B, C, and D: classes that represent different tasks that will be executed by different threads. Each of these classes has an operator() function that takes a SharedResource& parameter and implements the task. The implementation of each task is different, but all tasks use the fut object of the SharedResource parameter to synchronize their execution.
Controller: a class that coordinates the execution of the tasks. It has a run function that creates an instance of each task and launches it in a separate thread. It also sets the value of the fut object of the SharedResource parameter once all threads have finished executing.
The code uses std::mutex and std::condition_variable objects to synchronize the execution of the threads and ensure that the shared resources are accessed in a thread-safe way. The ThreadCount struct is used to keep track of the number of running threads and signal when all threads have finished executing. The SharedResource struct is used to synchronize the execution of the tasks by setting the value of its fut object once all threads have reached a certain point in their execution.
