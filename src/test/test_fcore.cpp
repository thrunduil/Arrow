#include "test_fcore.h"
#include "arrow/modules.h"

#include <iostream>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

struct tester_fcore
{
    using error_handler_ptr = arrow::error_handler_ptr;

    void                make();
    void                parse_dir(const std::string& path, bool succ);

    error_handler_ptr   get_error_handler();
    error_handler_ptr   get_error_handler_null();
};

tester_fcore::error_handler_ptr 
tester_fcore::get_error_handler()
{
    static error_handler_ptr eh = error_handler_ptr(new arrow::error_handler(&std::cout));
    return eh;
}

tester_fcore::error_handler_ptr 
tester_fcore::get_error_handler_null()
{
    static error_handler_ptr eh = error_handler_ptr(new arrow::error_handler(nullptr));
    return eh;
}

void tester_fcore::make()
{
    std::string lib_path    = "C:/coding/public/arrow/src/test/fcore/tests";

    std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/equality.f";
    //std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/bools_with_existentials.f";
    //std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/primitive.f";
    //std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/test.f";
    //std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/test_existentials.f";
    //std::string path    = "C:/coding/public/arrow/src/test/fcore/tests/test_module.f";

    arrow::ast::term x  = arrow::ast::int_literal(arrow::ast::location(), 1);
    arrow::ast::term y  = arrow::ast::int_literal(arrow::ast::location(), 2);
    x                   = y;

    try
    {
        arrow::modules mm(get_error_handler(), false);
        mm.add_lib_path(lib_path);

        mm.parse_module(path);
        //mm.parse_module(path2);

        std::vector<arrow::ast::term> evaled;
        mm.process_modules(evaled);
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << "\n";
    }

    parse_dir("C:/coding/public/arrow/src/test/fcore/tests", true);

    std::cout << "finished" << "\n";
};

void tester_fcore::parse_dir(const std::string& d, bool succ)
{
    namespace fs = boost::filesystem;

    std::string lib_path    = "C:/coding/sources/symbolic/src/test/fcore/tests";

    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_itr(d); dir_itr != end_iter; ++dir_itr)
    {
        std::string ext  = dir_itr->path().extension().string();

        if (ext != ".f")
            continue;

        std::string path    = dir_itr->path().string();

        error_handler_ptr eh = succ ? get_error_handler() : get_error_handler_null();
        eh->clear_errors();

        try
        {
            arrow::modules mm(eh, false);
            mm.parse_module(path);
            mm.add_lib_path(lib_path);

            std::vector<arrow::ast::term> evaled;
            mm.process_modules(evaled);

            /*
            bool ok = mm.has_errors() == false;

            if (succ != ok)
                std::cout << path << ": FAILED" << "\n";
            else
            {
                //std::cout << path << ": OK" << "\n";
            };
            */
        }
        catch(std::exception& ex)
        {
            if (succ == true)
            {
                std::cout << path << ":" << "\n" << "\t";
                std::cout << ex.what() << "\n";
            }
        };
    };
};

void test_fcore()
{
    tester_fcore tsm;
    tsm.make();
};
