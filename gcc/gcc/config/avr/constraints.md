;; Constraint definitions for ATMEL AVR micro controllers.
;; Copyright (C) 2006, 2007 Free Software Foundation, Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; Register constraints

(define_register_constraint "t" "R0_REG"
  "Temporary register r0")

(define_register_constraint "b" "BASE_POINTER_REGS"
  "Base pointer registers (r28--r31)")

(define_register_constraint "e" "POINTER_REGS"
  "Pointer registers (r26--r31)")

(define_register_constraint "w" "ADDW_REGS"
  "Registers from r24 to r31.  These registers
   can be used in @samp{adiw} command.")

(define_register_constraint "d" "LD_REGS"
  "Registers from r16 to r31.")

(define_register_constraint "l" "NO_LD_REGS"
  "Registers from r0 to r15.")

(define_register_constraint "a" "SIMPLE_LD_REGS"
  "Registers from r16 to r23.")

(define_register_constraint "x" "POINTER_X_REGS"
  "Register pair X (r27:r26).")

(define_register_constraint "y" "POINTER_Y_REGS"
  "Register pair Y (r29:r28).")

(define_register_constraint "z" "POINTER_Z_REGS"
  "Register pair Z (r31:r30).")

(define_register_constraint "q" "STACK_REG"
  "Stack pointer register (SPH:SPL).")

(define_constraint "I"
  "Integer constant in the range 0 @dots{} 63."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 63")))

(define_constraint "J"
  "Integer constant in the range -63 @dots{} 0."
  (and (match_code "const_int")
       (match_test "ival <= 0 && ival >= -63")))

(define_constraint "K"
  "Integer constant 2."
  (and (match_code "const_int")
       (match_test "ival == 2")))

(define_constraint "L"
  "Zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))

(define_constraint "M"
  "Integer constant in the range 0 @dots{} 0xff."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 0xff")))

(define_constraint "N"
  "Constant integer @minus{}1."
  (and (match_code "const_int")
       (match_test "ival == -1")))

(define_constraint "O"
  "Constant integer 8, 16, or 24."
  (and (match_code "const_int")
       (match_test "ival == 8 || ival == 16 || ival == 24")))

(define_constraint "P"
  "Constant integer 1."
  (and (match_code "const_int")
       (match_test "ival == 1")))

(define_constraint "G"
  "Constant float 0."
  (and (match_code "const_double")
       (match_test "op == CONST0_RTX (SFmode)")))

(define_constraint "R"
  "Integer constant in the range -6 @dots{} 5."
  (and (match_code "const_int")
       (match_test "ival >= -6 && ival <= 5")))
       
(define_memory_constraint "Q"
  "A memory address based on Y or Z pointer with displacement."
  (and (match_code "mem")
       (match_test "extra_constraint_Q (op)")))

(define_constraint "C03"
  "Constant integer 3."
  (and (match_code "const_int")
       (match_test "ival == 3")))

(define_constraint "C04"
  "Constant integer 4."
  (and (match_code "const_int")
       (match_test "ival == 4")))

(define_constraint "C05"
  "Constant integer 5."
  (and (match_code "const_int")
       (match_test "ival == 5")))

(define_constraint "C06"
  "Constant integer 6."
  (and (match_code "const_int")
       (match_test "ival == 6")))

(define_constraint "C07"
  "Constant integer 7."
  (and (match_code "const_int")
       (match_test "ival == 7")))

(define_constraint "Ca2"
  "Constant 2-byte integer that allows AND without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 2, (1<<0) | (1<<7) | (1<<8))")))

(define_constraint "Ca4"
  "Constant 4-byte integer that allows AND without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 4, (1<<0) | (1<<7) | (1<<8))")))

(define_constraint "Co2"
  "Constant 2-byte integer that allows OR without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 2, (1<<0) | (1<<1) | (1<<8))")))

(define_constraint "Co4"
  "Constant 4-byte integer that allows OR without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 4, (1<<0) | (1<<1) | (1<<8))")))

(define_constraint "Cx2"
  "Constant 2-byte integer that allows XOR without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 2, (1<<0) | (1<<8))")))

(define_constraint "Cx4"
  "Constant 4-byte integer that allows XOR without clobber register."
  (and (match_code "const_int")
       (match_test "avr_popcount_each_byte (op, 4, (1<<0) | (1<<8))")))
