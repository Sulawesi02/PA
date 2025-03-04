#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Single step execution", cmd_si },
  { "info", "Print program status", cmd_info },
  { "x", "Scan memory", cmd_x },
  { "p", "Evaluate expression", cmd_p },
  { "w", "Set watchpoint", cmd_w },
  { "d", "Delete watchpoint", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

// 单步执行
static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  int steps = 1;  // 默认执行1步
  
  if (arg != NULL) {
    steps = atoi(arg);
    if (steps <= 0) {
      printf("请输入有效的步数\n");
      return 0;
    }
  }
  cpu_exec(steps);
  return 0;
}

// 打印程序状态
static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  
  if (arg == NULL) {
    printf("请输入参数\n");
    return 0;
  }
  
  if (strcmp(arg, "r") == 0) {
    for (int i = 0; i < 8; i++) {
      printf("%s:0x%x\n", reg_name(i,4), reg_l(i));
    }
  } else if (strcmp(arg,"w") == 0){
    print_watchpoints();// 打印所有监视点信息
  } else{
    printf("请输入有效的参数\n");
  }

  return 0;
}

// 扫描内存
static int cmd_x(char *args) {
  char *n = strtok(NULL, " ");
  char *expr = strtok(NULL, " ");
  
  if (n == NULL || expr == NULL) {
    printf("使用格式: x N EXPR\n");
    return 0;
  }
  
  int len = atoi(n);
  if (len <= 0) {
    printf("请输入有效的内存长度\n");
    return 0;
  }
  
  // 暂定 EXPR 中只能是一个十六进制数
  vaddr_t addr = strtoul(expr, NULL, 16);
  
  for (int i = 0; i < len; i++) {
    printf("0x%08x: ", addr + i * 4);
    // 读取4字节内存
    uint32_t data = vaddr_read(addr + i * 4, 4);
    printf("0x%08x\n", data);
  }
  
  return 0;
}

// 表达式求值
static int cmd_p(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("请输入表达式\n");
    return 0;
  }

  bool success = false;
  uint32_t value = expr(arg, &success);

  // printf("expr: value = %u, success = %d\n", value, success);

  if (success) {
    printf("表达式的值: %u\n", value);
  } else {
    printf("表达式求值失败\n");
  }
  
  return 0;
}

// 设置监视点
static int cmd_w(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("请输入表达式\n");
    return 0;
  }
  printf("表达式: %s\n", arg);
  return 0;
}

// 删除监视点
static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("请输入表达式\n");
    return 0;
  }
  printf("表达式: %s\n", arg);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
