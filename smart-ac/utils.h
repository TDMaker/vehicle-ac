#ifndef __UTILS_H__
#define __UTILS_H__
#include <math.h>
#include <string.h>
#include <time.h>
#include <pbc/pbc.h>
#include "rd_hashmap.h"

#define CAPACITY 1024
typedef struct
{
    int *data;
    int length;
} rdvec;

typedef struct
{
    int **elem;
    int rows;
    int cols;
} rdmat;

typedef struct
{
    float *elem;
    int rows;
    int cols;
} rdmat_f;
typedef struct
{
    int rows;
    int cols;
    element_t *elem;
} rdmat_mp;

typedef struct
{
    void **elem_;
    int length;
    int capacity;
} ptr_list;

typedef enum
{
    LABEL_ADD = 10,
    LABEL_DELETE = 20,
    LABEL_MULTIPLY = 30,
    LABEL_NOP = 0,
} Label;

typedef enum
{
    STATE_NOT_EXISTS = 0,
    STATE_START = 1,
    STATE_DELETE = 2,
    STATE_ADD = 3,
    STATE_MULTIPLY = 4,
    STATE_ADD_ = 5,
    STATE_MULTIPLY_ = 6,
    STATE_REPLACE = 7,
    STATE_UNKNOWN = 8,
} State;

rdvec make_rdvec();
rdvec cpy_rdvec(rdvec a);
rdmat make_rdmat(int rows, int cols);
rdmat_f make_rdmat_f(int rows, int cols);
rdmat_mp make_rdmat_mp(int rows, int cols);
rdmat_f rdmat_f_mul(rdmat_f a, rdmat_f b);
element_t **rdmat_mul_sp_mp(rdmat, rdmat_mp);
rdmat pick_rows(int count, rdmat a, int *rows);
rdmat get_arged_mat(rdmat a);
rdmat_f gaussian_elimination(rdmat a);
rdmat transpose(rdmat a);
void rdmat_print(const char *name, rdmat a);
void rdmat_f_print(const char *name, rdmat_f a);
void rdmat_mp_print(const char *name, rdmat_mp a);
void free_rdvec(rdvec a);
void free_rdmat(rdmat a);
void free_rdmat_f(rdmat_f a);
void free_rdmat_mp(rdmat_mp a);
ptr_list make_ptr_list(int capacity);
State transition(State state, Label label);
void print_state(const char *name, State state);
ptr_list add_to_list(ptr_list a, void *b);
ptr_list shrink_list(ptr_list a);
bool is_connector(const char *value);
bool is_or(const char *value);
bool is_and(const char *value);
#endif /* __UTILS_H__ */