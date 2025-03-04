#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <limits.h>
#include <stdlib.h>

// 词法单元类型枚举
enum {
  TK_NOTYPE = 256, // 空格串
  TK_EQ,       // ==

  /* TODO: Add more token types */
  TK_NEQ,      // !=
  TK_AND,      // &&
  TK_OR,       // ||
  TK_NOT,      // !
  TK_DEC,      // 十进制数
  TK_HEX,      // 十六进制数
  TK_REG,      // 寄存器
  TK_DEREF     // 解引用
};

// 正则表达式规则数组
static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // mul
  {"\\/", '/'},         // div
  {"==", TK_EQ},        // eq
  {"!=", TK_NEQ},       // neq
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},    // or
  {"!", TK_NOT},        // not
  {"[0-9]|([1-9][0-9]*)", TK_DEC},   // dec
  {"0[xX][0-9a-fA-F]+", TK_HEX},     // hex
  {"\\$[eE][0-9a-zA-Z]{2}", TK_REG}, // reg
  {"\\*\\([a-zA-Z0-9]+\\)", TK_DEREF}, // deref
  {"\\(", '('},         //left parentheses
  {"\\)", ')'},         //right parentheses
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

// 词法分析
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case TK_DEC:
          case TK_HEX:
          case TK_REG:{
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
          }
          default: {
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  for (int i = 0; i < nr_token; i++) {
    printf("token[%d]: type = %d, str = %s\n", i, tokens[i].type, tokens[i].str);
  }

  return true;
}

// 获取运算符优先级
static int get_priority(int type) {
  switch (type) {
    case TK_OR:{
      return 1;
    }
    case TK_AND:{
      return 2;
    }
    case TK_EQ:
    case TK_NEQ:{
      return 3;
    }
    case '+':
    case '-':
      return 4;
    case '*':
    case '/':
      return 5;
    case TK_DEREF:
    case TK_NOT:{
      return 6;
    }
    default:
      return INT_MAX;
  }
}

static bool is_operator(int type) {
  return get_priority(type) <= 6;
}

// 找到表达式中的主运算符（优先级最低的运算符）
static int find_dominant_op(int p, int q) {
  int op_pos = -1;
  int min_priority = INT_MAX;
  int paren_level = 0;

  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      paren_level++;
      continue;
    } else if (tokens[i].type == ')') {
      paren_level--;
      continue;
    } else if (paren_level == 0 && is_operator(tokens[i].type)) {
      if (tokens[i].type == '-' && (i == p || is_operator(tokens[i - 1].type) || tokens[op_pos - 1].type == '(')) {
        // 跳过连续的负号
        while (i + 1 <= q && tokens[i + 1].type == '-' && is_operator(tokens[i].type)) {
          i++;
        }
        continue;
      }
      
      int priority = get_priority(tokens[i].type);
      if (priority < min_priority) {
        min_priority = priority;
        op_pos = i;
      }
    } 
  }

  return op_pos;
}

// 检查表达式是否被一对匹配的括号包围
static bool check_parentheses(int p, int q) {
  if (tokens[p].type != '(' || tokens[q].type != ')') {
    return false;
  }

  int paren_level = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      paren_level++;
    } else if (tokens[i].type == ')') {
      paren_level--;
      if (paren_level == 0 && i != q) {
        return false;
      }
    }
  }

  return true;
}

// 语法分析
static double eval(int p, int q, bool *success){
  if(p > q) {
    *success = false;
    return 0;
  }
  else if(p == q) {
    *success = true;
    switch (tokens[p].type){
      case TK_DEC:
      case TK_HEX:
        return strtod(tokens[p].str, NULL);
      case TK_REG:{
        if(strcmp(&tokens[p].str[1], "eip") == 0) {
          return cpu.eip;
        }
        for(int i = 0; i < 8; i++) {
          if(strcmp(&tokens[p].str[1], reg_name(i,4)) == 0) {
            return reg_l(i);
          }
        }
        *success = false;
        return 0;
      }
      default:
        *success = false;
        return 0;
      }
  }
  else if(check_parentheses(p, q)) {
    return eval(p + 1, q - 1, success);
  }
  else {
    int op_pos = find_dominant_op(p, q);

    printf("expr: op_pos = %d\n", op_pos);

    if (op_pos == -1) {
      *success = false;
      return 0;
    }

    bool left_success, right_success;
    double val1 = eval(p, op_pos - 1, &left_success);
    double val2 = eval(op_pos + 1, q, &right_success);
    
    printf("expr: val1 = %f, success = %d\n", val1, left_success);
    printf("expr: val2 = %f, success = %d\n", val2, right_success);
    
    if (tokens[op_pos].type == TK_NOT) {
      if (right_success) {
        *success = true;
        return !val2;
      } else {
        *success = false;
        return 0;
      }
    }
    if (tokens[op_pos].type == TK_DEREF) {
      if (right_success) {
        *success = true;
        return vaddr_read(val2, 4);
      } else {
        *success = false;
        return 0;
      }
    }

    if (!left_success || !right_success) {
      *success = false;
      return 0;
    } 
    *success = true;
    switch (tokens[op_pos].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': {
        if (val2 == 0) {
          *success = false;
          return 0;
        }
        return val1 / val2;
      }
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_OR: return val1 || val2;
      default:
        *success = false;
        return 0;
    }
  }
}

double expr(char *e, bool *success) {
  memset(tokens, 0, sizeof(tokens));

  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i = 0; i < nr_token; i++) {
    // 如果 * 是表达式的第一个 token 或者 * 前面是一个运算符，则 * 是解引用运算符
    if(tokens[i].type == '*' && (i == 0 || is_operator(tokens[i - 1].type))) {
      tokens[i].type = TK_DEREF;
    }
  }

  return eval(0, nr_token - 1, success);
}
