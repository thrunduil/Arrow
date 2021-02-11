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

#include "semantics.h"
#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "semantics/utils.h"

namespace arrow { namespace sema
{

class check_unique_id
{
    private:
        using key_type  = ast::unique_string;
        using id_map    = std::map<key_type, ast::location>;

    private:
        id_map              m_map;
        bool                m_allow_underscore;
        error_handler_ptr   m_eh;

    public:
        check_unique_id(const error_handler_ptr& eh, bool allow_underscore)
            :m_allow_underscore(allow_underscore), m_eh(eh)
        {};

        void add(const ast::identifier& id)
        {
            if (id.is_underscore() == true)
            {
                if (m_allow_underscore == true)
                    return;

                error::scope_error_msg(m_eh).error_underscore_not_allowed(id->get_location());
                return;
            };

            auto pos = m_map.find(id.get_name_simple());

            if (pos != m_map.end())
            {
                error::scope_error_msg(m_eh).error_symbol_defined
                    (id.get_name_simple(), id->get_location(), pos->second);
                return;
            };

            m_map.insert(pos, id_map::value_type(id.get_name_simple(), id->get_location()));
            return;
        };

        void check_underscore(const ast::identifier& id)
        {
            if (id.is_underscore() == true)
            {
                if (m_allow_underscore == true)
                    return;

                error::scope_error_msg(m_eh).error_underscore_not_allowed(id->get_location());
                return;
            };
        };
};

template<class Ast_type>
struct get_code
{};

template<>
struct get_code<ast::kind>
{
    static ast::details::kind_code eval(const ast::kind& k)
    {
        return k->get_kind_code();
    }
};

template<>
struct get_code<ast::type>
{
    static ast::details::type_code eval(const ast::type& k)
    {
        return k->get_type_code();
    }
};

template<>
struct get_code<ast::term>
{
    static ast::details::term_code eval(const ast::term& k)
    {
        return k->get_term_code();
    }
};

class constraints_visitor : public ast::traversal_visitor<constraints_visitor>
{    
    private:
        using base_type = ast::traversal_visitor<constraints_visitor>;

    private:
        error_handler_ptr   m_eh;
        bool                m_allow_type_var;

    public:
        constraints_visitor(const error_handler_ptr& eh)
            :m_eh(eh), m_allow_type_var(false)
        {};

    public:
        //forward all other functions to the base class
        template<class Ast, class ... Args>
        void eval(const Ast& ast, Args&& ... args)
        {
            return base_type::eval(ast, std::forward<Args>(args)...);
        };

        //---------------------------------------------------------------
        //                      OTHER
        //---------------------------------------------------------------        
        void eval(const ast::identifier& id)
        {
            if (id.is_underscore() == true)
                error::scope_error_msg(m_eh).error_underscore_not_allowed(id->get_location());
        }

        //use default traversal
        //void ast::eval(const ast::module_definition& seq);
        //void ast::eval(const ast::annotated_type_identifier& seq);
        //void ast::eval(const ast::annotated_identifier& seq);

        //---------------------------------------------------------------
        //               DEFINITIONS
        //---------------------------------------------------------------
        /// definition variable names must be unique
        void eval(const ast::definitions& seq)
        {
            long s = seq.size();

            check_unique_id check_names_terms(m_eh, false);
            check_unique_id check_names_types(m_eh, false);

            for (long i = 0; i < s; ++i)
            {
                const ast::definition& def  = seq.get_definition(i);
                visit(def, check_names_terms, check_names_types);
            }
        };

        void eval(const ast::term_use& ast, check_unique_id& check_term, 
                  check_unique_id& check_type)
        {
            (void)check_term;
            (void)check_type;

            const ast::term& ex = ast.get_term();
            eval_toplevel_term(ex);
        };

        void eval(const ast::fixity_definition&, check_unique_id&, check_unique_id&)
        {};

        //underscore not allowed; type_var cannot be set
        void eval(const ast::term_definition& ast, check_unique_id& check_term, 
                  check_unique_id& check_type)
        {
            const ast::identifier& id   = ast.get_identifier();
            const ast::type& ty         = ast.get_type();
            const ast::term& ex         = ast.get_body();

            (void)check_type;
            check_term.check_underscore(id);
         
            if (ex)
                eval_toplevel_term(ex);

            if (ty)
                eval_toplevel_type(ty);
        }

