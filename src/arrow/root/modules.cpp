/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
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
#include "modules_impl.h"
#include "arrow/parser.h"
#include "scopes/module_impl.h"
#include "semantics/semantics.h"
#include "scopes/overload_set.h"
#include "semantics/unique_names_provider.h"

namespace arrow { namespace details
{

//---------------------------------------------------------
//                  module_info
//---------------------------------------------------------
module_info::module_info()
{};

module_info::module_info(ast::unique_string file_path, const module_ast& amod,
                const std::string& module_name, const string_vec& imports)
    :m_path(file_path), m_ast(amod), m_module_name(module_name), m_imports(imports)
{}

const std::string& module_info::get_module_name() const
{
    return m_module_name;
}
const module_info::module_ast& module_info::get_ast() const
{
    return m_ast;
}
const module_info::string_vec& module_info::get_imports() const
{
    return m_imports;
}

void module_info::add_imports(const std::vector<ast::unique_string>& default_imp)
{
    for (ast::unique_string imp : default_imp)
        m_imports.push_back(string_loc(imp, imp, ast::location()));
};

//---------------------------------------------------------
//                  modules_impl
//---------------------------------------------------------
modules_impl::modules_impl(const error_handler_ptr& eh, bool in_shell)
    :m_eh(eh), m_console_input_num(0), m_var_number(0)
    ,m_initialized_shell(false), m_in_shell(in_shell)
{
    if (!m_eh)
        m_eh    = error_handler_ptr(new error_handler(&std::cout));
};

bool modules_impl::is_shell() const
{
    return m_in_shell;
}

void modules_impl::add_lib_path(const std::string& dir)
{
    m_lib_paths.add_lib_path(dir);
};

error_handler_ptr modules_impl::get_error_handler() const
{
    return m_eh;
}

modules_impl::~modules_impl()
{
    clear_processed_modules();
};

ast::module_definition modules_impl::parse_module(const std::string& path)
{
    return parse_module(path, m_module_map);
};

ast::module_definition modules_impl::parse_module(const std::string& path, module_map& mmap) const
{
    std::ifstream file(path);

    if (!file)
    {
        std::string s = std::string(path) + " not found\n";
        throw std::runtime_error(s);
    };

    //avoid using long paths
    namespace fs = boost::filesystem;

    fs::path fp(path);

    return parse_module(file, path, fp.filename().string(), mmap);
};

ast::module_definition modules_impl::parse_module(std::istream& file, const std::string& path_long,
                           const std::string& path_short)
{
    return parse_module(file, path_long, path_short, m_module_map);
};

ast::module_definition modules_impl::parse_module(std::istream& file, const std::string& path_long,
                           const std::string& path_short, module_map& mmap) const
{
    auto eh_scope = m_eh->push_error_scope();    

    ast::unique_string file_path_short(path_short);
    ast::unique_string file_path_long(path_long);

    process_file(file_path_long, mmap);

    parser parse(m_eh);

    ast::module_definition defs = parse.parse_module(file, path_short);

    process_new_module(defs, file_path_long, mmap);    
    return defs;
};
ast::type modules_impl::get_console_type(const std::string& str) const
{
    using loc_vec   = std::vector<ast::location>;

    module shell    = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return ast::type();

    bool found;
    loc_vec ambig_loc;
    const overload_set&
    def             = shell.get_symbol_definition(ast::unique_string(str), true, 
                        found, ambig_loc);

    if (found == false || ambig_loc.size() > 0)
        return ast::type();

    ad::def_location def_loc;
    const symbol_definition_env* sym_def    = def.get_definition(0, def_loc);

    if (!sym_def)
        return ast::type();
    if (sym_def->has_error() == true)
        return ast::type();

    return sym_def->get_signature();
};

ast::kind modules_impl::get_console_kind(const std::string& str) const
{
    using loc_vec   = std::vector<ast::location>;
    module shell    = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return ast::kind();

    bool found;
    loc_vec ambig_loc;
    const overload_type_set&
    def             = shell.get_type_definition(ast::unique_string(str), true, 
                        found, ambig_loc);

    if (found == false || ambig_loc.size() > 0)
        return ast::kind();

    ad::def_location def_loc;
    const type_alias_info* sym_def  = def.get_definition(0, def_loc);

    if (!sym_def)
        return ast::kind();
    if (sym_def->has_error() == true)
        return ast::kind();

    return sym_def->get_kind();
};

ast::unique_string modules_impl::get_shell_module_name() const
{
    static ast::unique_string shell_name = ast::unique_string(":shell");
    return shell_name;
};

ast::unique_string modules_impl::parse_identifier(const std::string& command,
                                                  bool& has_errors) const
{   
    ast::unique_string mod_name = get_shell_module_name();

    auto scope = m_eh->push_error_scope();

    parser parse(m_eh);

    ast::identifier id  = parse.parse_identifier(command, get_shell_location());

    if (m_eh->has_errors())
    {
        has_errors = true;
        return ast::unique_string();
    };

    has_errors = false;
    return id.get_name_simple();
};

void modules_impl::parse_console_input(const std::string& command, bool& need_more_input,
                                       std::vector<ast::term>& evaled)
{
    auto eh_scope               = m_eh->push_error_scope();    
    ast::unique_string mod_name = get_shell_module_name();

    parser parse(m_eh);

    ast::module_definition mod = parse.parse_console_input(command, mod_name.get_string(), 
                                        m_console_input_num, need_more_input);

    if (need_more_input == true)
    {
        m_eh->clear_errors();
        return;
    };

    if (m_eh->has_errors() == true)
        return;

    const ast::definitions& defs    = mod.get_definitions();
    check_constraints(defs);
    
    const string_vec& imports       = mod.get_imports();

    module_info mod_info(mod_name, defs, mod_name.get_string(), imports);

    module_map mm;
    mm[mod_name] = mod_info;

    build_data bd(mm);
    process_modules(evaled, bd);
};

void modules_impl::process_new_module(const ast::module_definition& mod, 
                            ast::unique_string file_path, module_map& mmap) const
{
    if (m_eh->has_errors() == true)
        return;

    const ast::definitions& defs    = mod.get_definitions();
    check_constraints(defs);

    const std::string& module_name  = mod.get_module_name();
    ast::unique_string mod_name(module_name);
    const string_vec& imports       = mod.get_imports();

    mmap[file_path] = module_info(file_path, defs, module_name, imports);
};

void modules_impl::process_file(ast::unique_string path, module_map& mmap) const
{
    mmap.erase(path);
};

void modules_impl::clear_processed_modules()
{
    for (auto& pos : m_processed_mod_map)
        clear_module(pos.second);

    m_processed_mod_map.clear();
};

void modules_impl::clear_module(module& mod)
{
    mod.remove_cyclical_data();
};

struct modules_provider_impl : details::modules_provider
{
    const modules_impl* m_modules;

