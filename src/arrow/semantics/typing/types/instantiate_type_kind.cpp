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
#include "instantiate_type_kind.h"
#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "semantics/semantics.h"
#include "semantics/typing/terms/instantiate_term.h"

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//                  kind_subs_map
//------------------------------------------------------------------
kind_subs_map::kind_subs_map()
{};

void kind_subs_map::add_substitution(const ast::identifier& id, const ast::kind& k)
{
    m_subs_map[id] = k;
};

void kind_subs_map::add_instantiation(const ast::identifier& id, const ast::kind_var& k)
{
    m_subs_map[id] = k;
    m_inst_vec.push_back(id_kind(id,k));
};

ast::kind kind_subs_map::get_substitution(const ast::identifier& id, bool& has) const
{
    auto pos = m_subs_map.find(id);
    if (pos == m_subs_map.end())
    {
        has = false;
        return ast::kind();
    }
    else
    {
        has = true;
        return pos->second;
    };
};

long kind_subs_map::number_substitutions() const
{
    return (long)m_subs_map.size() - number_instantiations();
};
long kind_subs_map::number_instantiations() const
{
    return (long)m_inst_vec.size();
};

ast::term kind_subs_map::make_kind_application(const ast::term& ex) const
{
    ast::term ret = ex;
    for (size_t i = 0; i < m_inst_vec.size(); ++i)
    {
        const ast::kind& ki = m_inst_vec[i].second;
        ret = ast::term_kind_application(ki->get_location(), ret, ki);
    }

    return ret;
};

ast::type kind_subs_map::make_kind_application(const ast::type& ex) const
{
    ast::type ret = ex;
    for (size_t i = 0; i < m_inst_vec.size(); ++i)
    {
        const ast::kind& ki = m_inst_vec[i].second;
        ret = ast::type_kind_application(ki->get_location(), ret, ki);
    }

    return ret;
};

bool kind_subs_map::check_all_infered(const ast::location& loc, const error_handler_ptr& eh, 
                                    bool show_errors) const
{
    if (m_inst_vec.size() == 0)
        return true;

    bool succ   = true;

    for (const auto& item : m_inst_vec)
    {
        const ast::identifier& id   = item.first;
        const ast::kind& ki         = item.second;

        if (sema::has_kind_vars(ki) == true)
        {
            succ = false;

            if (show_errors == true)
            {
                ast::kind k0        = sema::expand_kind_vars(ki);
                error::type_error_msg(eh).error_unable_to_deduce_kind_application
                                            (loc, id, k0);
            };
        };
    };

    return succ;
};

//------------------------------------------------------------------
//                  instantiate_kind
//------------------------------------------------------------------
class apply_kind_args_vis : public ast::rewrite_visitor<apply_kind_args_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<apply_kind_args_vis>;
        using kind_vec      = std::vector<ast::kind>;
        using subs_map      = std::map<ad::var_info, ast::kind>;

    private:
        kind_subs_map*      m_map;
        long                n_rem_args;
        long                m_pos;
        error_handler_ptr   m_eh;
        bool                m_show_errors;

    public:
        apply_kind_args_vis(const error_handler_ptr& eh, kind_subs_map& subs,
                            bool show_errors)
            :m_eh(eh), m_map(&subs), m_show_errors(show_errors)
        {};

        ast::kind   make(const ast::kind& t, const kind_vec& args, bool& has_errors);
        ast::type   make(const ast::type& t, const kind_vec& args, bool& has_errors,
                        inst_collector* col);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        //---------------------------------------------------------------
        //                  KINDS
        //---------------------------------------------------------------
        ast::kind eval(const ast::kind_identifier& kv)
        {
            bool has;
            ast::kind ret   = m_map->get_substitution(kv.get_identifier(), has);

            if (has)
                return ret;
            else
                return kv;
        }

    private:
        void    make_subs_map(const ast::type& t, const kind_vec& args, ast::type& body,
                    bool add_kind_vars);
        void    make_subs_map(const ast::kind& k, const kind_vec& args, ast::kind& body);
};

ast::type apply_kind_args_vis::make(const ast::type& t, const kind_vec& args, bool& has_errors, 
                                    inst_collector* col)
{
    has_errors      = false;

    if (t->get_type_code() != ast::details::type_code::type_kind_scope)
    {
        if (t->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv    = t.static_cast_to<ast::type_var>();
            if (tv.is_set() == true)
                return make(tv.get(), args, has_errors, col);
        };

        if (args.size() > 0)
        {
            has_errors  = true;
            if (m_show_errors == true)
            {
                error::type_error_msg(m_eh).error_kind_abstraction_required
                                        (t->get_location(), t);
            };
            return ast::error_type(t->get_location());
        }

        return t;
    };

    //if kinds args are closed, then instantiate
    size_t end_pos  = args.size();
    size_t last_pos = 0;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool free_vars;
        bool cl = sema::is_closed_kind(args[last_pos], free_vars);

        if (cl == false || free_vars == true)
            break;
    };

    n_rem_args  = (long)args.size();
    m_pos       = 0;

    ast::type body;
    ast::type t2    = t;

    if (end_pos > 0 && last_pos == end_pos)
    {
        make_subs_map(t2, args, body, false);
        t2          = col->make_type_kind(ast::type(), t2, 0, end_pos, args);        

        make_subs_map(t2, kind_vec(), body, true);
    }
    else
    {
        make_subs_map(t2, args, body, true);
    };

    if (n_rem_args > 0)
    {
        if (m_show_errors == true)
        {
            long arg_size       = (long)args.size();
            long max_allowed    = arg_size - n_rem_args;

            error::type_error_msg(m_eh).error_invalid_kind_application_too_many_args
                    (t->get_location(), t, max_allowed, arg_size);
        };

        has_errors  = true;
        return ast::error_type(t->get_location());
    };

    has_errors      = false;
    return visit(body);
};