        //underscore not allowed; body cannot be a type_var
        void eval(const ast::type_definition& ast, check_unique_id& check_term, 
                  check_unique_id& check_type)
        {
            const ast::identifier& id   = ast.get_identifier();
            const ast::kind& ki         = ast.get_kind();
            const ast::type& ty         = ast.get_body();

            (void)check_term;
            check_type.add(id);            

            eval_toplevel_kind(ki);
            eval_toplevel_type(ty);
        };

        //underscore not allowed; body cannot be a type_var
        //type variables must be different
        void eval(const ast::type_definition_var& ast, check_unique_id& check_term, 
                  check_unique_id& check_type)
        {
            const ast::identifier& id   = ast.get_identifier();
            const ast::type& ty         = ast.get_body();
            const ast::kind& k          = ast.get_return_kind();
            long n                      = ast.var_size();

            (void)check_term;
            check_type.add(id);
            
            check_unique_id check_names_loc(m_eh, false);

            for (long i = 0; i < n; ++i)
            {
                const ast::annotated_type_identifier& aid   = ast.get_arg(i);
                const ast::identifier& id_loc   = aid.get_identifier();
                const ast::kind& ki             = aid.get_kind();

                check_names_loc.add(id_loc);
                visit(ki);
            };

            eval_toplevel_type(ty);

            if (k)
                eval_toplevel_kind(k);
        };

        //---------------------------------------------------------------
        //                      TERMS
        //---------------------------------------------------------------
        void eval_toplevel_term(const ast::term& ex)
        {
            if (ex->get_term_code() == ast::details::term_code::term_kind_scope)
            {
                ast::term_kind_scope ks = ex.static_cast_to<ast::term_kind_scope>();
                process_kind_scope(ks);

                return;
            }

            if (ex->get_term_code() == ast::details::term_code::term_constant)
                return;
            else
                return visit(ex);
        };

        //all names must be different; underscores not allowed
        void eval(const ast::mu_term& ast)
        {
            using annot_id  = ast::annotated_identifier;

            check_unique_id check_names(m_eh, false);

            ast::mu_term lam    = ast;
            for (;;)
            {
                const ast::term& body       = lam.get_body();
                const annot_id& var         = lam.get_var();
                const ast::identifier& id   = var.get_identifier();
                const ast::type& ty         = var.get_type();

                check_names.add(id);

                visit(ty);

                if (body->get_term_code() == ast::details::term_code::mu_term)
                {
                    lam = body.static_cast_to<ast::mu_term>();
                    continue;
                }
                else
                {
                    visit(body);
                    break;
                }
            };
        };

        //all names must be different; underscores allowed
        void eval(const ast::lambda_term& ast)
        {
            using annot_id  = ast::annotated_identifier;

            check_unique_id check_names(m_eh, true);

            ast::lambda_term lam    = ast;
            for (;;)
            {
                const ast::term& body       = lam.get_body();
                const annot_id& var         = lam.get_var();
                const ast::identifier& id   = var.get_identifier();
                const ast::type& ty         = var.get_type();

                check_names.add(id);

                visit(ty);

                if (body->get_term_code() == ast::details::term_code::lambda_term)
                {
                    lam = body.static_cast_to<ast::lambda_term>();
                    continue;
                }
                else
                {
                    visit(body);
                    break;
                }
            };
        };

        //all names must be different; underscores not allowed
        void eval(const ast::all_term& ast)
        {
            using annot_id  = ast::annotated_type_identifier;

            check_unique_id check_names(m_eh, false);

            ast::all_term lam               = ast;

            for (;;)
            {
                const ast::term& body       = lam.get_body();
                const annot_id& var         = lam.get_var();
                const ast::identifier& id   = var.get_identifier();
                const ast::kind& ki         = var.get_kind();

                check_names.add(id);

                visit(ki);

                if (body->get_term_code() == ast::details::term_code::all_term)
                {
                    lam = body.static_cast_to<ast::all_term>();
                    continue;
                }
                else
                {
                    visit(body);
                    break;
                }
            };
        };

