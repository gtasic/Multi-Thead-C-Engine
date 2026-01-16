#pragma once

#include <memory_resource>
#include <array>
#include <vector>
#include <cstddef>

namespace memory{
    
    template <size_t BufferSize = 10*1024*1024>

    class SafePoolThread : public  std::pmr::memory_resource{
    public : 
        SafePoolThread()= default;
        SafePoolThread(const SafePoolThread&) = delete;
        SafePoolThread& operator=(const SafePoolThread&) = delete;

        void release_current_thread(){
            get_local_resource().resource.release();
        }

        protected : 
         
        void* do_allocate(size_t bites, size_t alignement) override{
            return get_local_resource().resource.allocate(bites, alignement);
            
        }
        void do_deallocate(void* p, size_t bites, size_t alignement) override{
            get_local_resource().resource.deallocate(p , bites , alignement);
        }

        bool do_is_equal(const std::pmr::memory_resource& other)const noexcept override {
            return this == &other;
        }

        private : 

        struct SelfPool{
            std::vector<std::byte> buffer;
            std::pmr::monotonic_buffer_resource resource; 

            SelfPool(): buffer(BufferSize), resource(buffer.data(), buffer.size(), std::pmr::null_memory_resource()){

            };
        };

        static SelfPool& get_local_resource(){
            thread_local SelfPool pool;
            return pool;  
        }

    };
}