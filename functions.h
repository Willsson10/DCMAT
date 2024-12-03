#ifndef __FUNCS_H__
#define __FUNCS_H_

#include <stdio.h>
#include "ast.h"

void plot_func(AST_NODE* func);
void set_func_atual(AST_NODE* func);
AST_NODE* get_func_atual();
void free_func_atual();
void show_settings();
void reset_settings();
void set_h_view(double lo, double hi);
void set_v_view(double lo, double hi);
void set_float_precision (int valor);
int get_float_precision ();
void set_axis(int state) ;
void set_integral_steps(int valor);
int get_erase_plot();
void set_erase_plot(int state);
void store_func(AST_NODE* func);
void free_all_func();
double integrate(double lower, double upper, AST_NODE* func) ;
double sum_func(const char* variable, int lower, int upper, AST_NODE* expr);

AST_NODE* get_last_node ();

void about ();


typedef struct functions {
    AST_NODE* func;
    struct functions* next;

}Functions;

#endif