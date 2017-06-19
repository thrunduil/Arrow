/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "arrow/modules.h"
#include "grammar/output/lexer_arrow.hpp"
#include "grammar/output/parser_arrow.hpp"
#include "antlr/antlr.h"
#include "scopes/module.h"
#include "scopes/graph.h"
#include "lib_paths.h"

#include <fstream>
#include <memory>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace arrow { namespace details
{

class module_info
{
    private:
                            /// import x alias x location
        using string_loc    = std::tuple<ast::unique_string, ast::unique_string, ast::location>;
        using string_vec    = std::vector<string_loc>;
        using module_ast    = ast::definitions;

    private:
        ast::unique_string  m_path;
        module_ast          m_ast;
        std::string         m_module_name;
        string_vec          m_imports;

    public:
        module_info();

        module_info(ast::unique_string file_path, const module_ast& amod,
                        const std::string& module_name, const string_vec& imports);

        void                add_imports(const std::vector<ast::unique_string>& default_imp);
        const std::string&  get_module_name() const;
        const module_ast&   get_ast() const;
        const string_vec&   get_imports() const;
};

class build_data
{
    private:
        using module_map    = std::map<ast::unique_string, module_info>;
        using build_order   = std::vector<ast::unique_string>;
        using named_mod_map = std::map<ast::unique_string, module>;
        using import_graph  = var_graph<ast::unique_string>;

    public:        
        import_graph        m_import_graph;
        build_order         m_build_order;
        module_map          m_module_map;
        named_mod_map       m_named_mod_map;
        module              m_shell_mod;

    public:
        build_data(const module_map& map)
            :m_module_map(map)
        {};

        void remove_shell_module(const module& mod)
        {
            m_shell_mod             = mod;
            ast::unique_string name = mod.get_module_name();
            m_named_mod_map.erase(name);
        }
};

class modules_impl : public std::enable_shared_from_this<modules_impl>
{
    private:
        using module_map    = std::map<ast::unique_string, module_info>;
                            /// import x alias x location
        using string_loc    = std::tuple<ast::unique_string, ast::unique_string, ast::location>;
        using string_vec    = std::vector<string_loc>;
        using string_loc_map= std::map<ast::unique_string, ast::location>;
        using named_mod_map = std::map<ast::unique_string, module>;
        using module_ast    = ast::definitions;
        using components_vec= std::vector<std::vector<ast::unique_string>>;
        using build_order   = std::vector<ast::unique_string>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;
        using overload_type_set
                            = ad::overload_set<ad::overload_type_config>;

    private:
        error_handler_ptr   m_eh;        
        module_map          m_module_map;
        named_mod_map       m_processed_mod_map;
        long                m_console_input_num;
        long                m_var_number;
        lib_paths           m_lib_paths;
        bool                m_initialized_shell;
        bool                m_in_shell;

    public:
        modules_impl(const error_handler_ptr& eh, bool in_shell);
        ~modules_impl();

        bool                is_shell() const;
        void                add_lib_path(const std::string& dir);

        error_handler_ptr   get_error_handler() const;

        ast::module_definition  parse_module(const std::string& path);
        ast::module_definition  parse_module(std::istream& os, const std::string& path_long,
                                    const std::string& path_short);

        void                process_modules(std::vector<ast::term>& evaled);
        
        void                check_constraints(const ast::definitions& ast) const;
        void                add_default_imports(build_data& bd);
        void                link_modules(build_data& bd);
        void                collect_imports(build_data& bd, string_loc_map& imports) const;
        void                build_additional_imports(build_data& bd, const string_loc_map& imports) const;
        void                build_scopes(build_data& bd);
        void                desugaring(build_data& bd);
        void                build_dependency_graph(build_data& bd);
        void                build_closures(build_data& bd);
        void                type_check(build_data& bd);
        void                eval(std::vector<ast::term>& evaled, build_data& bd);
        void                finish_processing(build_data& bd);

        /// shell
        void                eval_term(const std::string& command, bool& need_input);
        void                print_type(const std::string& command, bool& need_input);
        void                print_kind(const std::string& command, bool& need_input);
        void                print_help(const std::string& command, bool& need_input);
        void                process_definition(const std::string& command, 
                                bool& need_more_input, std::vector<ast::term>& evaled);
        void                process_commands(const std::string& command, 
                                bool& need_more_input);

        /// get module with given name; return null module if not exist
        module              get_module_by_name(ast::unique_string name) const;

        /// get module with given name; throw exception if cannot be found
        module              get_module_by_index(ad::module_index mi);

    private:        
        ast::module_definition  
                            parse_module(const std::string& path, module_map& mmap) const;
        ast::module_definition  
                            parse_module(std::istream& os, const std::string& path_long,
                                const std::string& path_short, module_map& mmap) const;

        /// start compiling new file
        void                process_file(ast::unique_string path, module_map& mmap) const;

        void                process_new_module(const ast::module_definition& mod, 
                                ast::unique_string path, module_map& mmap) const;

        void                parse_console_input(const std::string& command, bool& need_more_input,
                                std::vector<ast::term>& evaled);
        void                process_modules(std::vector<ast::term>& evaled, build_data& bd);

        /// clear existing modules
        void                clear_processed_modules();
        void                clear_module(module& mod);
        void                remove_processed_module(module& mod, build_data& bd);
        
        /// prepare module with given name
        void                make_module(ast::unique_string mod_name, const module_ast& ast, 
                                const string_vec& imports, build_data& bd);

        /// make build order; also check import cycles; print error if 
        /// found import cycles
        void                make_build_order(const components_vec& components, 
                                build_order& bo) const;

        std::string         get_module_path(ast::unique_string import, bool& found) const;
        void                make_default_imports(std::vector<ast::unique_string>& imp) const;

        /// shell
        ast::unique_string  get_shell_module_name() const;
        long                get_var_number();
        std::string         get_fresh_variable();
        ast::type           get_console_type(const std::string& str) const;
        ast::kind           get_console_kind(const std::string& str) const;
        ast::location       get_shell_location() const;
        void                print_type_definition(const ad::type_alias_info& def) const;
        void                print_term_definition(const ad::symbol_definition_env& def) const;
        void                print_term_definition(const overload_set& def) const;
        void                print_type_definition(const overload_type_set& def) const;
        void                print_operator_declarations(const oper::operator_declaration& oper) const;
        ast::unique_string  parse_identifier(const std::string& command,
                                bool& has_errors) const;
};

};};
