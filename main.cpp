#include "utils.hpp"

class Lexer {
private:
    std::string_view src;
    std::size_t cursor;

    // helper functions
    inline bool
    isEOF(void) const { return cursor >= src.length(); }

    inline char
    peek(void) const { if(isEOF()) return '\0'; return src[cursor]; }

    inline char
    peekNext(void) const {
        if(cursor + 1 >= src.length())
            return '\0';
        return src[cursor + 1];
    }

    inline char
    peekAhead(std::size_t offset) const {
        if(cursor + offset >= src.length())
            return '\0';
        return src[cursor + offset];
    }

    inline char
    advance(void){ if(isEOF()) return '\0'; return src[cursor++]; }

    inline void
    skipWhitespaceAndComments(void) {
        while(!isEOF()){
            if(
                std::isspace(
                    static_cast<unsigned char>(peek())
                )
            ){
                advance();
                continue;
            }

            if(
                peek() == '/' && 
                (peekNext() == '/' || peekNext() == '*')
            ){
                handleComment();
                continue;
            }
            break;
        }
    }

    inline void
    handleComment(void){
        if(peek() == '/' && peekNext() == '/') {
            advance(); advance();
            while(!isEOF() && peek() != '\n') 
                advance(); 
            return;
        }

        if(peek() == '/' && peekNext() == '*'){
            advance(); advance();
            while(!isEOF()) {
                if(peek() == '*' && peekNext() == '/'){
                    advance(); advance();
                    break;
                }
                advance();
            }
            return;
        }
    }

    std::optional<Token>
    tryMultiCharToken(void){
        char c = peek();
        char n = peekNext();

        struct 
        Pattern {
            char first;
            char second;
            Tokentype type;
        };

        static const std::vector<Pattern> patterns = {
            { '=', '=', Tokentype::eq_eq },
            { '!', '=', Tokentype::neq },
            { '+', '=', Tokentype::add_eq },
            { '-', '=', Tokentype::sub_eq },
            { '-', '>', Tokentype::arrow },
            { '<', '=', Tokentype::le },
            { '>', '=', Tokentype::ge },
            { '+', '+', Tokentype::inc },
            { '-', '-', Tokentype::dec },
            { '&', '&', Tokentype::andd },
            { ':', ':', Tokentype::scope_res },
            { '|', '|', Tokentype::orr }
        };

        for(const auto& p : patterns){
            if(c == p.first && n == p.second){
                advance(); advance();
                return Token{ p.type, src.substr(cursor - 2, 2) };
            }
        }
        return std::nullopt;
    }

    Tokentype
    getKeywordOrType(
        const std::string_view value
    ){
        static const std::unordered_map<std::string_view, Tokentype> map = {
            // types
            { "i8", Tokentype::t_i8 },
            { "i16", Tokentype::t_i16 },
            { "i32", Tokentype::t_i32 },
            { "i64", Tokentype::t_i64 },
            { "u8", Tokentype::t_u8 },
            { "u16", Tokentype::t_u16 },
            { "u32", Tokentype::t_u32 },
            { "u64", Tokentype::t_u64 },
            { "f8", Tokentype::t_f8 },
            { "f16", Tokentype::t_f16 },
            { "f32", Tokentype::t_f32 },
            { "f64", Tokentype::t_f64 },
            { "bool", Tokentype::t_bool },
            { "char", Tokentype::t_char },
            { "void", Tokentype::t_void },
            { "str", Tokentype::t_str },
            // keywords
            { "mut", Tokentype::kw_mut },
            { "if", Tokentype::kw_if },
            { "for", Tokentype::kw_for },
            { "while", Tokentype::kw_while },
            { "ret", Tokentype::kw_ret },
            { "else", Tokentype::kw_else },
            { "main", Tokentype::main_ep },
            { "f", Tokentype::kw_f },
            // boolean values
            { "true", Tokentype::bool_val },
            { "false", Tokentype::bool_val },
        };

        auto it = map.find(value);
        if(it != map.end())
            return it->second;
        return Tokentype::identifier;
    }

    Token
    handleNumber(void) {
        std::size_t start = cursor;

        while(
            !isEOF() && 
            std::isdigit(
                static_cast<unsigned char>(peek())
            )){ advance(); }

        if(peek() == '.'){
            advance();
            while(
                !isEOF() && 
                std::isdigit(
                    static_cast<unsigned char>(peek())
            )){ advance(); }
        }

        return { Tokentype::num, src.substr(start, cursor - start) };
    }

