#include "registerbasedcpu.h"
#include "cpu.h"
#include "memoryunit.h"

namespace FluffyGamevev::CPU8
{
    // +---+---+---+---+---+---+---+---+---------------+---------------+
    // | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | Extra Byte    | Interpreter   |
    // +---+---+---+---+---+---+---+---+---------------+---------------+
    // | 00 : Mov      |  dst  |  src  |               | Mov           |
    // | 01 : Mov []   |  dst  |  src  |               | Mov           |
    // | 02 : Add      |  lhs  |  rhs  |               | BinaryOp      |
    // | 03 : Sub      |  lhs  |  rhs  |               | BinaryOp      |
    // | 04 : Mul      |  lhs  |  rhs  |               | BinaryOp      |
    // | 05 : Div      |  lhs  |  rhs  |               | BinaryOp      |
    // | 06 : Mod      |  lhs  |  rhs  |               | BinaryOp      |
    // | 07 : And      |  lhs  |  rhs  |               | BinaryOp      |
    // | 08 : Or       |  lhs  |  rhs  |               | BinaryOp      |
    // | 09 : Xor      |  lhs  |  rhs  |               | BinaryOp      |
    // | 10 : Cmp      |  lhs  |  rhs  |               | BinaryOp      |
    // | 11 : Unary Op | 0 Add |  rhs  | rhs           | BinaryOp      |
    // | 11 : Unary Op | 1 Sub |  rhs  | rhs           | BinaryOp      |
    // | 11 : Unary Op | 2 Mul |  rhs  | rhs           | BinaryOp      |
    // | 11 : Unary Op | 3 Div |  rhs  | rhs           | BinaryOp      |
    // | 12 : Unary Op | 0 Mod |  rhs  | rhs           | BinaryOp      |
    // | 12 : Unary Op | 1 And |  rhs  | rhs           | BinaryOp      |
    // | 12 : Unary Op | 2 Or  |  rhs  | rhs           | BinaryOp      |
    // | 12 : Unary Op | 3 Xor |  rhs  | rhs           | BinaryOp      |
    // | 13 : Unary Op | 0 Not |  dst  |               | UnraryOp      |
    // | 13 : Unary Op | 1 Inc |  dst  |               | UnraryOp      |
    // | 13 : Unary Op | 2 Dec |  dst  |               | UnraryOp      |
    // | 13 : Unary Op | 3 Mov |  dst  | value         | Mov           |
    // | 14 : Unary Op | 0Mov[]|  dst  | value         | Mov           |
    // | 14 : Unary Op | 1 Push|  src  |               | UnraryOp      |
    // | 14 : Unary Op | 2 Pop |  dst  |               | UnraryOp      |
    // | 14 : Unary Op | 3 Load|  dst  | addr          | BinaryOp      |
    // | 15 : Unary Op | 0 Cmp |  lhs  | rhs           | BinaryOp      |
    // | 15 : Unary Op | 1 ??? |       |               |               |
    // | 15 : No Args  | 2     | 0 Nop |               | No Arg        |
    // | 15 : No Args  | 2     | 1 Dat | value         | UnraryOp      |
    // | 15 : No Args  | 2     | 2 Jmp | dst           | UnraryOp      |
    // | 15 : No Args  | 2     | 3 Je  | dst           | UnraryOp      |
    // | 15 : No Args  | 3     | 0 Jg  | dst           | UnraryOp      |
    // | 15 : No Args  | 3     | 1 Jl  | dst           | UnraryOp      |
    // | 15 : No Args  | 3     | 2 ??? |               |               |
    // | 15 : No Args  | 3     | 3 ??? |               |               |
    // +---------------+-------+-------+---------------+---------------+

    enum class Register
    {
        A,
        B,
        C,
        SP,
        Flags,

        Count
    };

    enum class Flag
    {
        Equal = 0x01,
        Greater = 0X02
    };

