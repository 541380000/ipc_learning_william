cmake_minimum_required(VERSION 3.19)
project(ipc_learning)

find_package(Threads)
set(CMAKE_CXX_STANDARD 14)
add_executable(ipc_learning PIPE_banshuanggong.cpp "PIP#_quanshuanggong.cpp" FIFO.cpp posix_message_queue.cpp include_all.h posix_message_queue_signal.cpp posix_message_queue_signal_safer.cpp posix_message_queue_signal_safer_faster_sig_wait.cpp posix_realtime_signal.cpp systemV_message_queue.cpp read_write_lock.cpp posix_named_semapehre.cpp posix_memory-bases_semaphere.cpp shared_memory_demo_counter.cpp
        shared_memory_demo_counter2_with_shared_memory_sem.cpp shared_memory_BSD_without_file_demo_counter.cpp shared_memory_posix.cpp)


target_link_libraries(ipc_learning rt ${CMAKE_THREAD_LIBS_INIT})
