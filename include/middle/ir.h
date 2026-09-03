#include "middle/semantic_analyzer.h"
#include "common/arena.h"
#include "front/parser.h"



typedef enum {
    IR_CONST,
    IR_MOVE,

    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,

    IR_CMP_EQ,
    IR_CMP_NE,

    IR_CMP_LT,
    IR_CMP_GT,

    IR_CMP_LE,
    IR_CMP_GE,

    IR_BRANCH,
    IR_JUMP,

    IR_LABEL,

    IR_PRINT
} IROpCode;



typedef enum {
    IR_OPERAND_CONST,
    IR_OPERAND_VAR,
    IR_OPERAND_TEMP
} IROperandType;


typedef struct {
    IROperandType type;
     union {
        int value;
        Symbol* symbol;
        int temp;
     };
} IROperand;


typedef struct {
    IROpCode op_code;
    IROperand dst;
    IROperand src_1;
    IROperand src_2;

    unsigned label;
} IRInstruction;



typedef struct {
    IRInstruction *data;
    size_t size;
    size_t capacity;
} IRInstructionVec;


typedef struct {
    Arena* arena;
    IRInstructionVec instructions_vector;

    int next_temp;
    int next_label;
    
    ASTNode* ast;

    SemanticContext* ctx;
} IRContext;


void ir_print(IRContext* irc);
void ir_context_init(IRContext* irc, Arena* a, ASTNode* ast, SemanticContext* ctx);
void ir_vec_free(IRInstructionVec *vec);
void ir_generate(IRContext* irc);