    modules_provider_impl(const modules_impl* mod)
        : m_modules(mod)
    {};

    virtual module get_module(ast::unique_string name) const override
    {
        return m_modules->get_module_by_name(name);
    };
};

module modules_impl::get_module_by_index(ad::module_index mi)
{
    module ret = get_module_by_name(mi.get_value());

    if (ret.is_null() == true)
        throw std::runtime_error("invalid module index");

    return ret;
};

void modules_impl::check_constraints(const ast::definitions& ast) const
{
    return sema::check_constraints(ast, m_eh);
}

void modules_impl::process_modules(std::vector<ast::term>& evaled)
{
    build_data bd(m_module_map);
    process_modules(evaled, bd);
    m_module_map.clear();
};

void modules_impl::process_modules(std::vector<ast::term>& evaled, build_data& bd)
{
    ad::unique_names_provider::get().clear();

    add_default_imports(bd);
    link_modules(bd);
    build_scopes(bd);
    desugaring(bd);    
    build_dependency_graph(bd);
    build_closures(bd);
    type_check(bd);
    eval(evaled, bd);

    finish_processing(bd);

    (void)evaled;
};

void modules_impl::add_default_imports(build_data& bd)
{
    std::vector<ast::unique_string> def_imp;
    make_default_imports(def_imp);

    for (auto& pos : bd.m_module_map)
    {
        bool is_shell   = pos.first == get_shell_module_name();

        if (is_shell == true)
        {
            if (m_initialized_shell == false)
            {
                //for shell module we must add predefined imports only ones
                pos.second.add_imports(def_imp);
                m_initialized_shell = true;
            };
        }
        else
        {
            pos.second.add_imports(def_imp);
        };
    };
};

void modules_impl::link_modules(build_data& bd)
{
    m_eh->clear_errors();

    string_loc_map all_imports;
    collect_imports(bd, all_imports);

    build_additional_imports(bd, all_imports);

    if (m_eh->has_errors() == true)
    {
        bd.m_module_map.clear();
        return;
    };

    for (const auto& pos : bd.m_module_map)
    {
        const std::string& mod_name = pos.second.get_module_name();
        const module_ast& ast       = pos.second.get_ast();
        const string_vec& imports   = pos.second.get_imports();
        
        make_module(ast::unique_string(mod_name), ast, imports, bd);
    };

    if (m_eh->has_errors() == true)
    {
        bd.m_module_map.clear();
        return;
    };

    //process imports
    {
        //const modules_provider& mods,
        using import_loc = std::pair<ast::unique_string, ast::location>;

        std::set<import_loc> undefined;
        modules_provider_impl mp(this);

        for (auto& pos : bd.m_named_mod_map)
            pos.second.make_imports(mp, undefined, m_eh);

        for (const auto& pos : undefined)
            error::scope_error_msg(m_eh).error_undefined_import(pos.second, pos.first);
    };

    if (m_eh->has_errors() == true)
    {
        bd.m_module_map.clear();
        return;
    };

    //build import graph
    bd.m_import_graph.reserve((long)bd.m_module_map.size());

    for (auto& pos : bd.m_named_mod_map)
        pos.second.add_import_edges(bd.m_import_graph);
    
    components_vec components;
    bd.m_import_graph.strongly_connected_components(components);

    make_build_order(components, bd.m_build_order);

    //collect operator declarations
    for (auto& pos : bd.m_named_mod_map)
        pos.second.collect_operator_declarations(m_eh);

    //collect symbols
    for (auto& pos : bd.m_named_mod_map)
        pos.second.collect_symbol_definitions(m_eh);

    if (m_eh->has_errors() == true)
    {
        bd.m_module_map.clear();
        return;
    };
};

void modules_impl::collect_imports(build_data& bd, string_loc_map& all_imports) const
{
    //collect imports from import declarations
    for (const auto& pos : bd.m_module_map)
    {
        const string_vec& imports   = pos.second.get_imports();
        
        for (const auto& imp : imports)
        {
            ast::unique_string imp_name = std::get<0>(imp);
            ast::location imp_loc       = std::get<2>(imp);
            all_imports[imp_name]       = imp_loc;
        };
    };

    //remove already processed modules
    for (const auto& pos : bd.m_module_map)
    {
        const std::string& mod_name = pos.second.get_module_name();
        all_imports.erase(ast::unique_string(mod_name));
    };
};

void modules_impl::build_additional_imports(build_data& bd, const string_loc_map& imports) const
{
    for (const auto& imp : imports)
    {
        bool found;
        ast::unique_string imp_name = imp.first;
        ast::location loc           = imp.second;
        std::string path            = get_module_path(imp_name, found);

        if (found == false)
        {
            error::scope_error_msg(m_eh).error_unable_find_import_def
                                        (loc, imp_name.get_string());
            continue;
        };

        parse_module(path, bd.m_module_map);
    };
};

void modules_impl::make_build_order(const components_vec& components, build_order& bo) const
{
    bo.clear();

    for (const auto& comp : components)
    {
        if (comp.size() != 1)
            error::scope_error_msg(m_eh).error_cyclical_dependencies_module(comp);
        else
            bo.push_back(comp[0]);
    };
};

void modules_impl::build_scopes(build_data& bd)
{
    for (auto pos = bd.m_named_mod_map.begin(); pos != bd.m_named_mod_map.end();)
    {
        module& mod = pos->second;
        ++pos;

        m_eh->clear_errors();

        mod.build_type_scopes(m_eh);
        sema::build_scopes(mod, m_eh);

        if (m_eh->has_errors() == true)
            remove_processed_module(mod, bd);
    };
}

void modules_impl::desugaring(build_data& bd)
{
    for (auto pos = bd.m_named_mod_map.begin(); pos != bd.m_named_mod_map.end();)
    {
        module& mod = pos->second;
        ++pos;

        m_eh->clear_errors();

        mod.type_desugaring(m_eh);
        sema::desugaring(mod, m_eh);

        if (m_eh->has_errors() == true)
            remove_processed_module(mod, bd);
    };
};

void modules_impl::build_closures(build_data& bd)
{
    for (auto pos = bd.m_named_mod_map.begin(); pos != bd.m_named_mod_map.end();)
    {
        module& mod = pos->second;
        ++pos;

        m_eh->clear_errors();

        sema::build_closures(mod, m_eh);

        if (m_eh->has_errors() == true)
            remove_processed_module(mod, bd);
    };
};

void modules_impl::build_dependency_graph(build_data& bd)
{
    for (auto pos = bd.m_named_mod_map.begin(); pos != bd.m_named_mod_map.end();)
    {
        module& mod = pos->second;
        ++pos;

        m_eh->clear_errors();
        mod.build_type_dependency_graph(m_eh);
        sema::build_dependecy_graph(mod, m_eh);

        if (m_eh->has_errors() == true)
            remove_processed_module(mod, bd);
    };
};

void modules_impl::type_check(build_data& bd)
{
    for (ast::unique_string mod_name : bd.m_build_order)
    {
        auto pos = bd.m_named_mod_map.find(mod_name);
        
        if (pos == bd.m_named_mod_map.end())
            continue;

        module& mod = pos->second;

        m_eh->clear_errors();

        sema::type_check_types(mod, m_eh);
        sema::type_check_terms(mod, m_eh);
    }
};

void modules_impl::eval(std::vector<ast::term>& evaled, build_data& bd)
{
    m_eh->clear_errors();

    for (ast::unique_string mod_name : bd.m_build_order)
    {
        auto pos = bd.m_named_mod_map.find(mod_name);
        
        if (pos == bd.m_named_mod_map.end())
            continue;

        module& mod = pos->second;        

        sema::eval(mod, m_eh, evaled);
    }

    m_eh->clear_errors();
};

void modules_impl::finish_processing(build_data& bd)
{
    for (auto pos = bd.m_named_mod_map.begin(); pos != bd.m_named_mod_map.end();)
    {
        module& mod = pos->second;
        ++pos;

        mod.finish_building();
    };

    if (bd.m_shell_mod.is_null() == false)
        bd.m_shell_mod.finish_building();
};

void modules_impl::make_module(ast::unique_string mod_name, const module_ast& ast, 
                        const string_vec& imports, build_data& bd)
{
    {
        auto pos = bd.m_named_mod_map.find(mod_name);

        if (pos != bd.m_named_mod_map.end())
        {
            module linked_mod  = pos->second;
            linked_mod.add_imports(imports);
            linked_mod.add_ast(ast);
            return;
        };
    };

    auto pos = m_processed_mod_map.find(mod_name);

    if (pos == m_processed_mod_map.end())
    {
        using owner_ptr = std::shared_ptr<modules_impl>;
        owner_ptr owner = this->shared_from_this();

        bool is_shell   = mod_name == get_shell_module_name();
        module mod(mod_name, details::module_index(mod_name), owner, is_shell);
        mod.add_imports(imports);
        mod.add_ast(ast);

        bd.m_named_mod_map[mod_name]  = mod;
        m_processed_mod_map[mod_name]  = mod;

        return;
    }
    else
    {
        module linked_mod  = pos->second;
        linked_mod.add_imports(imports);
        linked_mod.add_ast(ast);

        bd.m_named_mod_map[mod_name]  = linked_mod;
    };
};

module modules_impl::get_module_by_name(ast::unique_string name) const
{
    auto pos = m_processed_mod_map.find(name);

