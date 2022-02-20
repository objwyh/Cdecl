//
//  main.cpp
//  Cdecl
//
//  Created by 王逸含 on 2019/10/20.
//  Copyright © 2019 王逸含. All rights reserved.
//

#include <functional>
#include <iostream>
#include <stack>
#include <string>

enum TypeTag { TOKEN_IDENTIFIER, TOKEN_QUALIFIER, TOKEN_TYPE };

struct Token {
  char type;
  std::string str;
};

using std::cin;
using std::cout;
using std::endl;

int pos;
std::string buff;
struct Token CurToken;
std::stack<Token, std::deque<Token>> TokenStack;
std::function<void(void)> NextState;

TypeTag ClassifyString() {
  if (CurToken.str == "const") {
    CurToken.str = "read-only";
    return TOKEN_QUALIFIER;
  }

  if (CurToken.str == "volatile")
    return TOKEN_QUALIFIER;
  if (CurToken.str == "void")
    return TOKEN_TYPE;
  if (CurToken.str == "char")
    return TOKEN_TYPE;
  if (CurToken.str == "signed")
    return TOKEN_TYPE;
  if (CurToken.str == "unsigned")
    return TOKEN_TYPE;
  if (CurToken.str == "short")
    return TOKEN_TYPE;
  if (CurToken.str == "int")
    return TOKEN_TYPE;
  if (CurToken.str == "long")
    return TOKEN_TYPE;
  if (CurToken.str == "float")
    return TOKEN_TYPE;
  if (CurToken.str == "double")
    return TOKEN_TYPE;
  if (CurToken.str == "struct")
    return TOKEN_TYPE;
  if (CurToken.str == "union")
    return TOKEN_TYPE;
  if (CurToken.str == "enum")
    return TOKEN_TYPE;

  return TOKEN_IDENTIFIER;
}

void GetToken() {
  char c;
  CurToken.str = "";

  if (pos == buff.size()) {
    return;
  }

  while ((c = buff[pos++]) == ' ')
    ;

  if (std::isalnum(c)) {
    CurToken.str += c;
    while (std::isalnum((c = buff[pos++])))
      CurToken.str += c;
    pos--;
    CurToken.type = ClassifyString();
    return;
  }
  CurToken.str += c;
  CurToken.type = c;
  return;
}

void GetArray();
void GetParams();
void GetLparen();
void GetPointerPart();
void GetType();

void Initialize() {
  //  Initialize
  pos = 0;
  CurToken.str = "";
  CurToken.type = 0;
  while (!TokenStack.empty())
    TokenStack.pop();

  GetToken();
  while (CurToken.type != TOKEN_IDENTIFIER) {
    TokenStack.push(CurToken);
    GetToken();
  }

  cout << CurToken.str << " is ";
  cout.flush();
  GetToken();
  NextState = GetArray;
}

void GetArray() {
  NextState = GetParams;
  while (CurToken.type == '[') {
    cout << "array ";
    GetToken();
    if (isdigit(CurToken.str[0])) {
      cout << "0~" << atoi(CurToken.str.c_str()) - 1 << " ";
      cout.flush();
      GetToken();
    }
    GetToken();
    cout << "of ";
    cout.flush();
    NextState = GetLparen;
  }
}

void GetParams() {
  NextState = GetLparen;

  if (CurToken.type == '(') {
    while (CurToken.type == '(') {
      GetToken();
    }
    GetToken();
    cout << "fcuntion returning ";
    cout.flush();
  }
}

void GetLparen() {
  NextState = GetPointerPart;

  if (!TokenStack.empty()) {
    if (TokenStack.top().type == '(') {
      TokenStack.pop();
      GetToken();
      NextState = GetArray;
    }
  }
}

void GetPointerPart() {
  NextState = GetType;
  if (TokenStack.empty()) {
    cout << "unknow declaration" << endl;
    NextState = nullptr;
    return;
  }
  if (TokenStack.top().type == '*') {
    cout << "pointer to ";
    cout.flush();
    TokenStack.pop();
    NextState = GetLparen;
  } else if (TokenStack.top().type == TOKEN_QUALIFIER) {
    cout << TokenStack.top().str + " ";
    cout.flush();
    TokenStack.pop();
    NextState = GetLparen;
  }
}

void GetType() {
  NextState = nullptr;
  while (!TokenStack.empty()) {
    cout << TokenStack.top().str + " ";
    cout.flush();
    TokenStack.pop();
  }
  cout << endl;
}

void Cdecl() {
  NextState = Initialize;
  while (NextState != nullptr)
    NextState();
}

inline void PrintCLI() {
  cout << "cdecl> ";
  cout.flush();
}

int main(int argc, char *argv[]) {
  cout << "Enter quit to exit" << endl;
  do {
    PrintCLI();
    std::getline(cin, buff);
    if (buff.empty() || buff[0] == '\n')
      continue;
    if (buff == "quit")
      break;
    Cdecl();
  } while (1);

  return 0;
}