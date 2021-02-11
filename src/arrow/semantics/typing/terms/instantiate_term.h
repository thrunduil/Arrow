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
#pragma once

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/module.h"
#include "semantics/typing/types/kind_checker.h"
#include "semantics/typing/types/unifier_kinds.h"
#include "type_equaler.h"
#include "term_check_helpers.h"

#include <list>
#include <vector>

namespace arrow { namespace typing
{

class delayed_inst_base
{
    public:
        using var_set           = std::set<ad::var_info>;

    public:
        virtual ~delayed_inst_base(){};

        virtual void            set_process(bool proc) = 0;
        virtual bool            process() const = 0;
        virtual bool            is_kind(bool& is_term) const = 0;
        virtual const var_set&  get_instantiated() const = 0;
};

template<class Ty>  struct is_kind_type;
template<>          struct is_kind_type<ast::type>  { static const bool value = false; };
template<>          struct is_kind_type<ast::kind>  { static const bool value = true; };

template<class Ty>
struct delayed_term_inst : public delayed_inst_base
{   
    bool                    m_process;
    ast::identifier         m_new_id;
    ast::identifier_term    m_base_id;
    std::vector<Ty>         m_args;
    var_set                 m_instantiated;

    delayed_term_inst(const ast::identifier& new_id, const ast::identifier_term& base_id, 
        const std::vector<Ty>& args, const var_set& instantiated)
        :m_new_id(new_id), m_base_id(base_id), m_args(args), m_process(false)
        ,m_instantiated(instantiated)
    {};

    virtual void            set_process(bool proc) override     { m_process = proc; };
    virtual bool            process() const override            { return m_process; };
    virtual bool            is_kind(bool& is_term) const override;
    virtual const var_set&  get_instantiated() const override   { return m_instantiated; }
    bool                    has_instantiation(const ad::var_info& id) const;
};

template<class Ty>
struct delayed_type_inst : public delayed_inst_base
{   
    bool                    m_process;
    ast::identifier         m_new_id;
    ast::identifier_type    m_base_id;
    std::vector<Ty>         m_args;
    var_set                 m_instantiated;

    delayed_type_inst(const ast::identifier& new_id, const ast::identifier_type& base_id, 
        const std::vector<Ty>& args, const var_set& instantiated)
        :m_new_id(new_id), m_base_id(base_id), m_args(args), m_process(false)
        ,m_instantiated(instantiated)
    {};

    virtual void            set_process(bool proc) override     { m_process = proc; };
    virtual bool            process() const override            { return m_process; };
    virtual bool            is_kind(bool& is_term) const override;
    virtual const var_set&  get_instantiated() const override   { return m_instantiated; }
    bool                    has_instantiation(const ad::var_info& id) const;
};

template<class Ty>
bool delayed_type_inst<Ty>::has_instantiation(const ad::var_info& id) const
{
    auto pos = m_instantiated.find(id);
    if (pos == m_instantiated.end())
        return false;
    else
        return true;
};

template<class Ty>
bool delayed_type_inst<Ty>::is_kind(bool& is_term) const
{
    is_term = false;
    return is_kind_type<Ty>::value;
};

template<class Ty>
bool delayed_term_inst<Ty>::has_instantiation(const ad::var_info& id) const
{
    auto pos = m_instantiated.find(id);
    if (pos == m_instantiated.end())
        return false;
    else
        return true;
};
template<class Ty>
bool delayed_term_inst<Ty>::is_kind(bool& is_term) const
{
    is_term = true;
    return is_kind_type<Ty>::value;
};


class inst_collector
{
    private:
        using context_type  = details::kind_inference_context;
        using subs_ptr      = std::shared_ptr<infered_substitutions>;
        using queue_item    = std::shared_ptr<delayed_inst_base>;
        using queue         = std::list<queue_item>;
        using vars_set      = std::set<ad::var_info>;
        using close_type_fun= std::function<ast::type (const ast::type&)>;
        using close_kind_fun= std::function<ast::kind (const ast::kind&)>;

    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        queue               m_queue;
        vars_set            m_instantiated;
        kind_checker        m_kind_checker;
        type_equaler        m_type_eq;
        unifier_kinds       m_kind_eq;
        bool                m_instanitiate_local;

    public:
        inst_collector(const error_handler_ptr& eh, const module& mod);

        /// remover all delayed instantiations due to errors
        void            clear_instantiations();

        /// instantiate functions; must be called after type checking
        /// of a recursive group
        void            instantiate();

        /// process type applications; create instantiation stubs
        /// local functions can be instantiated only when type context
        /// is empty
        ast::term       make_term_type_application(const ast::term& ex,
                            bool can_instantiate_local);
        ast::term       make_term_kind_application(const ast::term& ex,
                            bool can_instantiate_local);

        ast::type       make_type_type_application(const ast::type& ex,
                            bool can_instantiate_local);
        ast::type       make_type_kind_application(const ast::type& ex,
                            bool can_instantiate_local);

