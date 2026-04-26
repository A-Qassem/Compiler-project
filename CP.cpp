#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct T {
    string t;
    string v;
    int l;
    int c;
};

string src;
vector<T> ts;
int idx = 0;
bool ok = true;

bool let(char c) {
    return isalpha((unsigned char)c);
}

bool dig(char c) {
    return isdigit((unsigned char)c);
}

bool pos(string s) {
    for (char c : s) {
        if (c != '0') {
            return true;
        }
    }
    return false;
}

void add(string t, string v, int l, int c) {
    ts.push_back({t, v, l, c});
}

void err(string s, int l, int c) {
    cout << "Error at " << l << ":" << c << " - " << s << "\n";
    ok = false;
}

void lex() {
    int i = 0;
    int l = 1;
    int c = 1;
    while (i < (int)src.size()) {
        int st = 0;
        string v = "";
        int sl = l;
        int sc = c;
        char ch = src[i];
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            i++;
            c++;
            continue;
        }
        if (ch == '\n') {
            i++;
            l++;
            c = 1;
            continue;
        }
        while (true) {
            if (st == 0) {
                if (let(ch)) {
                    st = 1;
                    v += ch;
                    i++;
                    c++;
                } else if (dig(ch)) {
                    st = 2;
                    v += ch;
                    i++;
                    c++;
                } else if (ch == '=') {
                    st = 3;
                    v += ch;
                    i++;
                    c++;
                } else if (ch == '+' || ch == '-' || ch == '>') {
                    v += ch;
                    add("OP", v, sl, sc);
                    i++;
                    c++;
                    break;
                } else if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ';') {
                    v += ch;
                    add(v, v, sl, sc);
                    i++;
                    c++;
                    break;
                } else {
                    if (!pos(v)) {
                        err("number must be positive", sl, sc);
                        break;
                    }
                    string s = "invalid character ";
                    s += ch;
                    err(s, sl, sc);
                    i++;
                    c++;
                    break;
                }
            } else if (st == 1) {
                if (i < (int)src.size() && (let(src[i]) || dig(src[i]))) {
                    ch = src[i];
                    v += ch;
                    i++;
                    c++;
                } else {
                    if (v == "solve" || v == "read" || v == "print" || v == "loop" || v == "if") {
                        add(v, v, sl, sc);
                    } else {
                        add("ID", v, sl, sc);
                    }
                    break;
                }
            } else if (st == 2) {
                if (i < (int)src.size() && dig(src[i])) {
                    ch = src[i];
                    v += ch;
                    i++;
                    c++;
                } else if (i < (int)src.size() && let(src[i])) {
                    err("invalid number", sl, sc);
                    while (i < (int)src.size() && (let(src[i]) || dig(src[i]))) {
                        i++;
                        c++;
                    }
                    break;
                } else {
                    add("NUM", v, sl, sc);
                    break;
                }
            } else if (st == 3) {
                if (i < (int)src.size() && src[i] == '=') {
                    v += src[i];
                    i++;
                    c++;
                    add("OP", v, sl, sc);
                    break;
                }
                add("=", v, sl, sc);
                break;
            }
            if (i >= (int)src.size()) {
                if (st == 1) {
                    if (v == "solve" || v == "read" || v == "print" || v == "loop" || v == "if") {
                        add(v, v, sl, sc);
                    } else {
                        add("ID", v, sl, sc);
                    }
                } else if (st == 2) {
                    if (!pos(v)) {
                        err("number must be positive", sl, sc);
                    } else {
                        add("NUM", v, sl, sc);
                    }
                } else if (st == 3) {
                    add("=", v, sl, sc);
                }
                break;
            }
            ch = src[i];
        }
    }
    add("EOF", "", l, c);
}

T cur() {
    return ts[idx];
}

bool is(string t) {
    return cur().t == t;
}

bool eat(string t) {
    if (is(t)) {
        idx++;
        return true;
    }
    err("expected " + t + " but found " + cur().t, cur().l, cur().c);
    return false;
}

bool term() {
    if (is("ID") || is("NUM")) {
        idx++;
        return true;
    }
    err("expected term", cur().l, cur().c);
    return false;
}

bool expr() {
    if (is("read")) {
        eat("read");
        eat("(");
        eat(")");
        return true;
    }
    if (!term()) {
        return false;
    }
    if (is("OP") && (cur().v == "+" || cur().v == "-")) {
        idx++;
        return term();
    }
    return true;
}

bool cond() {
    if (!term()) {
        return false;
    }
    if (is("OP") && (cur().v == ">" || cur().v == "==")) {
        idx++;
    } else {
        err("expected condition operator", cur().l, cur().c);
        return false;
    }
    return term();
}

bool stmt();

bool list() {
    while (is("ID") || is("loop") || is("if") || is("print")) {
        if (!stmt()) {
            return false;
        }
    }
    return true;
}

bool asn() {
    eat("ID");
    eat("=");
    expr();
    eat(";");
    return ok;
}

bool lop() {
    eat("loop");
    eat("(");
    cond();
    eat(")");
    eat("{");
    list();
    eat("}");
    return ok;
}

bool iff() {
    eat("if");
    eat("(");
    cond();
    eat(")");
    eat("{");
    list();
    eat("}");
    return ok;
}

bool prt() {
    eat("print");
    eat("(");
    term();
    eat(")");
    eat(";");
    return ok;
}

bool stmt() {
    if (is("ID")) {
        return asn();
    }
    if (is("loop")) {
        return lop();
    }
    if (is("if")) {
        return iff();
    }
    if (is("print")) {
        return prt();
    }
    err("expected statement", cur().l, cur().c);
    return false;
}

bool prog() {
    eat("solve");
    eat("{");
    list();
    eat("}");
    if (!is("EOF")) {
        err("unexpected token", cur().l, cur().c);
    }
    return ok;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        ifstream f(argv[1]);
        if (!f) {
            cout << "Cannot open file\n";
            return 1;
        }
        string s;
        while (getline(f, s)) {
            src += s;
            src += '\n';
        }
    } else {
        string s;
        while (getline(cin, s)) {
            src += s;
            src += '\n';
        }
    }
    lex();
    if (ok && prog()) {
        cout << "Accepted\n";
        return 0;
    }
    cout << "Rejected\n";
    return 1;
}