    Token
    handleStringLiteral(void){
        std::size_t start = cursor;
        advance();

        while(!isEOF() && peek() != '"'){
            if(peek() == 'f'){
                if(peek() == '\\'){
                    advance();
                    if(!isEOF()) advance();
                }
            }

            if(peek() == '\\'){
                advance();
                if(!isEOF()) advance();
            }else advance();
        }
        if(peek() == '"') advance();
        return { Tokentype::str_lit, src.substr(start + 1, cursor - start - 2) };
    }

    Token
    handleCharLiteral(void){
        std::size_t start = cursor;
        advance();

        if(peek() == '\\'){
            advance();
            if(!isEOF()) advance();
        }else if(!isEOF() && peek() != '\'') advance();

        if(peek() == '\'') advance();

        return { Tokentype::char_lit, src.substr(start + 1, cursor - start - 2) };
    }

    Token
    handleIdentifierOrKeyword(void){
        std::size_t start = cursor;
        while(
            !isEOF() && 
            (
                std::isalnum(
                    static_cast<unsigned char>(peek())
                ) || peek() == '_'
            )
        ){ advance(); }
        std::string_view value = src.substr(start, cursor - start);

        std::size_t temp_cursor = cursor;
        while(
            temp_cursor < src.length() && 
            std::isspace(
                static_cast<unsigned char>(src[temp_cursor])
            )
        ){ temp_cursor++; }

        if(
            temp_cursor < src.length() && 
            src[temp_cursor] == '('
        ){
            Tokentype type = getKeywordOrType(value);
            if(type == Tokentype::identifier){
                return { Tokentype::fn, value };
            }
            return { type, value };
        }

        Tokentype type = getKeywordOrType(value);
        return { type, value };

    }

    Token
    handleSingleChar(const char curr){
        static const std::unordered_map<char, Tokentype> charMap = {
            { '!', Tokentype::nott },
            { '+', Tokentype::add },
            { '-', Tokentype::sub },
            { '*', Tokentype::mult },
            { '/', Tokentype::div },
            { '=', Tokentype::eq },
            { '<', Tokentype::lt },
            { '>', Tokentype::gt },
            { '(', Tokentype::par_l },
            { ')', Tokentype::par_r },
            { '[', Tokentype::br_l },
            { ']', Tokentype::br_r },
            { '{', Tokentype::cbr_l },
            { '}', Tokentype::cbr_r },
            { ';', Tokentype::instr_end },
            { ':', Tokentype::colon },
            { '.', Tokentype::dot },
            { ',', Tokentype::comma },
        };

        auto it = charMap.find(curr);
        if(it != charMap.end()){
            advance();
            return { it->second, src.substr(cursor - 1, 1) };
        }
        return { Tokentype::invalid, "" };
    }

    Token
    next(void){
        skipWhitespaceAndComments();
        if(isEOF()) return { Tokentype::eof, "" };

        if(auto multi = tryMultiCharToken()){ return *multi; }

        char curr = peek();
        if(curr == '"') return handleStringLiteral();
        if(curr == '\'') return handleCharLiteral();
        
        Token single_char = handleSingleChar(curr);
        if(single_char.type != Tokentype::invalid)
        { return single_char; }

        if(std::isdigit(static_cast<unsigned char>(curr)))
        { return handleNumber(); }

        if(std::isalpha(static_cast<unsigned char>(curr)) || curr == '_')
        { return handleIdentifierOrKeyword(); }

        advance();
        return { Tokentype::invalid, src.substr(cursor - 1, 1) };
    }

public:
    explicit Lexer(std::string_view src) : src(src), cursor(0) {}

    std::vector<Token> 
    tokenize(void){
        std::vector<Token> tokens;
        tokens.reserve(128);

        while(true){
            Token token = next();
            tokens.push_back(token);
            if(token.type == Tokentype::eof) break;
        }
        return tokens;
    }
};

int
main(void){
    std::string code = R"(
        main(void): i8 {
            print("hello world\n");

            num: mut i32[] = [1, 2, 3, 4, 5];            
            for(i: mut i8, inRange(0, 5)){
                println(num[i]);
            }
            
            ret 0;
        }
    )";

    Lexer lexer_instance(code);
    std::vector<Token> tokens = lexer_instance.tokenize();

    for(const auto& token : tokens){
        std::cout << to_string(token.type) << ": \"" << token.value << "\"" << std::endl;
    }

    return 0;
}