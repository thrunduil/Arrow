#include <iostream>
#include <string>

#include "arrow/modules.h"
#include "debugging.h"

#include <boost/algorithm/string.hpp>

enum class command 
{ 
    CMD_NONE, CMD_TYPE, CMD_KIND, CMD_EVAL, CMD_CLEAR, CMD_HELP, CMD_EXIT 
};

void console_loop()
{
    std::cout << "    -----------------------------------------------" << std::endl
              << "    *         Welcome to the Arrow shell!         *" << std::endl
              << "    *                                             *" << std::endl
              << "    * Type in expressions to execute, or          *" << std::endl
              << "    * :p[rint] <expr> -- to print value of <expr> *" << std::endl
              << "    * :h[elp] <symb>  -- to print info for <symb> *" << std::endl
              << "    * :t[ype] <expr>  -- to print type of <expr>  *" << std::endl
              << "    * :k[ind] <type>  -- to print kind of <type>  *" << std::endl
              << "    * :c[lear]        -- remove all definitions   *" << std::endl
              << "    * :e[xit]         -- to exit the shell.       *" << std::endl
              << "    -----------------------------------------------" << std::endl
              << std::endl;

    arrow::error_handler_ptr eh = arrow::error_handler_ptr(new arrow::error_handler(&std::cout));

    arrow::modules mm(eh, true);

    bool exit   = false;
    command cmd = command::CMD_NONE;

    while (!exit)
    {
        std::cout << "<< ";
        std::cout.flush();

        std::string buffer;           // user input is collected here
        bool needs_more_input = true; // does the parser wait for more user input?
        cmd = command::CMD_NONE;

        while (needs_more_input)
        {
            if (std::cin.eof()) 
            {
                exit = true;
                break;
            }

            std::string line;
            std::getline(std::cin, line);

            std::string command(line);
            boost::algorithm::trim(command);            

            if (buffer.empty() && command.size() > 1 && command[0] == ':')
            {
                if (command[1] == 'e')
                {
                    cmd = command::CMD_EXIT;
                    exit = true;
                    break;
                }
                else if (command[1] == 'p')
                {
                    size_t i = command.find(' ');
                    if (i == command.npos)
                    {
                        std::cout << "what do you want to print?" << std::endl;
                        break;
                    }
                    cmd = command::CMD_EVAL;
                    buffer = command.substr(i);
                    command.clear();
                }
                else if (command[1] == 'h')
                {
                    size_t i = command.find(' ');
                    if (i == command.npos)
                    {
                        std::cout << "what do you want to type?" << std::endl;
                        break;
                    }
                    cmd = command::CMD_HELP;
                    buffer = command.substr(i);
                    command.clear();
                }
                else if (command[1] == 't')
                {
                    size_t i = command.find(' ');
                    if (i == command.npos)
                    {
                        std::cout << "what do you want to type?" << std::endl;
                        break;
                    }
                    cmd = command::CMD_TYPE;
                    buffer = command.substr(i);
                    command.clear();
                }
                else if (command[1] == 'k')
                {
                    size_t i = command.find(' ');
                    if (i == command.npos)
                    {
                        std::cout << "what do you want to type?" << std::endl;
                        break;
                    }
                    cmd = command::CMD_KIND;
                    buffer = command.substr(i);
                    command.clear();
                }
                else if (command[1] == 'c')
                {
                    size_t i = command.find(' ');
                    (void)i;
                    cmd = command::CMD_CLEAR;
                    buffer = "";
                }
            }

            if (buffer.empty() && command.empty())
            {
                break;
            }

            eh->clear_errors();

            buffer = buffer + command + "\n";

            try
            {
                if (cmd == command::CMD_EVAL)
                {
                    mm.eval_term(buffer, needs_more_input);
                }
                else if (cmd == command::CMD_TYPE)
                {
                    mm.print_type(buffer, needs_more_input);
                }
                else if (cmd == command::CMD_KIND)
                {
                    mm.print_kind(buffer, needs_more_input);
                }
                else if (cmd == command::CMD_HELP)
                {
                    mm.print_help(buffer, needs_more_input);
                }
                else if (cmd == command::CMD_CLEAR)
                {
                    mm.clear_definitions();
                    needs_more_input = false;
                }
                else
                {                    
                    mm.process_commands(buffer, needs_more_input);
                }
            }
            catch(std::exception& exc)
            {
                std::string mess = exc.what();
                std::cout << mess << std::endl;
                needs_more_input = false;
            }

            catch(...)
            {
                std::cout << "unknown error" << std::endl;
                needs_more_input = false;
            }

            if (needs_more_input) 
            {
                std::cout << "... ";
                std::cout.flush();
            }

        }
    }

    std::cout << std::endl << "Bye!" << std::endl;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    arrow::debug_heap_manager();
    arrow::disable_crash_dialog();
    arrow::report_crash_to_stderr();

    try
    {
        setvbuf(stdout,NULL,_IONBF,0); // disable stdout / stderr buffering
        setvbuf(stderr,NULL,_IONBF,0); // windows doesnt have auto-flush on end of line

        console_loop();
    }
    catch(std::exception &e)
    {
        std::cout << "exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
