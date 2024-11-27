
/* NOTE: generally.. opcodes in revivals dont change.. but a clever revival owner can easily just change all of these things up! and BOOM! no more execution.. */

/*
** $Id: lopcodes.h,v 1.125.1.1 2007/12/27 13:02:25 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef r_opcodes_h
#define r_opcodes_h

#if !defined(INLINE_OP) && defined(WIN32)
#define INLINE_OP __forceinline
#elif !defined(INLINE_OP)
#define INLINE_OP inline __attribute__((always_inline))
#endif

#include "llimits.h"


/*===========================================================================
  We assume that instructions are unsigned numbers.
  All instructions have an opcode in the first 6 bits.
  Instructions can have the following fields:
    `A' : 8 bits
    `B' : 9 bits
    `C' : 9 bits
    `Bx' : 18 bits (`B' and `C' together)
    `sBx' : signed Bx

  A signed argument is represented in excess K; that is, the number
  value is the unsigned value minus K. K is exactly the maximum value
  for that argument (so that -max is represented by 0, and +max is
  represented by 2*max), which is half the maximum for the corresponding
  unsigned argument.
===========================================================================*/


enum rOpMode { riABC, riABx, riAsBx };  /* basic instruction format */
#define LUAVM_SECURE

typedef unsigned int (*RbxOpEncoder)(unsigned int i, int pc, unsigned key);

#define LUAVM_SHUFFLE_COMMA ,

#ifdef LUAVM_SECURE
#define LUAVM_SHUFFLE2(sep,a0,a1) a1 sep a0
#define LUAVM_SHUFFLE3(sep,a0,a1,a2) a1 sep a2 sep a0
#define LUAVM_SHUFFLE4(sep,a0,a1,a2,a3) a3 sep a1 sep a0 sep a2
#define LUAVM_SHUFFLE5(sep,a0,a1,a2,a3,a4) a4 sep a0 sep a2 sep a1 sep a3
#define LUAVM_SHUFFLE6(sep,a0,a1,a2,a3,a4,a5) a3 sep a5 sep a2 sep a0 sep a1 sep a4
#define LUAVM_SHUFFLE7(sep,a0,a1,a2,a3,a4,a5,a6) a2 sep a3 sep a0 sep a4 sep a6 sep a1 sep a5
#define LUAVM_SHUFFLE8(sep,a0,a1,a2,a3,a4,a5,a6,a7) a7 sep a0 sep a5 sep a6 sep a3 sep a1 sep a2 sep a4
#define LUAVM_SHUFFLE9(sep,a0,a1,a2,a3,a4,a5,a6,a7,a8) a2 sep a6 sep a4 sep a7 sep a1 sep a8 sep a0 sep a3 sep a5
#else
#define LUAVM_SHUFFLE2(sep,a0,a1) a0 sep a1
#define LUAVM_SHUFFLE3(sep,a0,a1,a2) a0 sep a1 sep a2
#define LUAVM_SHUFFLE4(sep,a0,a1,a2,a3) a0 sep a1 sep a2 sep a3
#define LUAVM_SHUFFLE5(sep,a0,a1,a2,a3,a4) a0 sep a1 sep a2 sep a3 sep a4
#define LUAVM_SHUFFLE6(sep,a0,a1,a2,a3,a4,a5) a0 sep a1 sep a2 sep a3 sep a4 sep a5
#define LUAVM_SHUFFLE7(sep,a0,a1,a2,a3,a4,a5,a6) a0 sep a1 sep a2 sep a3 sep a4 sep a5 sep a6
#define LUAVM_SHUFFLE8(sep,a0,a1,a2,a3,a4,a5,a6,a7) a0 sep a1 sep a2 sep a3 sep a4 sep a5 sep a6 sep a7
#define LUAVM_SHUFFLE9(sep,a0,a1,a2,a3,a4,a5,a6,a7,a8) a0 sep a1 sep a2 sep a3 sep a4 sep a5 sep a6 sep a7 sep a8
#endif