ast::kind apply_kind_args_vis::make(const ast::kind& k, const kind_vec& args, bool& has_errors)
{
    has_errors      = false;

    if (k->get_kind_code() != ast::details::kind_code::kind_scope)
    {
        if (k->get_kind_code() == ast::details::kind_code::kind_var)
        {
            ast::kind_var kv    = k.static_cast_to<ast::kind_var>();
            if (kv.is_set() == true)
                return make(kv.get(), args, has_errors);
        };

        if (args.size() > 0)
        {
            has_errors  = true;
            if (m_show_errors == true)
            {
                error::type_error_msg(m_eh).error_kind_abstraction_required
                                        (k->get_location(), k);
            };

            return k;
        }

        return k;
    };

    n_rem_args  = (long)args.size();
    m_pos       = 0;

    ast::kind body;

    // we do not need fresh binders for kinds
    make_subs_map(k, args, body);

    if (n_rem_args > 0)
    {
        if (m_show_errors == true)
        {
            long arg_size       = (long)args.size();
            long max_allowed    = arg_size - n_rem_args;

            error::type_error_msg(m_eh).error_invalid_kind_application_too_many_args
                    (k->get_location(), k, max_allowed, arg_size);
        };

        has_errors  = true;
        return k;
    };

    has_errors      = false;
    return visit(body);
};

void apply_kind_args_vis::make_subs_map(const ast::type& t, const kind_vec& args, ast::type& body,
                                    bool add_kind_vars)
{
    if (t->get_type_code() == ast::details::type_code::type_kind_scope)
    {
        ast::type_kind_scope ts     = t.static_cast_to<ast::type_kind_scope>();
        const ast::identifier& var  = ts.get_var();
        ast::type ts_body           = ts.get_body();

        --n_rem_args;

        if (n_rem_args >= 0)
        {
            m_map->add_substitution(var, args[m_pos]);
            ++m_pos;

            if (n_rem_args > 0 || add_kind_vars == true)
                return make_subs_map(ts_body, args, body, add_kind_vars);
        }
        else
        {
            ast::kind_var kv    = ast::kind_var(t->get_location(), false);
            m_map->add_instantiation(var, kv);
            return make_subs_map(ts_body, args, body, true);
        };
    }
    else if (t->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv = t.static_cast_to<ast::type_var>();

        if (tv.is_set() == true)
            return make_subs_map(tv.get(), args, body, add_kind_vars);
        
        body = tv;
        return;
    }

    body = t;
    return;
};

void apply_kind_args_vis::make_subs_map(const ast::kind& k, const kind_vec& args, ast::kind& body)
{
    if (k->get_kind_code() == ast::details::kind_code::kind_scope)
    {
        ast::kind_scope ks          = k.static_cast_to<ast::kind_scope>();
        const ast::identifier& var  = ks.get_var();
        ast::kind ks_body           = ks.get_body();

        --n_rem_args;

        if (n_rem_args >= 0)
        {
            m_map->add_substitution(var, args[m_pos]);
            ++m_pos;
        }
        else
        {
            ast::kind_var kv    = ast::kind_var(k->get_location(), false);
            m_map->add_instantiation(var, kv);
        };

        return make_subs_map(ks_body, args, body);
    }
    else if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        ast::kind_var kv = k.static_cast_to<ast::kind_var>();

        if (kv.is_set() == true)
            return make_subs_map(kv.get(), args, body);
        
        body = kv;
        return;
    }

    body = k;
    return;
};

ast::type instantiate_kind::make(const ast::type& ty, const std::vector<ast::kind>& args,
                        kind_subs_map& subs, bool& has_errors, inst_collector* col)
{
    apply_kind_args_vis vis(m_eh, subs, m_show_errors);
    ast::type ret = vis.make(ty, args, has_errors, col);
    return ret;
};

ast::kind instantiate_kind::make(const ast::kind& k, const std::vector<ast::kind>& args,
                        kind_subs_map& subs, bool& has_errors)
{
    apply_kind_args_vis vis(m_eh, subs, m_show_errors);
    ast::kind ret = vis.make(k, args, has_errors);
    return ret;
};

};};