        //labels must be different; underscore not allowed;
        //type annotation of field is not required
        void eval(const ast::record_term& ast)
        {
            long k          = ast.size();
            using annot_id  = ast::annotated_identifier;

            check_unique_id check_names(m_eh, false);

            for (long i = 0; i < k; ++i)
            {
                const auto& aid             = ast.get_label(i);
                const ast::identifier& id   = aid.get_identifier();
                const ast::type& ty         = aid.get_type();
                const ast::term& ex         = ast.get_term(i);

                check_names.add(id);

                eval_type(ty);
                visit(ex);
            };
        };

        void eval(const ast::term_constant& ast)
        {
            error::scope_error_msg(m_eh).error_term_constant_outside_term_definition
                                (ast->get_location());
        };

        //labels must be different; underscores not allowed;
        //type annotations for case symbols are not required
        void eval(const ast::variant_case_term& ast)
        {
            const ast::term& ex         = ast.get_term();
            long n                      = ast.size();

            visit(ex);

            check_unique_id check_names(m_eh, false);

            for (long i = 0; i < n; ++i)
            {
                const auto& aid1            = ast.get_label(i);
                const ast::identifier& id1  = aid1.get_identifier();
                const ast::type& t1         = aid1.get_type();
                const ast::term& b1         = ast.get_bind(i);
            
                check_names.add(id1);
                eval_type(t1);

                visit(b1);
            };        
        };

        //type annotations are not required; underscores not allowed
        void eval(const ast::variant_term& ast)
        {
            using annot_id              = ast::annotated_identifier;
            const ast::term& ex         = ast.get_term();
            const ast::type& ty         = ast.get_type();
            const annot_id& aid         = ast.get_label();

            visit(ex);
            visit(ty);

            check_unique_id check_names(m_eh, false);
            check_names.add(aid.get_identifier());
            eval_type(aid.get_type());
        };

        void eval(const ast::error_term& ast)
        {
            (void)ast;
            m_eh->add_error();
        };

        //underscores not allowed; type annotations not required
        void eval(const ast::unpack_term& ast)
        {
            const ast::term& pack   = ast.get_package();
            const ast::term& body   = ast.get_term();

            const auto& atid        = ast.get_type_identifier();
            const auto &aid         = ast.get_term_identifier();

            visit(pack);
            visit(body);
            visit(atid);

            const ast::identifier& id   = aid.get_identifier();
            const ast::type& ty         = aid.get_type();

            visit(id);
            eval_type(ty);
        };

        /// type vars are allowed as some type
        void eval(const ast::pack_term& ast)
        {
            using annot_id      = ast::annotated_type_identifier;
            const annot_id& var = ast.get_var();
            const ast::term& ex = ast.get_body();
            const ast::type& ty = ast.get_var_type();
            const ast::type& st = ast.get_some_type();

            visit(var);
            visit(ex);
            visit(ty);

            if (sema::get_head_type(st) != ast::details::type_code::type_var)
                visit(st);
        };

        void eval(const ast::cell_term& ast)
        {
            error::scope_error_msg(m_eh).error_cell_term_not_allowed_parse(ast->get_location());
        };

        void eval(const ast::term_kind_scope& ast)
        {
            error::scope_error_msg(m_eh).error_kind_scope_not_allowed(ast->get_location());
        }

        void eval(const ast::closure_term& ast)
        {
            error::scope_error_msg(m_eh).error_closure_term_not_allowed(ast->get_location());
        };

        void eval(const ast::overloaded_symbol& ast)
        {
            error::scope_error_msg(m_eh).error_overloaded_symbol_not_allowed(ast->get_location());
        };

        //use default traversal                
        //void eval(const ast::term_type_application& ast);
        //void eval(const ast::term_kind_application& ast);
        //void eval(const ast::tuple_term& ast);
        //void eval(const ast::term_sequence& ast);
        //void eval(const ast::list_term& ast);
        //void eval(const ast::bracket_term& ast);
        //void eval(const ast::int_literal& ast);
        //void eval(const ast::bool_literal& ast);
        //void eval(const ast::float_literal& ast);
        //void eval(const ast::char_literal& ast);
        //void eval(const ast::string_literal& ast);        
        //void eval(const ast::ref_term& ast);
        //void eval(const ast::assign_term& ast);
        //void eval(const ast::deref_term& ast);        
        //void eval(const ast::identifier_term& ast);        
        //void eval(const ast::annotated_term& ast);
        //void eval(const ast::if_term& ast);
        //void eval(const ast::term_application& ast);
        //void eval(const ast::let_term& ast);
        //void eval(const ast::path_term& ast);
        //void eval(const ast::path_int_term& ast);
        //void eval(const ast::pack_term& ast);        
        //void eval(const ast::val_term& ast);

