/* 
 *  This file is a part of Hasky Lib.
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
#include "scope_builder.h"
#include "scope_builder_terms.h"
#include "scope_builder_types.h"

namespace arrow { namespace sema { namespace details
{

template<>
struct process_sequence_config<ast::term_sequence>
{
    using item_type     = ast::term_sequence_item::item_type;
    using basic_type    = ast::term;
    
    static bool is_identifier_or_id_tuple(const ast::term& ex)
    {
        if (ex->get_term_code() == ast::details::term_code::identifier_term)
            return true;

        if (ex->get_term_code() == ast::details::term_code::tuple_term)
        {
            ast::tuple_term tt  = ex.static_cast_to<ast::tuple_term>();
            
            if (tt.size() != 1)
                return false;

            return is_identifier_or_id_tuple(tt.get(0));
        };

        if (ex->get_term_code() == ast::details::term_code::term_sequence)
        {
            ast::term_sequence ts   = ex.static_cast_to<ast::term_sequence>();
            if (ts.size() != 1)
                return false;

            const ast::term_sequence_item& item = ts.get(0);

            if (item.get_item_type() != ast::term_sequence_item::item_type::basic_item)
                return false;

            return is_identifier_or_id_tuple(item.get_basic());
        };

        return false;
    };

    static ast::identifier get_identifier_or_id_tuple(const ast::term& ex)
    {
        if (ex->get_term_code() == ast::details::term_code::identifier_term)
            return ex.static_cast_to<ast::identifier_term>().get_identifier();

        if (ex->get_term_code() == ast::details::term_code::tuple_term)
        {
            ast::tuple_term tt  = ex.static_cast_to<ast::tuple_term>();
            return get_identifier_or_id_tuple(tt.get(0));
        };

        //this is term sequence
        ast::term_sequence ts   = ex.static_cast_to<ast::term_sequence>();
        const ast::term_sequence_item& item = ts.get(0);
        return get_identifier_or_id_tuple(item.get_basic());
    };
};

template<>
struct process_sequence_config<ast::type_sequence>
{
    using item_type     = ast::type_sequence_item::item_type;
    using basic_type    = ast::type;

    static bool is_identifier_or_id_tuple(const ast::type& ex)
    {
        if (ex->get_type_code() == ast::details::type_code::identifier_type)
            return true;

        if (ex->get_type_code() == ast::details::type_code::tuple_type)
        {
            ast::tuple_type tt  = ex.static_cast_to<ast::tuple_type>();
            
            if (tt.size() != 1)
                return false;

            return is_identifier_or_id_tuple(tt.get(0));
        };

        if (ex->get_type_code() == ast::details::type_code::type_sequence)
        {
            ast::type_sequence ts   = ex.static_cast_to<ast::type_sequence>();
            if (ts.size() != 1)
                return false;

            const ast::type_sequence_item& item = ts.get(0);

            if (item.get_item_type() != ast::type_sequence_item::item_type::basic_item)
                return false;

            return is_identifier_or_id_tuple(item.get_basic());
        };

        return false;
    };

    static ast::identifier get_identifier_or_id_tuple(const ast::type& ex)
    {
        if (ex->get_type_code() == ast::details::type_code::identifier_type)
            return ex.static_cast_to<ast::identifier_type>().get_identifier();

        if (ex->get_type_code() == ast::details::type_code::tuple_type)
        {
            ast::tuple_type tt  = ex.static_cast_to<ast::tuple_type>();
            return get_identifier_or_id_tuple(tt.get(0));
        };

        //this is term sequence
        ast::type_sequence ts   = ex.static_cast_to<ast::type_sequence>();
        const ast::type_sequence_item& item = ts.get(0);
        return get_identifier_or_id_tuple(item.get_basic());
    };
};

//------------------------------------------------------------------
//                   process_sequence   
//------------------------------------------------------------------
template<class Seq, class Owner>
void process_sequence<Seq,Owner>::make(const Seq& ast)
{
    using item_type     = typename config::item_type;    

    long s              = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const basic_type& ex    = e.get_basic();
                process_term(ex, i);
                break;
            };
            case item_type::proj_item:
            {
                process_proj();
                break;
            };
            case item_type::dot_item:
            {
                const ast::identifier& id   = e.get_dot();
                process_dot(id, i);
                break;
            };

            case item_type::type_applic_item:
            {
                const type_vec& args    = e.get_type_applic_args();
                process_type_applic(args);
                break;
            }

            case item_type::kind_applic_item:
            {
                const kind_vec& args    = e.get_kind_applic_args();
                process_kind_applic(args);
                break;
            }
        };
    };

    finalize();
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::process_type_applic(const type_vec& args)
{
    if (m_in_module_path == true)
    {
        error::scope_error_msg(m_eh).error_module_in_expression
                    (m_last_mod->get_location(), m_last_mod);
        return;
    };

    m_in_module_path = false;

    for (const applic_type& elem: args)
        m_owner->visit(elem);
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::process_kind_applic(const kind_vec& args)
{
    if (m_in_module_path == true)
    {
        error::scope_error_msg(m_eh).error_module_in_expression
                    (m_last_mod->get_location(), m_last_mod);
        return;
    };

    m_in_module_path = false;

    for (const applic_kind_type& elem: args)
        m_owner->visit(elem);
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::process_term(const basic_type& ex, long pos)
{
    if (m_in_module_path == true)
    {
        if (config::is_identifier_or_id_tuple(ex) == true)
        {
            ast::identifier id  = config::get_identifier_or_id_tuple(ex);

            //[...].id [...]
            bool is_mod_name    = m_root_mod.is_module_name(id, m_self_allowed);

            if (is_mod_name == true)
            {
                m_root_mod      = m_root_mod.get_module(id->get_location(), id, 
                                        m_self_allowed, m_eh);
                m_last_mod      = id;
                m_self_allowed  = false;
                id.get_symbol_data()->get_kind().set_sector_module();
                return;
            };

            if (pos > 0)
            {
                m_in_module_path    = false;
                build_qualified_identifier(id);
                return;
            };
        }
        else if (pos > 0)
        {
            error::scope_error_msg(m_eh).error_module_in_expression
                        (m_last_mod->get_location(), m_last_mod);

            m_in_module_path    = false;
            return;
        };
    };

    m_in_module_path    = false;
    m_owner->visit(ex);
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::process_dot(const ast::identifier& id, long pos)
{
    if (pos == 0)
    {
        //.id [...] 
        m_root_mod          = m_current_mod.get_unnamed_module(id->get_location(), m_eh);
        bool is_mod_name    = m_root_mod.is_module_name(id, m_self_allowed);

        if (is_mod_name == true)
        {
            m_root_mod      = m_root_mod.get_module(id->get_location(), id, 
                                m_self_allowed, m_eh);
            m_self_allowed  = false;
            m_last_mod      = id;

            id.get_symbol_data()->get_kind().set_sector_module();
            return;
        };

        m_in_module_path    = false;
        build_qualified_identifier(id);
        return;
    }
    else if (m_in_module_path == true)
    {
        //[path].id [...]
        bool is_mod_name    = m_root_mod.is_module_name(id, m_self_allowed);

        if (is_mod_name == true)
        {
            m_root_mod      = m_root_mod.get_module(id->get_location(), id, 
                                m_self_allowed, m_eh);
            m_self_allowed  = false;
            m_last_mod      = id;

            id.get_symbol_data()->get_kind().set_sector_module();
            return;
        };

        m_in_module_path    = false;
        build_qualified_identifier(id);
        return;
    }
    else
    {
        //[term].id [...]
        build_identifier(id);
        return;
    };
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::build_identifier(const ast::identifier& id)
{
    m_owner->build_identifier(id);
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::process_proj()
{
    if (m_in_module_path == true)
    {
        error::scope_error_msg(m_eh).error_module_in_expression
                    (m_last_mod->get_location(), m_last_mod);
        return;
    };

    m_in_module_path = false;
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::build_qualified_identifier(const ast::identifier& id)
{
    m_owner->build_qualified_identifier(id, m_root_mod);
};

template<class Seq, class Owner>
void process_sequence<Seq,Owner>::finalize()
{
    if (m_in_module_path == false)
        return;

    error::scope_error_msg(m_eh).error_module_in_expression
                    (m_last_mod->get_location(), m_last_mod);
};

template class process_sequence<ast::term_sequence, build_scope_visitor>;
template class process_sequence<ast::type_sequence, build_type_scope_visitor>;

};};};
