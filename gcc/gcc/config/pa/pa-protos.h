/* Prototypes for pa.c functions used in the md file & elsewhere.
   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2010, 2011
   Free Software Foundation,
   Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifdef RTX_CODE
/* Prototype function used in various macros.  */
extern rtx pa_eh_return_handler_rtx (void);

/* Used in insn-*.c.  */
extern int following_call (rtx);

/* Define functions in pa.c and used in insn-output.c.  */

extern const char *output_and (rtx *);
extern const char *output_ior (rtx *);
extern const char *output_move_double (rtx *);
extern const char *output_fp_move_double (rtx *);
extern const char *output_block_move (rtx *, int);
extern const char *output_block_clear (rtx *, int);
extern const char *output_cbranch (rtx *, int, rtx);
extern const char *output_lbranch (rtx, rtx, int);
extern const char *output_bb (rtx *, int, rtx, int);
extern const char *output_bvb (rtx *, int, rtx, int);
extern const char *output_dbra (rtx *, rtx, int);
extern const char *output_movb (rtx *, rtx, int, int);
extern const char *output_parallel_movb (rtx *, rtx);
extern const char *output_parallel_addb (rtx *, rtx);
extern const char *output_call (rtx, rtx, int);
extern const char *output_indirect_call (rtx, rtx);
extern const char *output_millicode_call (rtx, rtx);
extern const char *output_mul_insn (int, rtx);
extern const char *output_div_insn (rtx *, int, rtx);
extern const char *output_mod_insn (int, rtx);
extern const char *singlemove_string (rtx *);
extern void output_arg_descriptor (rtx);
extern void output_global_address (FILE *, rtx, int);
extern void print_operand (FILE *, rtx, int);
extern rtx legitimize_pic_address (rtx, enum machine_mode, rtx);
extern void hppa_encode_label (rtx);
extern int symbolic_expression_p (rtx);
extern bool pa_tls_referenced_p (rtx);
extern int pa_adjust_insn_length (rtx, int);
extern int fmpyaddoperands (rtx *);
extern int fmpysuboperands (rtx *);
extern void emit_bcond_fp (rtx[]);
extern int emit_move_sequence (rtx *, enum machine_mode, rtx);
extern int emit_hpdiv_const (rtx *, int);
extern int is_function_label_plus_const (rtx);
extern int jump_in_call_delay (rtx);
extern int hppa_fpstore_bypass_p (rtx, rtx);
extern int attr_length_millicode_call (rtx);
extern int attr_length_call (rtx, int);
extern int attr_length_indirect_call (rtx);
extern int attr_length_save_restore_dltp (rtx);

/* Declare functions defined in pa.c and used in templates.  */

extern rtx return_addr_rtx (int, rtx);

#ifdef ARGS_SIZE_RTX
/* expr.h defines ARGS_SIZE_RTX and `enum direction' */
#ifdef TREE_CODE
extern enum direction function_arg_padding (enum machine_mode, const_tree);
#endif
#endif /* ARGS_SIZE_RTX */
extern int insn_refs_are_delayed (rtx);
extern rtx get_deferred_plabel (rtx);
#endif /* RTX_CODE */

extern int ldil_cint_p (HOST_WIDE_INT);
extern int zdepi_cint_p (unsigned HOST_WIDE_INT);

extern void output_ascii (FILE *, const char *, int);
extern HOST_WIDE_INT compute_frame_size (HOST_WIDE_INT, int *);
extern int and_mask_p (unsigned HOST_WIDE_INT);
extern int cint_ok_for_move (HOST_WIDE_INT);
extern void hppa_expand_prologue (void);
extern void hppa_expand_epilogue (void);
extern bool pa_can_use_return_insn (void);
extern int ior_mask_p (unsigned HOST_WIDE_INT);
extern void compute_zdepdi_operands (unsigned HOST_WIDE_INT,
				     unsigned *);
#ifdef RTX_CODE
extern const char * output_64bit_and (rtx *);
extern const char * output_64bit_ior (rtx *);
extern int cmpib_comparison_operator (rtx, enum machine_mode);
#endif


/* Miscellaneous functions in pa.c.  */
#ifdef TREE_CODE
extern int reloc_needed (tree);
extern bool pa_return_in_memory (const_tree, const_tree);
#endif /* TREE_CODE */

extern void pa_asm_output_aligned_bss (FILE *, const char *,
				       unsigned HOST_WIDE_INT,
				       unsigned int);
extern void pa_asm_output_aligned_common (FILE *, const char *,
					  unsigned HOST_WIDE_INT,
					  unsigned int);
extern void pa_asm_output_aligned_local (FILE *, const char *,
					 unsigned HOST_WIDE_INT,
					 unsigned int);
extern void pa_hpux_asm_output_external (FILE *, tree, const char *);
extern bool pa_cannot_change_mode_class (enum machine_mode, enum machine_mode,
					 enum reg_class);
extern bool pa_modes_tieable_p (enum machine_mode, enum machine_mode);
extern HOST_WIDE_INT pa_initial_elimination_offset (int, int);

extern const int magic_milli[];
extern int shadd_constant_p (int);
