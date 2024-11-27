#include "robloxInstructions.h"
#include "robloxOpcodes.h"

extern "C" 
{
#include "lopcodes.h"
}

unsigned int RBX::getRobloxOpcode(unsigned int i)
{
    switch (i)
    {
    case OP_MOVE: return ROP_MOVE;
    case OP_LOADK: return ROP_LOADK;
    case OP_LOADBOOL: return ROP_LOADBOOL;
    case OP_LOADNIL: return ROP_LOADNIL;
    case OP_GETUPVAL: return ROP_GETUPVAL;
    case OP_GETGLOBAL: return ROP_GETGLOBAL;
    case OP_GETTABLE: return ROP_GETTABLE;
    case OP_SETGLOBAL: return ROP_SETGLOBAL;
    case OP_SETUPVAL: return ROP_SETUPVAL;
    case OP_SETTABLE: return ROP_SETTABLE;
    case OP_NEWTABLE: return ROP_NEWTABLE;
    case OP_SELF: return ROP_SELF;
    case OP_ADD: return ROP_ADD;
    case OP_SUB: return ROP_SUB;
    case OP_MUL: return ROP_MUL;
    case OP_DIV: return ROP_DIV;
    case OP_MOD: return ROP_MOD;
    case OP_POW: return ROP_POW;
    case OP_UNM: return ROP_UNM;
    case OP_NOT: return ROP_NOT;
    case OP_LEN: return ROP_LEN;
    case OP_CONCAT: return ROP_CONCAT;
    case OP_JMP: return ROP_JMP;
    case OP_EQ: return ROP_EQ;
    case OP_LT: return ROP_LT;
    case OP_LE: return ROP_LE;
    case OP_TEST: return ROP_TEST;
    case OP_TESTSET: return ROP_TESTSET;
    case OP_CALL: return ROP_CALL;
    case OP_TAILCALL: return ROP_TAILCALL;
    case OP_RETURN: return ROP_RETURN;
    case OP_FORLOOP: return ROP_FORLOOP;
    case OP_FORPREP: return ROP_FORPREP;
    case OP_TFORLOOP: return ROP_TFORLOOP;
    case OP_SETLIST: return ROP_SETLIST;
    case OP_CLOSE: return ROP_CLOSE;
    case OP_CLOSURE: return ROP_CLOSURE;
    case OP_VARARG: return ROP_VARARG;
    default: return ROP_RETURN;
    }
}

unsigned int RBX::setArgs(unsigned int i)
{
    int op = GET_OPCODE(i);
    unsigned int re = 0;

    switch (getOpMode(op))
    {
    case OpMode::iABC:
    {
        RSETARG_A(re, GETARG_A(i));
        RSETARG_B(re, GETARG_B(i));
        RSETARG_C(re, GETARG_C(i));
        break;
    }
    case OpMode::iABx:
    {
        RSETARG_A(re, GETARG_A(i));
        RSETARG_Bx(re, GETARG_Bx(i));
        break;
    }
    case OpMode::iAsBx:
    {
        RSETARG_A(re, GETARG_A(i));
        RSETARG_sBx(re, GETARG_sBx(i));
        break;
    }
    }

    return re;
}