    if (pos != m_processed_mod_map.end())
        return pos->second;
    else
        return module();
};

void modules_impl::remove_processed_module(module& mod, build_data& bd)
{
    ast::unique_string name = mod.get_module_name();

    if (mod.is_shell_module() == true)
    {
        bd.remove_shell_module(mod);
        return;
    };
    
    clear_module(mod);
    bd.m_named_mod_map.erase(name);    
    m_processed_mod_map.erase(name);
};

std::string modules_impl::get_fresh_variable()
{
    std::ostringstream os;
    std::string magic_code  = "kk432ll8";
    os << "shellvar_" << magic_code << get_var_number();
    return os.str();
}

long modules_impl::get_var_number()
{
    return ++m_var_number;
};

void modules_impl::print_type(const std::string& command, bool& need_input)
{
    std::string var = get_fresh_variable();

    m_eh->clear_errors();

    std::ostringstream os;
    os << var << " = " << command;

    need_input = false;
    std::vector<ast::term> evaled;
    process_definition(os.str(), need_input, evaled);

    if (m_eh->has_errors() == true || need_input == true)
        return;

    ast::type ty    = get_console_type(var);

    if (ty)
    {
        std::cout << "\n";

        sema::print_type(std::cout, ty);
        std::cout << "\n";

        std::cout << "\n";
    };
};

void modules_impl::print_kind(const std::string& command, bool& need_input)
{
    std::string var = get_fresh_variable();

    m_eh->clear_errors();

    std::ostringstream os;
    os << "type " << var << " = " << command;

    std::vector<ast::term> evaled;
    need_input = false;
    process_definition(os.str(), need_input, evaled);

    if (need_input == true || m_eh->has_errors() == true)
        return;

    ast::kind ki    = get_console_kind(var);

    if (ki)
    {
        std::cout << "\n";

        sema::print_kind(std::cout, ki);
        std::cout << "\n";

        std::cout << "\n";
    };
};

void modules_impl::process_commands(const std::string& command, 
                        bool& need_input)
{
    m_eh->clear_errors();

    need_input = false;
    std::vector<ast::term> evaled;
    process_definition(command, need_input, evaled);

    if (need_input == true || m_eh->has_errors() == true)
        return;

    module shell            = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return;

    if (evaled.size() == 0)
        return;

    std::cout << "\n";

    for (size_t i = 0; i < evaled.size(); ++i)
    {
        sema::print_term(std::cout, evaled[i], m_eh, shell);
        std::cout << "\n";
    };

    std::cout << "\n";
}

void modules_impl::eval_term(const std::string& command, bool& need_input)
{
    m_eh->clear_errors();

    std::ostringstream os;
    os << "val " << command;

    need_input = false;
    std::vector<ast::term> evaled;
    process_definition(os.str(), need_input, evaled);

    if (need_input == true || m_eh->has_errors() == true)
        return;

    module shell    = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return;

    if (evaled.size() == 0)
        return;

    std::cout << "\n";

    for (size_t i = 0; i < evaled.size(); ++i)
    {
        sema::print_term(std::cout, evaled[i], m_eh, shell);
        std::cout << "\n";
    };

    std::cout << "\n";
};

void modules_impl::print_help(const std::string& command, bool& need_input)
{
    m_eh->clear_errors();

    need_input              = false;

    using loc_vec           = std::vector<ast::location>;
    module shell            = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return;

    bool has_errors;
    ast::unique_string symb = parse_identifier(command, has_errors);

    if (has_errors == true)
        return;

    std::vector<const overload_type_set*> def_type;
    shell.get_all_type_definition(symb, true, def_type);

    std::vector<const overload_set*> def_term;
    shell.get_all_symbol_definition(symb, true, def_term);

    oper::operator_declaration oper;
    shell.get_all_operator_declaration(symb, true, oper);

    if (def_type.size() == 0 && def_term.size() == 0 && oper.is_defined() == false)
    {
        error::scope_error_msg(m_eh).error_undefined_variable(symb, get_shell_location());
        return;
    };

    bool add_nl = false;

    std::cout << "\n";

    if (oper.is_defined() == true)
    {
        print_operator_declarations(oper);
        add_nl = true;
    };

    for (auto pos : def_type)
    {
        if (add_nl)
            std::cout << "\n\n";

        print_type_definition(*pos);
        add_nl = true;
    }

    for (auto pos : def_term)
    {
        if (add_nl)
            std::cout << "\n\n";

        print_term_definition(*pos);
        add_nl = true;
    };

    std::cout << "\n";
};

void modules_impl::print_type_definition(const ad::type_alias_info& def) const
{
    std::cout << "type " << def.get_name().get_name_simple().get_string();
    std::cout << " : ";
    sema::print_kind(std::cout, def.get_kind());
    std::cout << "\n";
    std::cout << "    = ";
    sema::print_type(std::cout, def.get_def());
    std::cout << "\n";
};

void modules_impl::print_term_definition(const overload_set& def) const
{
    long n      = def.size();
    const symbol_definition_env* def_1;

    for (long i = 0; i < n; ++i)
    {
        ad::def_location def_loc;
        def_1   = def.get_definition(i, def_loc);

        if (!def_1)
            continue;

        print_term_definition(*def_1);
    };
};

void modules_impl::print_type_definition(const overload_type_set& def) const
{
    long n      = def.size();
    const type_alias_info* def_1;

    for (long i = 0; i < n; ++i)
    {
        ad::def_location def_loc;
        def_1   = def.get_definition(i, def_loc);

        if (!def_1)
            continue;

        print_type_definition(*def_1);
    };
};

void modules_impl::print_term_definition(const ad::symbol_definition_env& def) const
{
    if (def.has_error() == true)
        return;

    module shell    = get_module_by_name(get_shell_module_name());

    if (shell.is_null() == true)
        return;

    std::cout << def.get_symbol().get_name_simple().get_string();
    std::cout << " : ";
    sema::print_type(std::cout, def.get_signature());
    std::cout << "\n";

    bool has_body;
    const ast::term& ex = def.get_body(has_body);

    if (has_body == true)
    {
        std::cout << "    = ";
        sema::print_term(std::cout, ex, m_eh, shell);
        std::cout << "\n";
    };
};

void modules_impl::print_operator_declarations(const oper::operator_declaration& oper) const
{
    oper.print(std::cout);
};

ast::location modules_impl::get_shell_location() const
{
    return ast::location(m_console_input_num, 1, get_shell_module_name());
};

void modules_impl::process_definition(const std::string& command, 
                        bool& need_more_input, std::vector<ast::term>& evaled)
{
    parse_console_input(command, need_more_input, evaled);    
};

std::string modules_impl::get_module_path(ast::unique_string import, bool& found) const
{
    return m_lib_paths.get_module_path(import.get_string(), found);
}

void modules_impl::make_default_imports(std::vector<ast::unique_string>& imp) const
{
    imp.push_back(ast::unique_string("predefined"));
};

};};

