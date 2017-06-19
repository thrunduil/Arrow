private:
    using unique_string     = ast::unique_string;

private:
	long                    m_token_start_offset;
	long                    m_current_offset;
    error_handler_ptr       m_eh;
    unique_string           m_file;
    bool                    m_shell_mode;

    int32_t                 m_next_code_point;
    
    // The lexer used to parse user input in the console
    // should allow unterminated comments
    bool                    allow_unterminated_comments;

private:
    virtual void            consume() override;
    virtual void            resetText() override;
    bool                    is_begin_line(const antlr::LexerSharedInputState& inputState);
    // remove last symbols
    void                    pop_symbol(int num_symbols);
    void                    push_symbol(const std::string& sym);

    void                    reportError(const antlr::RecognitionException& ex) override;
    void                    reportError(const std::string& s) override;
    void                    reportWarning(const std::string& s) override;
    unique_string           get_file();

    int32_t                 check_next_code_point();

public:
    void                    init(const error_handler_ptr& eh, unique_string file, 
                                int tab_size, bool shell_mode);
    void                    set_location(const ast::location& loc);