#if defined(LUAVM_SECURE)
#define LUAVM_ENCODELINE(line, pc) ((line) ^ ((pc) << 8))
#define LUAVM_DECODELINE(line, pc) ((line) ^ ((pc) << 8))
#else
#define LUAVM_ENCODELINE(line, pc) (line)
#define LUAVM_DECODELINE(line, pc) (line)
#endif

// Encoding/decoding instructions
#if defined(LUAVM_SECURE)
#define LUAVM_ENCODEINSN(insn, key) ((insn) * key)
#define LUAVM_DECODEINSN(insn, key) ((insn).v * key)
#else
#define LUAVM_ENCODEINSN(insn, key) (insn)
#define LUAVM_DECODEINSN(insn, key) (insn).v
#endif


/*
** size and position of opcode arguments.
*/
#define RSIZE_C		9
#define RSIZE_B		9
#define RSIZE_Bx		(RSIZE_C + RSIZE_B)
#define RSIZE_A		8

#define RSIZE_OP		6

#ifdef LUAVM_SECURE
#define RPOS_OP		(RPOS_A + RSIZE_A)
#define RPOS_A	    (RPOS_C + RSIZE_C)
#define RPOS_C		(RPOS_B + RSIZE_B)
#define RPOS_B		0
#define RPOS_Bx		0
#else
#define POS_OP		0
#define POS_A		(POS_OP + SIZE_OP)
#define POS_C		(POS_A + SIZE_A)
#define POS_B		(POS_C + SIZE_C)
#define POS_Bx		POS_C
#endif

/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in LUAI_BITSINT-1 bits (-1 for sign)
*/
#if RSIZE_Bx < LUAI_BITSINT-1
#define RMAXARG_Bx        ((1<<RSIZE_Bx)-1)
#define RMAXARG_sBx        (RMAXARG_Bx>>1)         /* `sBx' is signed */
#else
#define RMAXARG_Bx        MAX_INT
#define RMAXARG_sBx        MAX_INT
#endif


#define RMAXARG_A        ((1<<RSIZE_A)-1)
#define RMAXARG_B        ((1<<RSIZE_B)-1)
#define RMAXARG_C        ((1<<RSIZE_C)-1)


/* creates a mask with `n' 1 bits at position `p' */
#define RMASK1(n,p)	((~((~(Instruction)0)<<n))<<p)

/* creates a mask with `n' 0 bits at position `p' */
#define RMASK0(n,p)	(~RMASK1(n,p))

/*
** the following macros help to manipulate instructions
*/

#define RGET_OPCODE(i)	(cast(ROpCode, ((i)>>RPOS_OP) & RMASK1(RSIZE_OP,0)))
#define RSET_OPCODE(i,o)	((i) = (((i)&RMASK0(RSIZE_OP,RPOS_OP)) | \
		((cast(Instruction, o)<<RPOS_OP)&RMASK1(RSIZE_OP,RPOS_OP))))

#define RGETARG_A(i)	(cast(int, ((i)>>RPOS_A) & RMASK1(RSIZE_A,0)))
#define RSETARG_A(i,u)	((i) = (((i)&RMASK0(RSIZE_A,RPOS_A)) | \
		((cast(Instruction, u)<<RPOS_A)&RMASK1(RSIZE_A,RPOS_A))))

#define RGETARG_B(i)	(cast(int, ((i)>>RPOS_B) & RMASK1(RSIZE_B,0)))
#define RSETARG_B(i,b)	((i) = (((i)&RMASK0(RSIZE_B,RPOS_B)) | \
		((cast(Instruction, b)<<RPOS_B)&RMASK1(RSIZE_B,RPOS_B))))

#define RGETARG_C(i)	(cast(int, ((i)>>RPOS_C) & RMASK1(RSIZE_C,0)))
#define RSETARG_C(i,b)	((i) = (((i)&RMASK0(RSIZE_C,RPOS_C)) | \
		((cast(Instruction, b)<<RPOS_C)&RMASK1(RSIZE_C,RPOS_C))))

#define RGETARG_Bx(i)	(cast(int, ((i)>>RPOS_Bx) & RMASK1(RSIZE_Bx,0)))
#define RSETARG_Bx(i,b)	((i) = (((i)&RMASK0(RSIZE_Bx,RPOS_Bx)) | \
		((cast(Instruction, b)<<RPOS_Bx)&RMASK1(RSIZE_Bx,RPOS_Bx))))

