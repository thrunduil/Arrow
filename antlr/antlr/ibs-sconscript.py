Import('base_env')
env = base_env.Clone()

classes = Dir('classes')
env['JARCHDIR'] = [classes,]
env.Java(
    target = classes,
    source = 'antlr',
)

#antlr_jar = env.Jar(
#    target = 'antlr.jar',
#    source = classes,
#)

library_sources = [
    'lib/cpp/src/ANTLRUtil.cpp',
    'lib/cpp/src/ASTFactory.cpp',
    'lib/cpp/src/ASTNULLType.cpp',
    # 'lib/cpp/src/ASTRefCount.cpp',
    'lib/cpp/src/BaseAST.cpp',
    # 'lib/cpp/src/base_memory.cpp',
    'lib/cpp/src/BitSet.cpp',
    'lib/cpp/src/CharBuffer.cpp',
    'lib/cpp/src/CharScanner.cpp',
    'lib/cpp/src/CommonAST.cpp',
    'lib/cpp/src/CommonASTWithHiddenTokens.cpp',
    'lib/cpp/src/CommonHiddenStreamToken.cpp',
    'lib/cpp/src/CommonToken.cpp',
    # 'lib/cpp/src/dll.cpp',
    'lib/cpp/src/InputBuffer.cpp',
    'lib/cpp/src/LLkParser.cpp',
    'lib/cpp/src/MismatchedCharException.cpp',
    'lib/cpp/src/MismatchedTokenException.cpp',
    'lib/cpp/src/NoViableAltException.cpp',
    'lib/cpp/src/NoViableAltForCharException.cpp',
    'lib/cpp/src/Parser.cpp',
    'lib/cpp/src/RecognitionException.cpp',
    'lib/cpp/src/String.cpp',
    'lib/cpp/src/Token.cpp',
    'lib/cpp/src/TokenBuffer.cpp',
    # 'lib/cpp/src/TokenRefCount.cpp',
    'lib/cpp/src/TokenStreamBasicFilter.cpp',
    'lib/cpp/src/TokenStreamHiddenTokenFilter.cpp',
    'lib/cpp/src/TokenStreamRewriteEngine.cpp',
    'lib/cpp/src/TokenStreamSelector.cpp',
    'lib/cpp/src/TreeParser.cpp',
]

env.Append(CPPPATH = ['lib/cpp', '$REGION_INC_DIR'])

antlr_lib = env.StaticLibrary(
    target = 'antlr',
    source = library_sources,
)

#aliases = { 'antlr': [antlr_jar, antlr_lib], 'all': [antlr_jar, antlr_lib] }
aliases = { 'antlr': [antlr_lib], 'all': [antlr_lib] }

Return('aliases')
