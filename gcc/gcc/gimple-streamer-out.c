/* Routines for emitting GIMPLE to a file stream.

   Copyright 2011 Free Software Foundation, Inc.
   Contributed by Diego Novillo <dnovillo@google.com>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "tree-flow.h"
#include "data-streamer.h"
#include "gimple-streamer.h"
#include "lto-streamer.h"
#include "tree-streamer.h"

/* Output PHI function PHI to the main stream in OB.  */

static void
output_phi (struct output_block *ob, gimple phi)
{
  unsigned i, len = gimple_phi_num_args (phi);

  streamer_write_record_start (ob, lto_gimple_code_to_tag (GIMPLE_PHI));
  streamer_write_uhwi (ob, SSA_NAME_VERSION (PHI_RESULT (phi)));

  for (i = 0; i < len; i++)
    {
      stream_write_tree (ob, gimple_phi_arg_def (phi, i), true);
      streamer_write_uhwi (ob, gimple_phi_arg_edge (phi, i)->src->index);
      lto_output_location (ob, gimple_phi_arg_location (phi, i));
    }
}


/* Emit statement STMT on the main stream of output block OB.  */

static void
output_gimple_stmt (struct output_block *ob, gimple stmt)
{
  unsigned i;
  enum gimple_code code;
  enum LTO_tags tag;
  struct bitpack_d bp;

  /* Emit identifying tag.  */
  code = gimple_code (stmt);
  tag = lto_gimple_code_to_tag (code);
  streamer_write_record_start (ob, tag);

  /* Emit the tuple header.  */
  bp = bitpack_create (ob->main_stream);
  bp_pack_var_len_unsigned (&bp, gimple_num_ops (stmt));
  bp_pack_value (&bp, gimple_no_warning_p (stmt), 1);
  if (is_gimple_assign (stmt))
    bp_pack_value (&bp, gimple_assign_nontemporal_move_p (stmt), 1);
  bp_pack_value (&bp, gimple_has_volatile_ops (stmt), 1);
  bp_pack_var_len_unsigned (&bp, stmt->gsbase.subcode);
  streamer_write_bitpack (&bp);

  /* Emit location information for the statement.  */
  lto_output_location (ob, gimple_location (stmt));

  /* Emit the lexical block holding STMT.  */
  stream_write_tree (ob, gimple_block (stmt), true);

  /* Emit the operands.  */
  switch (gimple_code (stmt))
    {
    case GIMPLE_RESX:
      streamer_write_hwi (ob, gimple_resx_region (stmt));
      break;

    case GIMPLE_EH_MUST_NOT_THROW:
      stream_write_tree (ob, gimple_eh_must_not_throw_fndecl (stmt), true);
      break;

    case GIMPLE_EH_DISPATCH:
      streamer_write_hwi (ob, gimple_eh_dispatch_region (stmt));
      break;

    case GIMPLE_ASM:
      streamer_write_uhwi (ob, gimple_asm_ninputs (stmt));
      streamer_write_uhwi (ob, gimple_asm_noutputs (stmt));
      streamer_write_uhwi (ob, gimple_asm_nclobbers (stmt));
      streamer_write_uhwi (ob, gimple_asm_nlabels (stmt));
      streamer_write_string (ob, ob->main_stream, gimple_asm_string (stmt),
			     true);
      /* Fallthru  */

    case GIMPLE_ASSIGN:
    case GIMPLE_CALL:
    case GIMPLE_RETURN:
    case GIMPLE_SWITCH:
    case GIMPLE_LABEL:
    case GIMPLE_COND:
    case GIMPLE_GOTO:
    case GIMPLE_DEBUG:
      for (i = 0; i < gimple_num_ops (stmt); i++)
	{
	  tree op = gimple_op (stmt, i);
	  /* Wrap all uses of non-automatic variables inside MEM_REFs
	     so that we do not have to deal with type mismatches on
	     merged symbols during IL read in.  The first operand
	     of GIMPLE_DEBUG must be a decl, not MEM_REF, though.  */
	  if (op && (i || !is_gimple_debug (stmt)))
	    {
	      tree *basep = &op;
	      while (handled_component_p (*basep))
		basep = &TREE_OPERAND (*basep, 0);
	      if (TREE_CODE (*basep) == VAR_DECL
		  && !auto_var_in_fn_p (*basep, current_function_decl)
		  && !DECL_REGISTER (*basep))
		{
		  bool volatilep = TREE_THIS_VOLATILE (*basep);
		  *basep = build2 (MEM_REF, TREE_TYPE (*basep),
				   build_fold_addr_expr (*basep),
				   build_int_cst (build_pointer_type
						  (TREE_TYPE (*basep)), 0));
		  TREE_THIS_VOLATILE (*basep) = volatilep;
		}
	    }
	  stream_write_tree (ob, op, true);
	}
      if (is_gimple_call (stmt))
	{
	  if (gimple_call_internal_p (stmt))
	    streamer_write_enum (ob->main_stream, internal_fn,
				 IFN_LAST, gimple_call_internal_fn (stmt));
	  else
	    stream_write_tree (ob, gimple_call_fntype (stmt), true);
	}
      break;

    case GIMPLE_NOP:
    case GIMPLE_PREDICT:
      break;

    default:
      gcc_unreachable ();
    }
}


/* Output a basic block BB to the main stream in OB for this FN.  */

void
output_bb (struct output_block *ob, basic_block bb, struct function *fn)
{
  gimple_stmt_iterator bsi = gsi_start_bb (bb);

  streamer_write_record_start (ob,
			       (!gsi_end_p (bsi)) || phi_nodes (bb)
			        ? LTO_bb1
				: LTO_bb0);

  streamer_write_uhwi (ob, bb->index);
  streamer_write_hwi (ob, bb->count);
  streamer_write_hwi (ob, bb->loop_depth);
  streamer_write_hwi (ob, bb->frequency);
  streamer_write_hwi (ob, bb->flags);

  if (!gsi_end_p (bsi) || phi_nodes (bb))
    {
      /* Output the statements.  The list of statements is terminated
	 with a zero.  */
      for (bsi = gsi_start_bb (bb); !gsi_end_p (bsi); gsi_next (&bsi))
	{
	  int region;
	  gimple stmt = gsi_stmt (bsi);

	  output_gimple_stmt (ob, stmt);

	  /* Emit the EH region holding STMT.  */
	  region = lookup_stmt_eh_lp_fn (fn, stmt);
	  if (region != 0)
	    {
	      streamer_write_record_start (ob, LTO_eh_region);
	      streamer_write_hwi (ob, region);
	    }
	  else
	    streamer_write_record_start (ob, LTO_null);
	}

      streamer_write_record_start (ob, LTO_null);

      for (bsi = gsi_start_phis (bb); !gsi_end_p (bsi); gsi_next (&bsi))
	{
	  gimple phi = gsi_stmt (bsi);

	  /* Only emit PHIs for gimple registers.  PHI nodes for .MEM
	     will be filled in on reading when the SSA form is
	     updated.  */
	  if (is_gimple_reg (gimple_phi_result (phi)))
	    output_phi (ob, phi);
	}

      streamer_write_record_start (ob, LTO_null);
    }
}