#define RGETARG_sBx(i)	(RGETARG_Bx(i)-RMAXARG_sBx)
#define RSETARG_sBx(i,b)	RSETARG_Bx((i),cast(unsigned int, (b)+RMAXARG_sBx))


#define RCREATE_ABC(o,a,b,c)	((cast(Instruction, o)<<RPOS_OP) \
			| (cast(Instruction, a)<<RPOS_A) \
			| (cast(Instruction, b)<<RPOS_B) \
			| (cast(Instruction, c)<<RPOS_C))

#define RCREATE_ABx(o,a,bc)	((cast(Instruction, o)<<RPOS_OP) \
			| (cast(Instruction, a)<<RPOS_A) \
			| (cast(Instruction, bc)<<RPOS_Bx))


/*
** Macros to operate RK indices
*/

/* this bit 1 means constant (0 means register) */
#define RBITRK		(1 << (RSIZE_B - 1))

/* test whether value is a constant */
#define RISK(x)		((x) & RBITRK)

/* gets the index of the constant */
#define RINDEXK(r)	((int)(r) & ~RBITRK)

#define RMAXINDEXRK	(RBITRK - 1)

/* code a constant index as a RK value */
#define RRKASK(x)	((x) | RBITRK)


/*
** invalid register that fits in 8 bits
*/
#define RNO_REG		RMAXARG_A


/*
** R(x) - register
** Kst(x) - constant (in constant table)
** RK(x) == if ISK(x) then Kst(INDEXK(x)) else R(x)
*/


/*
** grep "ORDER OP" if you change these enums
*/

/* NOTE: someday in the near future... these will change! and i wont be able to skid robloxs' src and just use their cool lil shuffle methods */

typedef enum {
    /*----------------------------------------------------------------------
    name		args	description
    ------------------------------------------------------------------------*/
    LUAVM_SHUFFLE9(LUAVM_SHUFFLE_COMMA,
    ROP_MOVE,/*	A B	R(A) := R(B)					*/
    ROP_LOADK,/*	A Bx	R(A) := Kst(Bx)					*/
    ROP_LOADBOOL,/*	A B C	R(A) := (Bool)B; if (C) pc++			*/
    ROP_LOADNIL,/*	A B	R(A) := ... := R(B) := nil			*/
    ROP_GETUPVAL,/*	A B	R(A) := UpValue[B]				*/

    ROP_GETGLOBAL,/*	A Bx	R(A) := Gbl[Kst(Bx)]				*/
    ROP_GETTABLE,/*	A B C	R(A) := R(B)[RK(C)]				*/

    ROP_SETGLOBAL,/*	A Bx	Gbl[Kst(Bx)] := R(A)				*/
    ROP_SETUPVAL),/*	A B	UpValue[B] := R(A)				*/

    LUAVM_SHUFFLE9(LUAVM_SHUFFLE_COMMA,
    ROP_SETTABLE,/*	A B C	R(A)[RK(B)] := RK(C)				*/

    ROP_NEWTABLE,/*	A B C	R(A) := {} (size = B,C)				*/

    ROP_SELF,/*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/

    ROP_ADD,/*	A B C	R(A) := RK(B) + RK(C)				*/
    ROP_SUB,/*	A B C	R(A) := RK(B) - RK(C)				*/
    ROP_MUL,/*	A B C	R(A) := RK(B) * RK(C)				*/
    ROP_DIV,/*	A B C	R(A) := RK(B) / RK(C)				*/
    ROP_MOD,/*	A B C	R(A) := RK(B) % RK(C)				*/
    ROP_POW),/*	A B C	R(A) := RK(B) ^ RK(C)				*/

    LUAVM_SHUFFLE9(LUAVM_SHUFFLE_COMMA,
    ROP_UNM,/*	A B	R(A) := -R(B)					*/
    ROP_NOT,/*	A B	R(A) := not R(B)				*/
    ROP_LEN,/*	A B	R(A) := length of R(B)				*/

    ROP_CONCAT,/*	A B C	R(A) := R(B).. ... ..R(C)			*/

    ROP_JMP,/*	sBx	pc+=sBx					*/

    ROP_EQ,/*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
    ROP_LT,/*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++  		*/
    ROP_LE,/*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++  		*/
    ROP_TEST),/*	A C	if not (R(A) <=> C) then pc++			*/

    LUAVM_SHUFFLE9(LUAVM_SHUFFLE_COMMA,
    ROP_TESTSET,/*	A B C	if (R(B) <=> C) then R(A) := R(B) else pc++	*/

    ROP_CALL,/*	A B C	R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
    ROP_TAILCALL,/*	A B C	return R(A)(R(A+1), ... ,R(A+B-1))		*/
    ROP_RETURN,/*	A B	return R(A), ... ,R(A+B-2)	(see note)	*/

    ROP_FORLOOP,/*	A sBx	R(A)+=R(A+2);
                if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
                ROP_FORPREP,/*	A sBx	R(A)-=R(A+2); pc+=sBx				*/

                ROP_TFORLOOP,/*	A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));
                                        if R(A+3) ~= nil then R(A+2)=R(A+3) else pc++	*/
                                        ROP_SETLIST,/*	A B C	R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	*/
                                        ROP_CLOSE),/*	A 	close all variables in the stack up to (>=) R(A)*/

                                        LUAVM_SHUFFLE2(LUAVM_SHUFFLE_COMMA,
                                        ROP_CLOSURE,/*	A Bx	R(A) := closure(KPROTO[Bx], R(A), ... ,R(A+n))	*/

                                        ROP_VARARG),/*	A B	R(A), R(A+1), ..., R(A+B-1) = vararg		*/

                                        ROP___COUNT
} ROpCode;