namespace arrow
{

modules::modules(const error_handler_ptr& eh, bool in_shell)
    :m_impl(new impl_type(eh, in_shell))
{};

ast::module_definition modules::parse_module(const std::string& path)
{
    return m_impl->parse_module(path);
};

void modules::add_lib_path(const std::string& dir)
{
    return m_impl->add_lib_path(dir);
};

ast::module_definition modules::parse_module(std::istream& os, const std::string& path_long,
                        const std::string& path_short)
{
    return m_impl->parse_module(os, path_long, path_short);
};

void modules::process_modules(std::vector<ast::term>& evaled)
{
    m_impl->process_modules(evaled);
};

void modules::eval_term(const std::string& command, bool& need_input)
{
    m_impl->eval_term(command, need_input);
}
void modules::print_type(const std::string& command, bool& need_input)
{
    m_impl->print_type(command, need_input);
}
void modules::print_kind(const std::string& command, bool& need_input)
{
    m_impl->print_kind(command, need_input);
}
void modules::print_help(const std::string& command, bool& need_input)
{
    m_impl->print_help(command, need_input);
}

void modules::clear_definitions()
{
    m_impl  = impl_ptr(new impl_type(m_impl->get_error_handler(), 
                                     m_impl->is_shell()));
}
void modules::process_commands(const std::string& command, 
                            bool& need_more_input)
{
    m_impl->process_commands(command, need_more_input);
}

};
