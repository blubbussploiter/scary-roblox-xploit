
#include <cstdint>
#include "robloxInstructions.h"

extern "C"
{
#include "lopcodes.h"
}

namespace RBX
{
    namespace OpcodeEncoding
    {
        static uint32_t rbxDaxEncodeOp(uint32_t x, uint32_t mulEven, uint32_t addEven, uint32_t mulOdd, uint32_t addOdd)
        {
            uint32_t result = 0;
            uint32_t mask = 1;
            for (size_t i = 0; i < 8 * sizeof(uint32_t); ++i)
            {
                uint32_t bitDesired = mask & x;
                uint32_t bitOdd = mask & (result * mulOdd + addOdd);
                uint32_t bitEven = mask & (result * mulEven + addEven);
                if ((bitEven ^ bitOdd) != bitDesired)
                {
                    result |= mask;
                }
                mask <<= 1;
            }
            return result;
        }

        static unsigned int rbxDaxEncode(unsigned int i, int pc, unsigned int key)
        {
            Instruction enc = i;
            Instruction op = RGET_OPCODE(i);

            switch (op) {
            case ROP_CALL:
            case ROP_TAILCALL:
            case ROP_RETURN:
            case ROP_CLOSURE:
                enc = rbxDaxEncodeOp(i, LUAVM_DAX_ME, pc, LUAVM_DAX_MO, LUAVM_DAX_AO);
                RSET_OPCODE(enc, op);
                break;
            case ROP_MOVE:
                RSETARG_C(enc, (pc | 1)); // non-zero
                break;
            default:
                break;
            }
            return LUAVM_ENCODEINSN(enc, key);
        }
    }

    namespace Encoding
    {
        static unsigned int encode(unsigned int i, int pc, unsigned key)
        {
            unsigned int rop = getRobloxOpcode(GET_OPCODE(i));
            unsigned int r = 0;
            unsigned int ri;

            r = setArgs(i);
            RSET_OPCODE(r, rop);
            ri = OpcodeEncoding::rbxDaxEncode(r, pc, key);

            return ri;
        }


        static unsigned int rawdog(unsigned int i, int pc, unsigned key)
        {
            unsigned int rop = getRobloxOpcode(GET_OPCODE(i));
            unsigned int r = 0;

            r = setArgs(i);
            RSET_OPCODE(r, rop);

            return r;
        }
    }
}