    namespace Instruction
    {
        template <Register LhsOffset, Register RhsOffset>
        static void Mov(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs = rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void MovPtr(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            ram.Buffer[lhs] = rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Add(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs += rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Sub(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs -= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Mul(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs *= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Div(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs /= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Mod(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs %= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void And(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs &= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Or(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs |= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Xor(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            lhs ^= rhs;
        }

        template <Register LhsOffset, Register RhsOffset>
        static void Cmp(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ cpu.Registers[(size_t)RhsOffset] };
            u8& flags{ cpu.Registers[(size_t)Register::Flags] };

            flags = (lhs == rhs ? (u8)Flag::Equal : 0) |
                    (lhs > rhs ? (u8)Flag::Greater : 0);
        }

        template <Register LhsOffset>
        static void AddUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs += rhs;
        }

        template <Register LhsOffset>
        static void SubUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs -= rhs;
        }

        template <Register LhsOffset>
        static void MulUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs *= rhs;
        }

        template <Register LhsOffset>
        static void DivUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs /= rhs;
        }

        template <Register LhsOffset>
        static void ModUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs %= rhs;
        }

        template <Register LhsOffset>
        static void AndUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs &= rhs;
        }

        template <Register LhsOffset>
        static void OrUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs |= rhs;
        }

        template <Register LhsOffset>
        static void XorUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs ^= rhs;
        }

        template <Register LhsOffset>
        static void CmpUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            u8& flags{ cpu.Registers[(size_t)Register::Flags] };
            ++cpu.InstructionPointer;

            flags = (lhs == rhs ? (u8)Flag::Equal : 0) |
                    (lhs > rhs ? (u8)Flag::Greater : 0);
        }

