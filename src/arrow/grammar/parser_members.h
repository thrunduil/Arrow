private:
    using location          = ast::location;
    using unique_string     = ast::unique_string;
    using int_type          = int64_t;
                            /// import x alias x location
    using string_loc        = std::tuple<unique_string, unique_string, ast::location>;
    using annot_polarity    = std::pair<ast::annotated_type_identifier, ast::polarity_type>;
    using type_id_vec       = std::vector<annot_polarity>;
    using type_id_vec2      = std::vector<ast::annotated_type_identifier>;
    using term_id_vec       = std::vector<ast::annotated_identifier>;
    using kind_id_vec       = std::vector<ast::identifier>;
    using id_type           = std::pair<ast::annotated_type_identifier, ast::type>;
    using id_type_vec       = std::vector<id_type>;
    using kind_vec          = std::vector<ast::kind>;

private:
    error_handler_ptr       m_eh;
    unique_string           m_file;
    bool                    m_shell_mode;
    lexer_arrow*            m_lexer;

private:
    void                    reportError(const antlr::RecognitionException& ex);
    void                    reportError(const std::string& s);
    void                    reportWarning(const std::string& s);

    void                    consume_until_semi();
    bool                    is_command_start(int token);
    antlr::BitSet           make_bitset_command();

    ast::location           get_location();
    unique_string           get_file();
    ast::location           get_location(const antlr::RefToken& token);
    std::string             get_text(const antlr::RefToken& token);
    std::string             get_text_cont(const antlr::RefToken& token, bool& can_continue);
    int64_t                 get_int64(const antlr::RefToken& token);

