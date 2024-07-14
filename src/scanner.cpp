#include "scanner.hpp"
#include <iostream>
#include <regex>

void Scanner::fillStrRefers(const std::string &content) {
    // 定义一个正则表达式来匹配 ${...} 和 \${...}
    std::regex pattern(R"(\$\{[^{}]*\}|\\\$\{[^{}]*\})");

    // 创建迭代器，首先获取非匹配的子串，然后获取匹配的子串
    std::sregex_token_iterator iter(content.begin(), content.end(), pattern, {-1, 0});
    std::sregex_token_iterator end;
    // 匹配字符串模板错误情况
    std::regex pattern2(R"([${}"])");
    std::smatch match;

    // 遍历并打印结果
    while (iter != end) {
        std::string token = *iter++;
        if (!token.empty()) {
            if (token.front() == '$') { // 判断是否为匹配的子串
                token = token.substr(2, token.length() - 3);
                if (std::regex_search(token, match, pattern2)) {
                    throw std::runtime_error{"string-expression error."};
                }
                Scanner scanner{token};
                scanner.line = line;
                scanner.getTokens();
                tokens.insert(tokens.cend(), scanner.tokens.cbegin(), scanner.tokens.cend() - 1);
            } else {
                LoxValuePtr v = std::make_shared<LoxString>(token);
                tokens.emplace_back(std::make_shared<Token>(TokenType::STRING, v, token, 0));
            }
        }
    }
}

void Scanner::parseString() {

    addToken(TokenType::STR_START);

    // 检查字符串完整性
    int i = start + 1;
    for (; i < program.length(); ++i) {
        if (program[i] == '"' and program[i - 1] != '\\') break; // 有效的结束双引号
    }
    if (i == program.length()) {
        std::cerr << "Line: " << line << ", Unterminated string." << std::endl;
        current = i;
        hasError = true;
        return;
    }

    auto ts = program.substr(current, i - start - 1);
    fillStrRefers(ts); // 获取可能是字符串模板的token数组

    start = i;
    current = i + 1;
    addToken(TokenType::STR_END);
}

void Scanner::parseNumber() {
    bool isFloat = false;
    while (isNum(peek())) advance();
    if (peek() == '.' and isNum(peekNext())) {
        isFloat = true;
        do advance(); while (isNum(peek()));
    }
    std::string value = program.substr(start, (current - start));
    if (isFloat) {
        addToken(TokenType::FLOATING, std::make_shared<LoxFloat>(std::stod(value)));
    } else {
        addToken(TokenType::INTEGER, std::make_shared<LoxInt>(std::stoll(value)));
    }
}

void Scanner::parseIdentifier() {
    while (isAlphaNum(peek())) advance();
    std::string value = program.substr(start, (current - start));
    TokenType tokenType;
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        tokenType = it->second;
    } else {
        tokenType = TokenType::IDENTIFIER;
    }
    addToken(tokenType);
}

void Scanner::parseComments() {
    if (match('/')) { // One_line comments
        while (not atEnd() and peek() != '\n') advance();
        return;
    }
    if (match('*')) { // Cross-line comments
        while (not(atEnd() or (peek() == '*' and peekNext() == '/'))) {
            if (peek() == '\n') line++;
            advance();
        }
        if (not atEnd()) {
            advance();
            advance();
        }
    }
}

std::optional<std::vector<TokenPtr>> Scanner::getTokens() {
    while (not atEnd()) {
        start = current;
        switch (char c = advance()) {
            case '(':addToken(TokenType::LEFT_PAREN);
                break;
            case ')':addToken(TokenType::RIGHT_PAREN);
                break;
            case '[':addToken(TokenType::LEFT_SQUARE);
                break;
            case ']':addToken(TokenType::RIGHT_SQUARE);
                break;
            case '{':addToken(TokenType::LEFT_BRACE);
                break;
            case '}':addToken(TokenType::RIGHT_BRACE);
                break;
            case ',':addToken(TokenType::COMMA);
                break;
            case ':':addToken(TokenType::COLON);
                break;
            case ';':addToken(TokenType::SEMICOLON);
                break;
            case '\n':line++;
                break;
            case '^':addToken(TokenType::BIT_XOR);
                break;
            case '~':addToken(TokenType::BIT_NOT);
                break;
            case '.':addToken(match('.') ? TokenType::RANGE : TokenType::DOT);
                break;
            case '!':addToken(match('=') ? TokenType::NOT_EQUAL : TokenType::NOT);
                break;
            case '&':addToken(match('&') ? TokenType::AND : TokenType::BIT_AND);
                break;
            case '|':addToken(match('|') ? TokenType::OR : TokenType::BIT_OR);
                break;
            case '=':addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                if (match('=')) {
                    addToken(TokenType::LESS_EQUAL);
                } else if (match('<')) {
                    addToken(TokenType::SHIFT_L);
                } else {
                    addToken(TokenType::LESS);
                }
                break;
            case '>':
                if (match('=')) {
                    addToken(TokenType::GREATER_EQUAL);
                } else if (match('>')) {
                    addToken(match('>') ? TokenType::SHIFT_RA : TokenType::SHIFT_R);
                } else {
                    addToken(TokenType::GREATER);
                }
                break;
            case '+':addToken(match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS);
                break;
            case '-':
                if (match('=')) {
                    addToken(TokenType::MINUS_EQUAL);
                } else if (match('>')) {
                    addToken(TokenType::ARROW);
                } else {
                    addToken(TokenType::MINUS);
                }
                break;
            case '%':addToken(match('=') ? TokenType::MOD_EQUAL : TokenType::MOD);
                break;
            case '*':
                if (match('=')) {
                    addToken(TokenType::STAR_EQUAL);
                } else if (match('*')) {
                    addToken(TokenType::POWER);
                } else {
                    addToken(TokenType::STAR);
                }
                break;
            case '/':
                if (peek() == '/' or peek() == '*') {
                    parseComments();
                } else {
                    addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
                }
                break;
            case '"':parseString();   // Literals
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\f':
            case '\v':break;
            default:
                if (isNum(c)) {
                    parseNumber();
                } else if (isAlpha(c)) {
                    parseIdentifier();
                } else {
                    std::cerr << "Line: " << line << ", Unexpected character [" << c << "]" << std::endl;
                    hasError = true;
                }
        }
    }

    if (hasError) {
        return std::nullopt;
    } else {
        addToken(TokenType::ENDMARKER);
        return tokens;
    }
}