        //---------------------------------------------------------------
        //               TYPES
        //---------------------------------------------------------------
        void eval_toplevel_type(const ast::type& ty)
        {
            ast::type body = ty;
            if (ty->get_type_code() == ast::details::type_code::type_kind_scope)
            {
                ast::type_kind_scope ks = ty.static_cast_to<ast::type_kind_scope>();                
                process_kind_scope(ks, body);
                return;
            };

            if (ty->get_type_code() == ast::details::type_code::type_constant)
                return;
            else
                return eval_type(ty);
        };

        void eval_type(const ast::type& ty)
        {
            if (ty->get_type_code() == ast::details::type_code::type_var)
            {
                ast::type_var tv    = ty.static_cast_to<ast::type_var>();

                if (tv.is_set() == true)
                {
                    error::scope_error_msg(m_eh).error_unexpected_filled_type_var
                                                    (ty->get_location(), tv);
                    return;
                }
            }
            else
            {
                visit(ty);
            }
        };

        template<class Ast>
        void check_unique_names_types(const Ast& ast, check_unique_id& check, ast::type& body)
        {
            body = ast.get_body();
            const ast::annotated_type_identifier& aid   = ast.get_var();
            const ast::identifier& id   = aid.get_identifier();
            const ast::kind& ki         = aid.get_kind();

            visit(ki);
            check.add(id);

            if (body->get_type_code() == ast::details::type_code::some_type)
            {
                ast::some_type st = body.static_cast_to<ast::some_type>();
                return check_unique_names_types(st, check, body);
            }
            else if (body->get_type_code() == ast::details::type_code::all_type)
            {
                ast::all_type st = body.static_cast_to<ast::all_type>();
                return check_unique_names_types(st, check, body);
            }
            else if (body->get_type_code() == ast::details::type_code::lambda_type)
            {
                ast::lambda_type st = body.static_cast_to<ast::lambda_type>();
                return check_unique_names_types(st, check, body);
            }
            else if (body->get_type_code() == ast::details::type_code::mu_type)
            {
                ast::mu_type st = body.static_cast_to<ast::mu_type>();
                return check_unique_names_types(st, check, body);
            }
            else
            {
                return;
            }
        }

        //all names must be different; underscores not allowed
        void eval(const ast::some_type& ast)
        {
            ast::type body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_types(ast, check_names, body);
            visit(body);
        }

        //all names must be different; underscores not allowed
        void eval(const ast::all_type& ast)
        {
            ast::type body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_types(ast, check_names, body);
            visit(body);
        }

        //all names must be different; underscores not allowed
        void eval(const ast::lambda_type& ast)
        {
            ast::type body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_types(ast, check_names, body);
            visit(body);
        }

        //all names must be different; underscores not allowed
        void eval(const ast::mu_type& ast)
        {
            ast::type body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_types(ast, check_names, body);
            visit(body);
        };

        void eval(const ast::type_constant& ast)
        {
            error::scope_error_msg(m_eh).error_type_constant_outside_type_definition
                                (ast->get_location());
        };

        void eval(const ast::type_kind_scope& ast)
        {
            error::scope_error_msg(m_eh).error_kind_scope_not_allowed(ast->get_location());
        }        

        //labels must be different
        void eval(const ast::record_type& ast)
        {
            long k          = ast.size();
            using annot_id  = ast::annotated_type_identifier;

            check_unique_id check_names(m_eh, false);

            for (long i = 0; i < k; ++i)
            {
                const ast::identifier& id   = ast.get_identifier(i).get_identifier();
                check_names.add(id);
            };

            base_type::eval(ast);
        };