#define RNUM_OPCODES	(cast(int, ROP___COUNT))



/*===========================================================================
  Notes:
  (*) In OP_CALL, if (B == 0) then B = top. C is the number of returns - 1,
      and can be 0: OP_CALL then sets `top' to last_result+1, so
      next open instruction (OP_CALL, OP_RETURN, OP_SETLIST) may use `top'.

  (*) In OP_VARARG, if (B == 0) then use actual number of varargs and
      set top (like in OP_CALL with C == 0).

  (*) In OP_RETURN, if (B == 0) then return up to `top'

  (*) In OP_SETLIST, if (B == 0) then B = `top';
      if (C == 0) then next `instruction' is real C

  (*) For comparisons, A specifies what condition the test should accept
      (true or false).

  (*) All `skips' (pc++) assume that next instruction is a jump
===========================================================================*/


/*
** masks for instruction properties. The format is:
** bits 0-1: op mode
** bits 2-3: C arg mode
** bits 4-5: B arg mode
** bit 6: instruction set register A
** bit 7: operator is a test
*/

enum rOpArgMask {
    ROpArgN,  /* argument is not used */
    ROpArgU,  /* argument is used */
    ROpArgR,  /* argument is a register or a jump offset */
    ROpArgK   /* argument is a constant or register/constant */
};

LUAI_DATA const lu_byte r_luaP_opmodes[RNUM_OPCODES];

#define rgetOpMode(m)	(cast(enum rOpMode, r_luaP_opmodes[m] & 3))
#define rgetBMode(m)	(cast(enum rOpArgMask, (r_luaP_opmodes[m] >> 4) & 3))
#define rgetCMode(m)	(cast(enum rOpArgMask, (r_luaP_opmodes[m] >> 2) & 3))
#define rtestAMode(m)	(r_luaP_opmodes[m] & (1 << 6))
#define rtestTMode(m)	(r_luaP_opmodes[m] & (1 << 7))


LUAI_DATA const char* const r_luaP_opnames[RNUM_OPCODES + 1];  /* opcode names */


/* number of list items to accumulate before a SETLIST instruction */
#define RLFIELDS_PER_FLUSH	50

/* for the new security layer that was added */
#define LUAVM_DAX_MO 0x29451AFB
#define LUAVM_DAX_ME 0x72394BC8
#define LUAVM_DAX_AO 0x46582A8B
#define LUAVM_DAX_AE 0x62A0B4E3

#endif