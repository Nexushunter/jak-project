#pragma once

/*!
 * @file Instruction.h
 * An EE instruction, represented as an operation, plus a list of source/destination atoms.
 * Can print itself (within the context of a LinkedObjectFile).
 */

#ifndef NEXT_INSTRUCTION_H
#define NEXT_INSTRUCTION_H

#include "OpcodeInfo.h"
#include "Register.h"

class LinkedObjectFile;

constexpr int MAX_INSTRUCTION_SOURCE = 3;
constexpr int MAX_INTRUCTION_DEST = 1;

// An "atom", representing a single register, immediate, etc... for use in an Instruction.
struct InstructionAtom {
  enum AtomKind {
    REGISTER,  // An EE Register
    IMM,       // An immediate value (stored as int32)
    IMM_SYM,   // An immediate value (a symbolic link)
    LABEL,     // A label in a LinkedObjectFile
    VU_ACC,    // The VU0 Accumulator
    VU_Q,      // The VU0 Q Register
    INVALID
  } kind = INVALID;

  void set_reg(Register r);
  void set_imm(int32_t i);
  void set_label(int id);
  void set_vu_q();
  void set_vu_acc();
  void set_sym(std::string _sym);

  Register get_reg() const;
  int32_t get_imm() const;
  int get_label() const;
  std::string get_sym() const;

  std::string to_string(const LinkedObjectFile& file) const;

  bool is_link_or_label() const;

 private:
  int32_t imm;
  int label_id;
  Register reg;

  std::string sym;
};

// An "Instruction", consisting of a "kind" (the opcode), and the source/destination atoms it
// operates on.
class Instruction {
 public:
  InstructionKind kind = InstructionKind::UNKNOWN;

  std::string to_string(const LinkedObjectFile& file) const;
  bool is_valid() const;

  void add_src(InstructionAtom& a);
  void add_dst(InstructionAtom& a);

  InstructionAtom& get_src(size_t idx);
  InstructionAtom& get_dst(size_t idx);
  const InstructionAtom& get_src(size_t idx) const;
  const InstructionAtom& get_dst(size_t idx) const;

  // source and destination atoms
  uint8_t n_src = 0, n_dst = 0;
  InstructionAtom src[MAX_INSTRUCTION_SOURCE];
  InstructionAtom dst[MAX_INTRUCTION_DEST];

  InstructionAtom& get_imm_src();
  int32_t get_imm_src_int();

  const OpcodeInfo& get_info() const;

  int get_label_target() const;

  // extra fields for some COP2 instructions.
  uint8_t cop2_dest = 0xff;  // 0xff indicates "don't print dest"
  uint8_t cop2_bc = 0xff;    // 0xff indicates "don't print bc"
  uint8_t il = 0xff;         // 0xff indicates "don't print il"
};

#endif  // NEXT_INSTRUCTION_H