        template <Register LhsOffset>
        static void MovUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs = rhs;
        }

        template <Register LhsOffset>
        static void MovPtrUnary(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            ram.Buffer[lhs] = rhs;
        }

        template <Register LhsOffset>
        static void Not(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            lhs = ~lhs;
        }

        template <Register LhsOffset>
        static void Inc(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            ++lhs;
        }

        template <Register LhsOffset>
        static void Dec(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            --lhs;
        }

        template <Register LhsOffset>
        static void Push(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8& stackPointer{ cpu.Registers[(int)Register::SP] };
            ram.Buffer[stackPointer] = lhs;
            ++stackPointer;
        }

        template <Register LhsOffset>
        static void Pop(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8& stackPointer{ cpu.Registers[(int)Register::SP] };
            --stackPointer;
            lhs = ram.Buffer[stackPointer];
        }

        template <Register LhsOffset>
        static void Load(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8& lhs{ cpu.Registers[(size_t)LhsOffset] };
            u8 rhs{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            lhs = ram.Buffer[rhs];
        }

        static void Jmp(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8 destination{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;
            cpu.InstructionPointer = destination;
        }

        static void Je(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8 destination{ rom.Buffer[cpu.InstructionPointer] };
            u8 flags{ cpu.Registers[(size_t)Register::Flags] };
            ++cpu.InstructionPointer;

            if ((flags & (u8)Flag::Equal) != 0)
            {
                cpu.InstructionPointer = destination;
            }
        }

        static void Jg(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8 destination{ rom.Buffer[cpu.InstructionPointer] };
            u8 flags{ cpu.Registers[(size_t)Register::Flags] };
            ++cpu.InstructionPointer;

            if ((flags & (u8)Flag::Greater) != 0)
            {
                cpu.InstructionPointer = destination;
            }
        }

        static void Jl(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8 destination{ rom.Buffer[cpu.InstructionPointer] };
            u8 flags{ cpu.Registers[(size_t)Register::Flags] };
            ++cpu.InstructionPointer;

            if ((flags & ((u8)Flag::Equal | (u8)Flag::Greater)) == 0)
            {
                cpu.InstructionPointer = destination;
            }
        }

        static void Dat(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            u8 data{ rom.Buffer[cpu.InstructionPointer] };
            ++cpu.InstructionPointer;

            u8& stackPointer{ cpu.Registers[(int)Register::SP] };
            ram.Buffer[stackPointer] = data;
            ++stackPointer;
        }

        static void Nop(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
        }

        static void InvalidOperation(CPU& cpu, const MemoryUnit& rom, MemoryUnit& ram)
        {
            *((unsigned int*)0) = 0xDEAD;
        }
    }


    void InitCPU(CPU& cpu)
    {
        cpu.Registers.resize((size_t)Register::Count);

        cpu.Instructions[0x00] = Instruction::Mov<Register::A, Register::A>;
        cpu.Instructions[0x01] = Instruction::Mov<Register::A, Register::B>;
        cpu.Instructions[0x02] = Instruction::Mov<Register::A, Register::C>;
        cpu.Instructions[0x03] = Instruction::Mov<Register::A, Register::SP>;
        cpu.Instructions[0x04] = Instruction::Mov<Register::B, Register::A>;
        cpu.Instructions[0x05] = Instruction::Mov<Register::B, Register::B>;
        cpu.Instructions[0x06] = Instruction::Mov<Register::B, Register::C>;
        cpu.Instructions[0x07] = Instruction::Mov<Register::B, Register::SP>;
        cpu.Instructions[0x08] = Instruction::Mov<Register::C, Register::A>;
        cpu.Instructions[0x09] = Instruction::Mov<Register::C, Register::B>;
        cpu.Instructions[0x0a] = Instruction::Mov<Register::C, Register::C>;
        cpu.Instructions[0x0b] = Instruction::Mov<Register::C, Register::SP>;
        cpu.Instructions[0x0c] = Instruction::Mov<Register::SP, Register::A>;
        cpu.Instructions[0x0d] = Instruction::Mov<Register::SP, Register::B>;
        cpu.Instructions[0x0e] = Instruction::Mov<Register::SP, Register::C>;
        cpu.Instructions[0x0f] = Instruction::Mov<Register::SP, Register::SP>;

        cpu.Instructions[0x10] = Instruction::MovPtr<Register::A, Register::A>;
        cpu.Instructions[0x11] = Instruction::MovPtr<Register::A, Register::B>;
        cpu.Instructions[0x12] = Instruction::MovPtr<Register::A, Register::C>;
        cpu.Instructions[0x13] = Instruction::MovPtr<Register::A, Register::SP>;
        cpu.Instructions[0x14] = Instruction::MovPtr<Register::B, Register::A>;
        cpu.Instructions[0x15] = Instruction::MovPtr<Register::B, Register::B>;
        cpu.Instructions[0x16] = Instruction::MovPtr<Register::B, Register::C>;
        cpu.Instructions[0x17] = Instruction::MovPtr<Register::B, Register::SP>;
        cpu.Instructions[0x18] = Instruction::MovPtr<Register::C, Register::A>;
        cpu.Instructions[0x19] = Instruction::MovPtr<Register::C, Register::B>;
        cpu.Instructions[0x1a] = Instruction::MovPtr<Register::C, Register::C>;
        cpu.Instructions[0x1b] = Instruction::MovPtr<Register::C, Register::SP>;
        cpu.Instructions[0x1c] = Instruction::MovPtr<Register::SP, Register::A>;
        cpu.Instructions[0x1d] = Instruction::MovPtr<Register::SP, Register::B>;
        cpu.Instructions[0x1e] = Instruction::MovPtr<Register::SP, Register::C>;
        cpu.Instructions[0x1f] = Instruction::MovPtr<Register::SP, Register::SP>;

        cpu.Instructions[0x20] = Instruction::Add<Register::A, Register::A>;
        cpu.Instructions[0x21] = Instruction::Add<Register::A, Register::B>;
        cpu.Instructions[0x22] = Instruction::Add<Register::A, Register::C>;
        cpu.Instructions[0x23] = Instruction::Add<Register::A, Register::SP>;
        cpu.Instructions[0x24] = Instruction::Add<Register::B, Register::A>;
        cpu.Instructions[0x25] = Instruction::Add<Register::B, Register::B>;
        cpu.Instructions[0x26] = Instruction::Add<Register::B, Register::C>;
        cpu.Instructions[0x27] = Instruction::Add<Register::B, Register::SP>;
        cpu.Instructions[0x28] = Instruction::Add<Register::C, Register::A>;
        cpu.Instructions[0x29] = Instruction::Add<Register::C, Register::B>;
        cpu.Instructions[0x2a] = Instruction::Add<Register::C, Register::C>;
        cpu.Instructions[0x2b] = Instruction::Add<Register::C, Register::SP>;
        cpu.Instructions[0x2c] = Instruction::Add<Register::SP, Register::A>;
        cpu.Instructions[0x2d] = Instruction::Add<Register::SP, Register::B>;
        cpu.Instructions[0x2e] = Instruction::Add<Register::SP, Register::C>;
        cpu.Instructions[0x2f] = Instruction::Add<Register::SP, Register::SP>;

        cpu.Instructions[0x30] = Instruction::Sub<Register::A, Register::A>;
        cpu.Instructions[0x31] = Instruction::Sub<Register::A, Register::B>;
        cpu.Instructions[0x32] = Instruction::Sub<Register::A, Register::C>;
        cpu.Instructions[0x33] = Instruction::Sub<Register::A, Register::SP>;
        cpu.Instructions[0x34] = Instruction::Sub<Register::B, Register::A>;
        cpu.Instructions[0x35] = Instruction::Sub<Register::B, Register::B>;
        cpu.Instructions[0x36] = Instruction::Sub<Register::B, Register::C>;
        cpu.Instructions[0x37] = Instruction::Sub<Register::B, Register::SP>;
        cpu.Instructions[0x38] = Instruction::Sub<Register::C, Register::A>;
        cpu.Instructions[0x39] = Instruction::Sub<Register::C, Register::B>;
        cpu.Instructions[0x3a] = Instruction::Sub<Register::C, Register::C>;
        cpu.Instructions[0x3b] = Instruction::Sub<Register::C, Register::SP>;
        cpu.Instructions[0x3c] = Instruction::Sub<Register::SP, Register::A>;
        cpu.Instructions[0x3d] = Instruction::Sub<Register::SP, Register::B>;
        cpu.Instructions[0x3e] = Instruction::Sub<Register::SP, Register::C>;
        cpu.Instructions[0x3f] = Instruction::Sub<Register::SP, Register::SP>;

        cpu.Instructions[0x40] = Instruction::Mul<Register::A, Register::A>;
        cpu.Instructions[0x41] = Instruction::Mul<Register::A, Register::B>;
        cpu.Instructions[0x42] = Instruction::Mul<Register::A, Register::C>;
        cpu.Instructions[0x43] = Instruction::Mul<Register::A, Register::SP>;
        cpu.Instructions[0x44] = Instruction::Mul<Register::B, Register::A>;
        cpu.Instructions[0x45] = Instruction::Mul<Register::B, Register::B>;
        cpu.Instructions[0x46] = Instruction::Mul<Register::B, Register::C>;
        cpu.Instructions[0x47] = Instruction::Mul<Register::B, Register::SP>;
        cpu.Instructions[0x48] = Instruction::Mul<Register::C, Register::A>;
        cpu.Instructions[0x49] = Instruction::Mul<Register::C, Register::B>;
        cpu.Instructions[0x4a] = Instruction::Mul<Register::C, Register::C>;
        cpu.Instructions[0x4b] = Instruction::Mul<Register::C, Register::SP>;
        cpu.Instructions[0x4c] = Instruction::Mul<Register::SP, Register::A>;
        cpu.Instructions[0x4d] = Instruction::Mul<Register::SP, Register::B>;
        cpu.Instructions[0x4e] = Instruction::Mul<Register::SP, Register::C>;
        cpu.Instructions[0x4f] = Instruction::Mul<Register::SP, Register::SP>;

        cpu.Instructions[0x50] = Instruction::Div<Register::A, Register::A>;
        cpu.Instructions[0x51] = Instruction::Div<Register::A, Register::B>;
        cpu.Instructions[0x52] = Instruction::Div<Register::A, Register::C>;
        cpu.Instructions[0x53] = Instruction::Div<Register::A, Register::SP>;
        cpu.Instructions[0x54] = Instruction::Div<Register::B, Register::A>;
        cpu.Instructions[0x55] = Instruction::Div<Register::B, Register::B>;
        cpu.Instructions[0x56] = Instruction::Div<Register::B, Register::C>;
        cpu.Instructions[0x57] = Instruction::Div<Register::B, Register::SP>;
        cpu.Instructions[0x58] = Instruction::Div<Register::C, Register::A>;
        cpu.Instructions[0x59] = Instruction::Div<Register::C, Register::B>;
        cpu.Instructions[0x5a] = Instruction::Div<Register::C, Register::C>;
        cpu.Instructions[0x5b] = Instruction::Div<Register::C, Register::SP>;
        cpu.Instructions[0x5c] = Instruction::Div<Register::SP, Register::A>;
        cpu.Instructions[0x5d] = Instruction::Div<Register::SP, Register::B>;
        cpu.Instructions[0x5e] = Instruction::Div<Register::SP, Register::C>;
        cpu.Instructions[0x5f] = Instruction::Div<Register::SP, Register::SP>;

        cpu.Instructions[0x60] = Instruction::Mod<Register::A, Register::A>;
        cpu.Instructions[0x61] = Instruction::Mod<Register::A, Register::B>;
        cpu.Instructions[0x62] = Instruction::Mod<Register::A, Register::C>;
        cpu.Instructions[0x63] = Instruction::Mod<Register::A, Register::SP>;
        cpu.Instructions[0x64] = Instruction::Mod<Register::B, Register::A>;
        cpu.Instructions[0x65] = Instruction::Mod<Register::B, Register::B>;
        cpu.Instructions[0x66] = Instruction::Mod<Register::B, Register::C>;
        cpu.Instructions[0x67] = Instruction::Mod<Register::B, Register::SP>;
        cpu.Instructions[0x68] = Instruction::Mod<Register::C, Register::A>;
        cpu.Instructions[0x69] = Instruction::Mod<Register::C, Register::B>;
        cpu.Instructions[0x6a] = Instruction::Mod<Register::C, Register::C>;
        cpu.Instructions[0x6b] = Instruction::Mod<Register::C, Register::SP>;
        cpu.Instructions[0x6c] = Instruction::Mod<Register::SP, Register::A>;
        cpu.Instructions[0x6d] = Instruction::Mod<Register::SP, Register::B>;
        cpu.Instructions[0x6e] = Instruction::Mod<Register::SP, Register::C>;
        cpu.Instructions[0x6f] = Instruction::Mod<Register::SP, Register::SP>;

        cpu.Instructions[0x70] = Instruction::And<Register::A, Register::A>;
        cpu.Instructions[0x71] = Instruction::And<Register::A, Register::B>;
        cpu.Instructions[0x72] = Instruction::And<Register::A, Register::C>;
        cpu.Instructions[0x73] = Instruction::And<Register::A, Register::SP>;
        cpu.Instructions[0x74] = Instruction::And<Register::B, Register::A>;
        cpu.Instructions[0x75] = Instruction::And<Register::B, Register::B>;
        cpu.Instructions[0x76] = Instruction::And<Register::B, Register::C>;
        cpu.Instructions[0x77] = Instruction::And<Register::B, Register::SP>;
        cpu.Instructions[0x78] = Instruction::And<Register::C, Register::A>;
        cpu.Instructions[0x79] = Instruction::And<Register::C, Register::B>;
        cpu.Instructions[0x7a] = Instruction::And<Register::C, Register::C>;
        cpu.Instructions[0x7b] = Instruction::And<Register::C, Register::SP>;
        cpu.Instructions[0x7c] = Instruction::And<Register::SP, Register::A>;
        cpu.Instructions[0x7d] = Instruction::And<Register::SP, Register::B>;
        cpu.Instructions[0x7e] = Instruction::And<Register::SP, Register::C>;
        cpu.Instructions[0x7f] = Instruction::And<Register::SP, Register::SP>;

        cpu.Instructions[0x80] = Instruction::Or<Register::A, Register::A>;
        cpu.Instructions[0x81] = Instruction::Or<Register::A, Register::B>;
        cpu.Instructions[0x82] = Instruction::Or<Register::A, Register::C>;
        cpu.Instructions[0x83] = Instruction::Or<Register::A, Register::SP>;
        cpu.Instructions[0x84] = Instruction::Or<Register::B, Register::A>;
        cpu.Instructions[0x85] = Instruction::Or<Register::B, Register::B>;
        cpu.Instructions[0x86] = Instruction::Or<Register::B, Register::C>;
        cpu.Instructions[0x87] = Instruction::Or<Register::B, Register::SP>;
        cpu.Instructions[0x88] = Instruction::Or<Register::C, Register::A>;
        cpu.Instructions[0x89] = Instruction::Or<Register::C, Register::B>;
        cpu.Instructions[0x8a] = Instruction::Or<Register::C, Register::C>;
        cpu.Instructions[0x8b] = Instruction::Or<Register::C, Register::SP>;
        cpu.Instructions[0x8c] = Instruction::Or<Register::SP, Register::A>;
        cpu.Instructions[0x8d] = Instruction::Or<Register::SP, Register::B>;
        cpu.Instructions[0x8e] = Instruction::Or<Register::SP, Register::C>;
        cpu.Instructions[0x8f] = Instruction::Or<Register::SP, Register::SP>;

        cpu.Instructions[0x90] = Instruction::Xor<Register::A, Register::A>;
        cpu.Instructions[0x91] = Instruction::Xor<Register::A, Register::B>;
        cpu.Instructions[0x92] = Instruction::Xor<Register::A, Register::C>;
        cpu.Instructions[0x93] = Instruction::Xor<Register::A, Register::SP>;
        cpu.Instructions[0x94] = Instruction::Xor<Register::B, Register::A>;
        cpu.Instructions[0x95] = Instruction::Xor<Register::B, Register::B>;
        cpu.Instructions[0x96] = Instruction::Xor<Register::B, Register::C>;
        cpu.Instructions[0x97] = Instruction::Xor<Register::B, Register::SP>;
        cpu.Instructions[0x98] = Instruction::Xor<Register::C, Register::A>;
        cpu.Instructions[0x99] = Instruction::Xor<Register::C, Register::B>;
        cpu.Instructions[0x9a] = Instruction::Xor<Register::C, Register::C>;
        cpu.Instructions[0x9b] = Instruction::Xor<Register::C, Register::SP>;
        cpu.Instructions[0x9c] = Instruction::Xor<Register::SP, Register::A>;
        cpu.Instructions[0x9d] = Instruction::Xor<Register::SP, Register::B>;
        cpu.Instructions[0x9e] = Instruction::Xor<Register::SP, Register::C>;
        cpu.Instructions[0x9f] = Instruction::Xor<Register::SP, Register::SP>;

        cpu.Instructions[0xa0] = Instruction::Cmp<Register::A, Register::A>;
        cpu.Instructions[0xa1] = Instruction::Cmp<Register::A, Register::B>;
        cpu.Instructions[0xa2] = Instruction::Cmp<Register::A, Register::C>;
        cpu.Instructions[0xa3] = Instruction::Cmp<Register::A, Register::SP>;
        cpu.Instructions[0xa4] = Instruction::Cmp<Register::B, Register::A>;
        cpu.Instructions[0xa5] = Instruction::Cmp<Register::B, Register::B>;
        cpu.Instructions[0xa6] = Instruction::Cmp<Register::B, Register::C>;
        cpu.Instructions[0xa7] = Instruction::Cmp<Register::B, Register::SP>;
        cpu.Instructions[0xa8] = Instruction::Cmp<Register::C, Register::A>;
        cpu.Instructions[0xa9] = Instruction::Cmp<Register::C, Register::B>;
        cpu.Instructions[0xaa] = Instruction::Cmp<Register::C, Register::C>;
        cpu.Instructions[0xab] = Instruction::Cmp<Register::C, Register::SP>;
        cpu.Instructions[0xac] = Instruction::Cmp<Register::SP, Register::A>;
        cpu.Instructions[0xad] = Instruction::Cmp<Register::SP, Register::B>;
        cpu.Instructions[0xae] = Instruction::Cmp<Register::SP, Register::C>;
        cpu.Instructions[0xaf] = Instruction::Cmp<Register::SP, Register::SP>;

        cpu.Instructions[0xb0] = Instruction::AddUnary<Register::A>;
        cpu.Instructions[0xb1] = Instruction::AddUnary<Register::B>;
        cpu.Instructions[0xb2] = Instruction::AddUnary<Register::C>;
        cpu.Instructions[0xb3] = Instruction::AddUnary<Register::SP>;
        cpu.Instructions[0xb4] = Instruction::SubUnary<Register::A>;
        cpu.Instructions[0xb5] = Instruction::SubUnary<Register::B>;
        cpu.Instructions[0xb6] = Instruction::SubUnary<Register::C>;
        cpu.Instructions[0xb7] = Instruction::SubUnary<Register::SP>;
        cpu.Instructions[0xb8] = Instruction::MulUnary<Register::A>;
        cpu.Instructions[0xb9] = Instruction::MulUnary<Register::B>;
        cpu.Instructions[0xba] = Instruction::MulUnary<Register::C>;
        cpu.Instructions[0xbb] = Instruction::MulUnary<Register::SP>;
        cpu.Instructions[0xbc] = Instruction::DivUnary<Register::A>;
        cpu.Instructions[0xbd] = Instruction::DivUnary<Register::B>;
        cpu.Instructions[0xbe] = Instruction::DivUnary<Register::C>;
        cpu.Instructions[0xbf] = Instruction::DivUnary<Register::SP>;

        cpu.Instructions[0xc0] = Instruction::ModUnary<Register::A>;
        cpu.Instructions[0xc1] = Instruction::ModUnary<Register::B>;
        cpu.Instructions[0xc2] = Instruction::ModUnary<Register::C>;
        cpu.Instructions[0xc3] = Instruction::ModUnary<Register::SP>;
        cpu.Instructions[0xc4] = Instruction::AndUnary<Register::A>;
        cpu.Instructions[0xc5] = Instruction::AndUnary<Register::B>;
        cpu.Instructions[0xc6] = Instruction::AndUnary<Register::C>;
        cpu.Instructions[0xc7] = Instruction::AndUnary<Register::SP>;
        cpu.Instructions[0xc8] = Instruction::OrUnary<Register::A>;
        cpu.Instructions[0xc9] = Instruction::OrUnary<Register::B>;
        cpu.Instructions[0xca] = Instruction::OrUnary<Register::C>;
        cpu.Instructions[0xcb] = Instruction::OrUnary<Register::SP>;
        cpu.Instructions[0xcc] = Instruction::XorUnary<Register::A>;
        cpu.Instructions[0xcd] = Instruction::XorUnary<Register::B>;
        cpu.Instructions[0xce] = Instruction::XorUnary<Register::C>;
        cpu.Instructions[0xcf] = Instruction::XorUnary<Register::SP>;

        cpu.Instructions[0xd0] = Instruction::Not<Register::A>;
        cpu.Instructions[0xd1] = Instruction::Not<Register::B>;
        cpu.Instructions[0xd2] = Instruction::Not<Register::C>;
        cpu.Instructions[0xd3] = Instruction::Not<Register::SP>;
        cpu.Instructions[0xd4] = Instruction::Inc<Register::A>;
        cpu.Instructions[0xd5] = Instruction::Inc<Register::B>;
        cpu.Instructions[0xd6] = Instruction::Inc<Register::C>;
        cpu.Instructions[0xd7] = Instruction::Inc<Register::SP>;
        cpu.Instructions[0xd8] = Instruction::Dec<Register::A>;
        cpu.Instructions[0xd9] = Instruction::Dec<Register::B>;
        cpu.Instructions[0xda] = Instruction::Dec<Register::C>;
        cpu.Instructions[0xdb] = Instruction::Dec<Register::SP>;
        cpu.Instructions[0xdc] = Instruction::MovUnary<Register::A>;
        cpu.Instructions[0xdd] = Instruction::MovUnary<Register::B>;
        cpu.Instructions[0xde] = Instruction::MovUnary<Register::C>;
        cpu.Instructions[0xdf] = Instruction::MovUnary<Register::SP>;

        cpu.Instructions[0xe0] = Instruction::MovPtrUnary<Register::A>;
        cpu.Instructions[0xe1] = Instruction::MovPtrUnary<Register::B>;
        cpu.Instructions[0xe2] = Instruction::MovPtrUnary<Register::C>;
        cpu.Instructions[0xe3] = Instruction::MovPtrUnary<Register::SP>;
        cpu.Instructions[0xe4] = Instruction::Push<Register::A>;
        cpu.Instructions[0xe5] = Instruction::Push<Register::B>;
        cpu.Instructions[0xe6] = Instruction::Push<Register::C>;
        cpu.Instructions[0xe7] = Instruction::Push<Register::SP>;
        cpu.Instructions[0xe8] = Instruction::Pop<Register::A>;
        cpu.Instructions[0xe9] = Instruction::Pop<Register::B>;
        cpu.Instructions[0xea] = Instruction::Pop<Register::C>;
        cpu.Instructions[0xeb] = Instruction::Pop<Register::SP>;
        cpu.Instructions[0xec] = Instruction::Load<Register::A>;
        cpu.Instructions[0xed] = Instruction::Load<Register::B>;
        cpu.Instructions[0xee] = Instruction::Load<Register::C>;
        cpu.Instructions[0xef] = Instruction::Load<Register::SP>;

        cpu.Instructions[0xf0] = Instruction::CmpUnary<Register::A>;
        cpu.Instructions[0xf1] = Instruction::CmpUnary<Register::B>;
        cpu.Instructions[0xf2] = Instruction::CmpUnary<Register::C>;
        cpu.Instructions[0xf3] = Instruction::CmpUnary<Register::SP>;
        cpu.Instructions[0xf4] = Instruction::InvalidOperation;
        cpu.Instructions[0xf5] = Instruction::InvalidOperation;
        cpu.Instructions[0xf6] = Instruction::InvalidOperation;
        cpu.Instructions[0xf7] = Instruction::InvalidOperation;
        cpu.Instructions[0xf8] = Instruction::Nop;
        cpu.Instructions[0xf9] = Instruction::Dat;
        cpu.Instructions[0xfa] = Instruction::Jmp;
        cpu.Instructions[0xfb] = Instruction::Je;
        cpu.Instructions[0xfc] = Instruction::Jg;
        cpu.Instructions[0xfd] = Instruction::Jl;
        cpu.Instructions[0xfe] = Instruction::InvalidOperation;
        cpu.Instructions[0xff] = Instruction::InvalidOperation;
    }
}