        ast::term       make_term_type_subs(const ast::identifier& id, const ast::type& subs, 
                            const ast::term& body, bool can_instantiate_local);

        ast::type       instantiate_and_infer_kind(const ast::type& t, 
                            const std::vector<details::applic_arg_term>& args, bool show_errors,
                            kind_subs_map& subs_map, bool& has_errors, bool can_instantiate_local);
        ast::type       instantiate_and_infer_kind(const ast::type& t, 
                            const std::vector<ast::kind>& args, bool show_errors,
                            kind_subs_map& subs_map, bool& has_errors, bool can_instantiate_local);
        ast::kind       instantiate_and_infer_kind(const ast::kind& t, 
                            const std::vector<details::applic_arg_type>& args, bool show_errors,
                            kind_subs_map& subs_map, bool& has_errors);

        void            close_arg_types(const close_type_fun& fun);
        void            close_arg_kinds(const close_kind_fun& fun);

    public:
        static bool     is_closed(const std::vector<details::applic_arg_term>& args);
        ast::term       resolve_overload(const ast::identifier_term& id, 
                            const std::vector<details::applic_arg_term>& args, 
                            const ast::type& ret_ty, subs_ptr& subs, 
                        const ast::overload_index* oi, bool& success);

    private:        
        bool            can_instantiate_term_symbol(const ast::identifier_term& id, const module& mod,
                            bool& is_local);
        bool            can_instantiate_type_symbol(const ast::identifier_type& id, const module& mod);
        bool            can_instantiate_local() const;
        bool            is_function_closure(const ast::term& ex) const;
        ast::term       add_closure(const ast::term& def, const ast::term& def_with_closure);

        void            collect_type_args(const ast::term& ex, std::vector<ast::type>& args,
                            ast::term& head);
        void            collect_type_args(const ast::type& ex, std::vector<ast::type>& args,
                            ast::type& head);
        void            collect_kind_args(const ast::term& ex, std::vector<ast::kind>& args,
                            ast::term& head);
        void            collect_kind_args(const ast::type& ex, std::vector<ast::kind>& args,
                            ast::type& head);

        ast::term       make_term_type(const ast::term& root, const ast::term& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args);
        ast::term       make_term_kind(const ast::term& root, const ast::term& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args);
        ast::type       make_type_type(const ast::type& root, const ast::type& ty, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args);

    public:
        ast::type       make_type_kind(const ast::type& root, const ast::type& ty, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args);

    private:
        ast::kind       make_kind_type(const ast::kind& root, const ast::kind& ty, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args);
        ast::kind       make_kind_kind(const ast::kind& root, const ast::kind& ty, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args);

        ast::term       instantiate_identifier_term_type(const ast::identifier_term& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args, bool& success, 
                            bool& has_errors);
        ast::type       instantiate_identifier_type_type(const ast::identifier_type& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args, bool& success, 
                            bool& has_errors);
        ast::term       instantiate_closure_type(const ast::closure_term& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args, bool& success, 
                            bool& has_errors);
        ast::term       instantiate_identifier_term_kind(const ast::identifier_term& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args, bool& success, 
                            bool& has_errors);
        ast::type       instantiate_identifier_type_kind(const ast::identifier_type& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args, bool& success, 
                            bool& has_errors);
        ast::term       instantiate_closure_kind(const ast::closure_term& id, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args, bool& success, 
                            bool& has_errors);

        ast::term       instantiate_general_term_type(const ast::term& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args, const ast::term& root);
        ast::type       instantiate_general_type_type(const ast::type& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::type>& args, const ast::type& root);
        ast::term       instantiate_general_term_kind(const ast::term& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args, const ast::term& root);
        ast::type       instantiate_general_type_kind(const ast::type& ex, size_t start_pos, 
                            size_t end_pos, const std::vector<ast::kind>& args, const ast::type& root);

        ast::identifier_term 
                        register_term_type_instantiation(const ast::identifier_term& base_id, 
                            const std::vector<ast::type>& args);
        ast::identifier_type 
                        register_type_type_instantiation(const ast::identifier_type& base_id, 
                            const std::vector<ast::type>& args);

        ast::identifier_term 
                        register_term_kind_instantiation(const ast::identifier_term& base_id, 
                            const std::vector<ast::kind>& args);
        ast::identifier_type 
                        register_type_kind_instantiation(const ast::identifier_type& base_id, 
                            const std::vector<ast::kind>& args);
        void            process_delayed_term_type_inst(delayed_term_inst<ast::type>& item, bool must_inst);
        void            process_delayed_term_kind_inst(delayed_term_inst<ast::kind>& item, bool must_inst);
        void            process_delayed_type_type_inst(delayed_type_inst<ast::type>& item, bool must_inst);
        void            process_delayed_type_kind_inst(delayed_type_inst<ast::kind>& item, bool must_inst);
};

};};