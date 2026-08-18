#ifndef UTILS_HPP
    #define UTILS_HPP

    #include <iostream>
    #include <vector>
    #include <cctype>
    #include <string>
    #include <string_view>
    #include <unordered_map>
    #include <optional>
    #include <memory>
    #include <variant>

    enum class 
    Tokentype {
        main_ep, fn, identifier, 
        num, bool_val, str_lit, char_lit,
        nott, andd, orr,
        add, sub, mult, div, eq, inc, dec,
        add_eq, sub_eq,
        eq_eq, neq,
        lt, gt,
        le, ge,
        arrow, dot, comma, scope_res,
        par_l, par_r, 
        br_l, br_r, 
        cbr_l, cbr_r,
        instr_end, colon,
        kw_mut, kw_if, kw_for, kw_while, kw_ret, kw_else, kw_f,
        t_i64, t_i32, t_i16, t_i8,
        t_u64, t_u32, t_u16, t_u8,
        t_void, t_bool, t_str, t_char,
        t_f64, t_f32, t_f16, t_f8,
        invalid, eof
    };

    struct 
    Token {
        Tokentype type;
        std::string_view value;
    };

    inline std::string_view
    to_string(const Tokentype type){
        static constexpr std::string_view names[] = {
            "MAIN_EP", "FUNCTION", "IDENTIFIER",
            "NUMBER", "BOOL_VALUE", "STR_LIT", "CHAR_LIT",
            "NOT", "AND", "OR",
            "ADD", "SUB", "MULT", "DIV", "EQ", "INC", "DEC",
            "ADD_EQ", "SUB_EQ",
            "EQ_EQ", "NEQ",
            "LT", "GT",
            "LE", "GE",
            "ARROW", "DOT", "COMMA", "SCOPE_RES",
            "PAR_L", "PAR_R",
            "BR_L", "BR_R",
            "CBR_L", "CBR_R",
            "INSTR_END", "COLON",
            "KW_MUT", "KW_IF", "KW_FOR", "KW_WHILE", "KW_RET", "KW_ELSE", "KW_F",
            "T_I64", "T_I32", "T_I16", "T_I8",
            "T_U64", "T_U32", "T_U16", "T_U8",
            "T_VOID", "T_BOOL", "T_STR", "T_CHAR",
            "T_F64", "T_F32", "T_F16", "T_F8",
            "INVALID", "EOF",
        };

        auto idx = static_cast<std::size_t>(type);
        return idx < std::size(names) ? names[idx] : "UNKNOWN";
    }

#endif