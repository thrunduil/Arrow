#include "../allocator.h"
#include <iostream>
#include "../region_allocator.h"
#include "boost/pool/pool.hpp"
#include "../timer.h"

using namespace region;

class X
{
    public:
        int m, n;
        X(int k) 
        {
            m  = k;
        };
        X() 
        {
        };
        ~X()
        {
            //std::cout << "X::~X()\n";
        };

};
template<class T>
T minimum(T a, T b)
{
    return a<b?a:b;
};

int main(int argc, const char* argv[])
{  
    typedef double type;

    size_t N = 1000000;

    double t_b = 10000, t_a = 10000;
    for(int i = 0; i < 1000; i++)
    {
        ::region::region reg;

        tic();
        for (size_t i = 0; i < N; ++i)
        {
            type* t = new(reg,get_ptr<type>()) type(1);
        };
        t_a = minimum(toc(),t_a);
    };
    for(int i = 0; i < 1000; i++)
    {
        boost::pool<> pool(sizeof(type));
        tic();
        for (size_t i = 0; i < N; ++i)
        {
            type* t = new(pool.malloc()) type(i);
        };
        t_b = minimum(toc(),t_b);        
    };    

    std::cout << t_a/t_b << "\n"; 

    tic();
    for (size_t i = 0; i < N; ++i)
    {
        //*(size_t*)malloc(sizeof(type)) = i;
    };
    std::cout << toc() << "\n";     


    try
    {
        X* ptr = new(current_region,get_ptr<X>()) X[9];
    }
    catch(...)
    {
    };

    try
    {
        X* ptr = new(current_region,get_ptr<X>()) X[9];
    }
    catch(...)
    {
    };

	return 0;
}