public:
    void                    init(const error_handler_ptr& eh, unique_string file,
                                bool m_shell_mode, lexer_arrow* lexer);
    bool                    at_eof();

    //--------------------------------------------------
    //              helpers
    //--------------------------------------------------
    ast::definitions        make_definitions(const ast::location& loc);
    ast::identifier         make_basic_identifier(const ast::location& loc, const std::string& str);
    ast::identifier         make_operator_identifier(const ast::location& loc, const std::string& str);
    ast::annotated_type_identifier
                            make_annotated_type_id(const ast::identifier& id, const ast::kind& ki);
    ast::annotated_type_identifier
                            make_annotated_type_id(const ast::identifier& id);
    ast::annotated_identifier
                            make_annotated_id(const ast::location& loc, const ast::identifier& id, 
                                const ast::type& ta);
    ast::annotated_identifier
                            make_annotated_underscore(const ast::location& loc, const ast::type& ta);
    ast::identifier         make_underscore(const ast::location& loc);
    ast::module_definition  make_module_definition(const std::string& name, const std::vector<string_loc>& imp,
                                const ast::definitions& defs);
    string_loc              make_import(const std::string& import, const std::string& alias, 
                                const ast::location& loc);

    template<class Type>
    Type                    make_bracket(const antlr::RefToken& start, Type& ex, 
                                const antlr::RefToken& end);

    //--------------------------------------------------
    //              definitions
    //--------------------------------------------------
    ast::term_definition    make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars,
                                const ast::type& t, const ast::term& e, bool over);
    ast::term_definition    make_overload_def(const ast::identifier& id, const kind_id_vec& kind_vars);
    ast::term_definition    make_extern_def(const ast::identifier& id, const kind_id_vec& kind_vars,
                                const ast::type& t, bool over);
    ast::term_definition    make_term_constant(const ast::identifier& id, const kind_id_vec& kind_vars,
                                const ast::type& t, bool over);
    ast::term_definition    make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                                const ast::term& e, bool over);
    ast::term_definition    make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                                const term_id_vec& args, const ast::type& ret, const ast::term& e, 
                                bool over);

    ast::type_definition    make_type_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                                const ast::kind& k, const ast::type& t, bool over);
    ast::type_definition    make_type_def_over(const ast::identifier& id, const kind_id_vec& kind_vars);

    ast::type_definition    make_type_def_extern(const ast::identifier& id, const kind_id_vec& kind_vars, 
                                const ast::kind& k, bool over);
    ast::term_use           make_term_use(const ast::term& t, const kind_id_vec& kind_vars);
    ast::type_definition_var
                            make_type_def_var(const ast::identifier& id, const kind_id_vec& kind_vars,
                                bool over);
    void                    set_type_def_body(ast::type_definition_var& td, const ast::kind& ki, 
                                const ast::type& t);
    ast::definition         make_fixity_definition(const ast::location& loc, const ast::operator_property& p, 
                                const ast::symbol_sequence& syms);
    void                    finish_operator_declaration(const ast::location& loc, ast::operator_info& p,
                                const ast::symbol_sequence& syms);

    //--------------------------------------------------
    //              terms
    //--------------------------------------------------
    ast::unpack_term        make_unpack(const ast::location& loc, const ast::term& e, 
                                const ast::annotated_type_identifier& id1, 
                                const ast::annotated_identifier& id2, const ast::term& t);
    ast::term               make_annotated_term(const ast::term& x, const ast::type& t);
    ast::all_term           make_all_term(const ast::location& loc, const type_id_vec& vars, 
                                const ast::term& t);
    ast::term               make_if_term(const ast::location& loc, const ast::term& t_if, 
                                const ast::term& t_then, const ast::term& t_else);
    ast::term               make_pack(const ast::location& loc, const id_type_vec& args, 
                                const ast::term& e);
    ast::term               make_let(const ast::location& loc, const ast::definitions& defs, 
                                const ast::term& body);
    ast::term               make_term_app(const ast::term& x1, const ast::term& x2);
    ast::term               make_lambda_term(const ast::location& loc, const term_id_vec& vars, 
                                const ast::type& ret, const ast::term& body);
    ast::term               make_mu_term(const ast::location& loc, const term_id_vec& vars,
                                const ast::term& body);

    ast::term_type_application
                            make_term_type_app(const ast::term& x, const ast::type& t);
    ast::term               make_ref_term(const ast::location& loc, const ast::term& t);
    ast::term               make_deref_term(const ast::location& loc, const ast::term& t);
    ast::term               make_identifier_term(const ast::identifier& id);
    ast::term               make_variant_term(const ast::location& loc, const ast::type& ty, 
                                const ast::annotated_identifier& id, const ast::term& e);
    ast::int_literal        make_int(const ast::location& loc, const std::string& txt);
    ast::bool_literal       make_boolean(const ast::location& loc, bool val);
    ast::char_literal       make_char(const ast::location& loc, const std::string& txt);
    ast::string_literal     make_string(const ast::location& loc, const std::string& txt);
    ast::term               make_float(const ast::location& loc, const std::string& txt);    
    ast::term               make_assign_term(const ast::term& lhs, const ast::term& rhs);
    ast::term               make_val_term(const ast::location& loc, const ast::type& t);
    ast::term               make_term_kind_scope(const ast::location&, const kind_id_vec& vars, 
                                const ast::term& body);
    ast::bracket_term       make_bracket_expr(const antlr::RefToken& start, ast::bracket_term& ex, 
                                const antlr::RefToken& end);


    //--------------------------------------------------
    //              kinds
    //--------------------------------------------------
    ast::kind               make_kind_var(const ast::location& loc);
    ast::kind               make_arrow_kind(const ast::kind& k1, ast::polarity_type kat, 
                                const ast::kind& k2);
    ast::kind               make_star_kind(const ast::location& loc);
    ast::kind               make_int_kind(const ast::location& loc);
    ast::kind               make_bool_kind(const ast::location& loc);
    ast::kind               make_char_kind(const ast::location& loc);
    ast::kind               make_string_kind(const ast::location& loc);
    ast::kind               make_kind_scope(const ast::location& loc, const kind_id_vec& vars, 
                                const ast::kind& body);
    ast::kind               make_kind_identifier(const ast::location& loc, const ast::identifier& name);
    void                    mark_kind_identifier(const ast::identifier& id);
    void                    mark_type_identifier(const ast::identifier& id);
    void                    mark_term_identifier(const ast::identifier& id);
    ast::kind               make_all_kind_star(const ast::location& loc, const kind_vec& kv, 
                                const ast::kind& body);
    ast::kind               make_some_kind_star(const ast::location& loc, const kind_vec& kv, 
                                const ast::kind& body);
    ast::kind               make_arrow_kind_star(const ast::kind& lhs, const ast::kind& rhs);
    ast::kind               make_tuple_kind_star(const ast::location& loc, const kind_vec& kv, 
                                bool fixed);
    ast::kind               make_list_kind_star(const ast::location& loc, const ast::kind& ki);
    ast::kind               make_ref_kind_star(const ast::location& loc, const ast::kind& ki);
    ast::kind               make_bool_kind_star(const ast::location& loc);
    ast::kind               make_int_kind_star(const ast::location& loc);
    ast::kind               make_float_kind_star(const ast::location& loc);
    ast::kind               make_string_kind_star(const ast::location& loc);
    ast::kind               make_char_kind_star(const ast::location& loc);

    //--------------------------------------------------
    //              types
    //--------------------------------------------------
    ast::type               make_arrow_type(const ast::type& t1, const ast::type& t2);
    ast::all_type           make_all_type(const ast::location& loc, const type_id_vec2& vars, 
                                const ast::type& body);
    ast::some_type          make_some_type(const ast::location& loc, const type_id_vec2& vars, 
                                const ast::type& body);
    ast::lambda_type        make_lambda_type(const ast::location& loc, const type_id_vec& vars,
                                const ast::kind& ret, const ast::type& body);
    ast::mu_type            make_mu_type(const ast::location& loc, const type_id_vec2& vars,
                                const ast::type& body);
    ast::type               make_ref_type(const ast::location& loc, const ast::type& t);
    ast::type               make_if_type(const ast::location& loc, const ast::type& t_if, 
                                const ast::type& t_then, const ast::type& t_else);
    ast::type               make_identifier_type(const ast::identifier& id);
    ast::variant_type       make_variant_type(const ast::location& loc);
    ast::type               make_type_application(const ast::type& t1, const ast::type& t2);
    ast::type               make_type_bool(const ast::location& loc);
    ast::type               make_type_int(const ast::location& loc);
    ast::type               make_type_float(const ast::location& loc);
    ast::type               make_type_char(const ast::location& loc);
    ast::type               make_type_string(const ast::location& loc);
    ast::record_type        make_record_type(const ast::location& loc);
    ast::type               make_type_var(const ast::location& loc);
    ast::type               make_list_type(const ast::location& loc, const ast::type& t);
    ast::type               make_type_constant(const ast::identifier& id, ast::kind ret_ki,
                                bool extern_def);
    ast::type               make_int_type(const ast::int_literal& il);
    ast::type               make_bool_type(const ast::bool_literal& il);
    ast::type               make_char_type(const ast::char_literal& il);
    ast::type               make_string_type(const ast::string_literal& il);
    ast::type               make_type_kind_scope(const ast::location&, const kind_id_vec& vars, 
                                const ast::type& body);
    ast::type               make_type_kind_application(const ast::type& t, const ast::kind& ki);
    ast::type               make_annotated_type(const ast::type& t, const ast::kind& ki);
    ast::type               make_type_path(const ast::type& x, const ast::int_literal& pos);    
    ast::bracket_type       make_bracket_type(const antlr::RefToken& start, ast::bracket_type& ex, 
                                const antlr::RefToken& end);

    //operators
    void                    add_operator_property_string(const ast::location& loc, ast::operator_property& prop, 
                                const std::string& prop_type, const std::string& prop_value);
    void                    add_operator_property_int(const ast::location& loc, ast::operator_property& prop, 
                                const std::string& prop_type, int_type prop_value);
    ast::identifier         make_operator(const ast::location& loc, const std::string& op_name);
    ast::identifier         make_brack_op(const ast::location& loc, const std::string& brack_op_name,
                                bool is_dot_brack);
    ast::identifier         make_brack_assign_op(const ast::location& loc, const std::string& brack_op_name,
                                bool is_dot_brack);

    ast::identifier         make_fixity_name(const ast::identifier& id, bool pref_under, bool post_under);