        //labels must be different
        void eval(const ast::variant_type& ast)
        {
            long k          = ast.size();
            using annot_id  = ast::annotated_type_identifier;

            check_unique_id check_names(m_eh, false);

            for (long i = 0; i < k; ++i)
            {
                const ast::identifier& id   = ast.get_identifier(i).get_identifier();
                check_names.add(id);
            };

            base_type::eval(ast);
        };

        void eval(const ast::type_var& ast)
        {
            if (m_allow_type_var == true)
                return;

            error::scope_error_msg(m_eh).error_type_var_not_allowed(ast->get_location());
        };
        
        //use default traversal
        //void eval(const ast::arrow_type& ast);
        //void eval(const ast::ref_type& ast);
        //void eval(const ast::if_type& ast);
        //void eval(const ast::path_int_type& ast);
        //void eval(const ast::list_type& ast);
        //void eval(const ast::identifier_type& ast);
        //void eval(const ast::tuple_type& ast);
        //void eval(const ast::bracket_type& ast);
        //void eval(const ast::annotated_type& ast);
        //void eval(const ast::type_application& ast);
        //void eval(const ast::predefined_type& ast);
        //void eval(const ast::int_type& ast);
        //void eval(const ast::error_type& ast);
        //void eval(const ast::bool_type& ast);
        //void eval(const ast::char_type& ast);
        //void eval(const ast::string_type& ast);
        //void eval(const ast::type_kind_application& ast);
        //void eval(const ast::type_type_application& ast);
        //void eval(const ast::product_of_types& ast)
        //void eval(const ast::type_sequence& ast)

        //---------------------------------------------------------------
        //               KINDS
        //---------------------------------------------------------------
        //use default traversal
        //void eval(const ast::star_kind& ast);
        //void eval(const ast::arrow_kind& ast);
        //void eval(const ast::star_arrow_kind& ast);
        //void eval(const ast::value_kind& ast);
        //void eval(const ast::kind_identifier& ast);        
        //void eval(const ast::product_kind& ast);
        //void eval(const ast::star_tuple_kind& ast);
        //void eval(const ast::star_all_kind& ast);
        //void eval(const ast::star_some_kind& ast);
        //void eval(const ast::star_list_kind& ast);
        //void eval(const ast::star_record_kind& ast);
        //void eval(const ast::star_variant_kind& ast);
        //void eval(const ast::star_predefined_kind& ast);
        //void eval(const ast::star_ref_kind& ast);

        void eval_toplevel_kind(const ast::kind& ki)
        {
            if (ki->get_kind_code() == ast::details::kind_code::kind_scope)
            {
                ast::kind_scope ks = ki.static_cast_to<ast::kind_scope>();
                process_kind_scope(ks);
                return;
            }
            else
            {
                return visit(ki);
            }
        };

        void eval(const ast::kind_scope& ast)
        {
            error::scope_error_msg(m_eh).error_kind_scope_not_allowed(ast->get_location());
        }

        void eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                visit(ast.get());
        }

        void process_kind_scope(const ast::kind_scope& ast)
        {
            ast::kind body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_kind(ast, check_names, body);
            visit(body);
        }

        void process_kind_scope(const ast::type_kind_scope& ast, ast::type& body)
        {
            check_unique_id check_names(m_eh, false);
            check_unique_names_kind(ast, check_names, body);
        }

        void process_kind_scope(const ast::term_kind_scope& ast)
        {
            ast::term body;
            check_unique_id check_names(m_eh, false);
            check_unique_names_kind(ast, check_names, body);

            if (body->get_term_code() == ast::details::term_code::term_constant)
                return;
            else
                return visit(body);
        }

        template<class Ast, class Ret>
        void check_unique_names_kind(const Ast& ast, check_unique_id& check, Ret& body)
        {
            body = ast.get_body();
            const ast::identifier& id   = ast.get_var();

            check.add(id);

            if (get_code<Ret>::eval(body) == get_code<Ret>::eval(ast))
            {
                Ast st = body.static_cast_to<Ast>();
                return check_unique_names_kind(st, check, body);
            }
            else
            {
                return;
            }
        }
};

void sema::check_constraints(const ast::definitions& ast, const error_handler_ptr& eh)
{
    constraints_visitor cv(eh);
    cv.visit(ast);
}

}};
