/* General-purpose hooks.
   Copyright (C) 2002, 2003, 2004, 2005, 2007, 2008, 2009, 2010, 2011
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  */

/* This file contains generic hooks that can be used as defaults for
   target or language-dependent hook initializers.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "hooks.h"

/* Generic hook that does absolutely zappo.  */
void
hook_void_void (void)
{
}

/* Generic hook that takes no arguments and returns false.  */
bool
hook_bool_void_false (void)
{
  return false;
}

/* Generic hook that takes no arguments and returns true.  */
bool
hook_bool_void_true (void)
{
  return true;
}

/* Generic hook that takes (bool) and returns false.  */
bool
hook_bool_bool_false (bool a ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes (bool, struct gcc_options *) and returns false.  */
bool
hook_bool_bool_gcc_optionsp_false (bool a ATTRIBUTE_UNUSED,
				   struct gcc_options *opts ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes const int, const int) and returns true.  */
bool hook_bool_const_int_const_int_true (const int a ATTRIBUTE_UNUSED,
                                         const int b ATTRIBUTE_UNUSED)
{
  return true;
}

/* Generic hook that takes (enum machine_mode) and returns false.  */
bool
hook_bool_mode_false (enum machine_mode mode ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes (enum machine_mode) and returns true.  */
bool
hook_bool_mode_true (enum machine_mode mode ATTRIBUTE_UNUSED)
{
  return true;
}

/* Generic hook that takes (enum machine_mode, const_rtx) and returns false.  */
bool
hook_bool_mode_const_rtx_false (enum machine_mode mode ATTRIBUTE_UNUSED,
				const_rtx value ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes (enum machine_mode, const_rtx) and returns true.  */
bool
hook_bool_mode_const_rtx_true (enum machine_mode mode ATTRIBUTE_UNUSED,
			       const_rtx value ATTRIBUTE_UNUSED)
{
  return true;
}

/* Generic hook that takes (enum machine_mode, rtx) and returns false.  */
bool
hook_bool_mode_rtx_false (enum machine_mode mode ATTRIBUTE_UNUSED,
			  rtx value ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes (enum machine_mode, rtx) and returns true.  */
bool
hook_bool_mode_rtx_true (enum machine_mode mode ATTRIBUTE_UNUSED,
			 rtx value ATTRIBUTE_UNUSED)
{
  return true;
}

/* Generic hook that takes (enum machine_mode, unsigned HOST_WIDE_INT)
   and returns false.  */
bool
hook_bool_mode_uhwi_false (enum machine_mode mode ATTRIBUTE_UNUSED,
			   unsigned HOST_WIDE_INT value ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes (FILE *, const char *) and does nothing.  */
void
hook_void_FILEptr_constcharptr (FILE *a ATTRIBUTE_UNUSED, const char *b ATTRIBUTE_UNUSED)
{
}

/* Used for the TARGET_ASM_CAN_OUTPUT_MI_THUNK hook.  */
bool
hook_bool_const_tree_hwi_hwi_const_tree_false (const_tree a ATTRIBUTE_UNUSED,
					       HOST_WIDE_INT b ATTRIBUTE_UNUSED,
					       HOST_WIDE_INT c ATTRIBUTE_UNUSED,
					       const_tree d ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_const_tree_hwi_hwi_const_tree_true (const_tree a ATTRIBUTE_UNUSED,
					      HOST_WIDE_INT b ATTRIBUTE_UNUSED,
					      HOST_WIDE_INT c ATTRIBUTE_UNUSED,
					      const_tree d ATTRIBUTE_UNUSED)
{
  return true;
}

bool
default_can_output_mi_thunk_no_vcall (const_tree a ATTRIBUTE_UNUSED,
				      HOST_WIDE_INT b ATTRIBUTE_UNUSED,
				      HOST_WIDE_INT c,
				      const_tree d ATTRIBUTE_UNUSED)
{
  return c == 0;
}

int
hook_int_uint_mode_1 (unsigned int a ATTRIBUTE_UNUSED,
		      enum machine_mode b ATTRIBUTE_UNUSED)
{
  return 1;
}

int
hook_int_const_tree_0 (const_tree a ATTRIBUTE_UNUSED)
{
  return 0;
}

/* ??? Used for comp_type_attributes, which ought to return bool.  */
int
hook_int_const_tree_const_tree_1 (const_tree a ATTRIBUTE_UNUSED, const_tree b ATTRIBUTE_UNUSED)
{
  return 1;
}

int
hook_int_rtx_0 (rtx a ATTRIBUTE_UNUSED)
{
  return 0;
}

int
hook_int_rtx_bool_0 (rtx a ATTRIBUTE_UNUSED, bool b ATTRIBUTE_UNUSED)
{
  return 0;
}

unsigned int
hook_uint_void_0 (void)
{
  return 0;
}

void
hook_void_tree (tree a ATTRIBUTE_UNUSED)
{
}

void
hook_void_constcharptr (const char *a ATTRIBUTE_UNUSED)
{
}

void
hook_void_tree_treeptr (tree a ATTRIBUTE_UNUSED, tree *b ATTRIBUTE_UNUSED)
{
}

void
hook_void_int_int (int a ATTRIBUTE_UNUSED, int b ATTRIBUTE_UNUSED)
{
}

bool
hook_bool_tree_false (tree a ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_const_tree_false (const_tree a ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_tree_true (tree a ATTRIBUTE_UNUSED)
{
  return true;
}

bool
hook_bool_const_tree_true (const_tree a ATTRIBUTE_UNUSED)
{
  return true;
}

bool
hook_bool_tree_tree_false (tree a ATTRIBUTE_UNUSED, tree b ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_tree_tree_true (tree a ATTRIBUTE_UNUSED, tree b ATTRIBUTE_UNUSED)
{
  return true;
}

bool
hook_bool_tree_bool_false (tree a ATTRIBUTE_UNUSED, bool b ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_rtx_false (rtx a ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_uintp_uintp_false (unsigned int *a ATTRIBUTE_UNUSED,
			     unsigned int *b ATTRIBUTE_UNUSED)
{
  return false;
}

bool
hook_bool_rtx_int_int_int_intp_bool_false (rtx a ATTRIBUTE_UNUSED,
					   int b ATTRIBUTE_UNUSED,
					   int c ATTRIBUTE_UNUSED,
					   int d ATTRIBUTE_UNUSED,
					   int *e ATTRIBUTE_UNUSED,
					   bool speed_p ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes an rtx and returns it.  */
rtx
hook_rtx_rtx_identity (rtx x)
{
  return x;
}

/* Generic hook that takes an rtx and returns NULL_RTX.  */
rtx
hook_rtx_rtx_null (rtx x ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Generic hook that takes a tree and an int and returns NULL_RTX.  */
rtx
hook_rtx_tree_int_null (tree a ATTRIBUTE_UNUSED, int b ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Generic hook that takes three trees and returns the last one as is.  */
tree
hook_tree_tree_tree_tree_3rd_identity (tree a ATTRIBUTE_UNUSED,
				       tree b ATTRIBUTE_UNUSED, tree c)
{
  return c;
}

/* Generic hook that takes a tree and returns a NULL string.  */
const char *
hook_constcharptr_const_tree_null (const_tree t ATTRIBUTE_UNUSED)
{
  return NULL;
}

tree
hook_tree_tree_int_treep_bool_null (tree t0 ATTRIBUTE_UNUSED,
				    int i ATTRIBUTE_UNUSED,
				    tree *p ATTRIBUTE_UNUSED,
				    bool ignore ATTRIBUTE_UNUSED)
{
  return NULL;
}

tree
hook_tree_tree_tree_null (tree t0 ATTRIBUTE_UNUSED, tree t1 ATTRIBUTE_UNUSED)
{
  return NULL;
}

tree
hook_tree_tree_tree_tree_null (tree t0 ATTRIBUTE_UNUSED,
			       tree t1 ATTRIBUTE_UNUSED,
			       tree t2 ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Generic hook that takes a rtx and returns a NULL string.  */
const char *
hook_constcharptr_const_rtx_null (const_rtx r ATTRIBUTE_UNUSED)
{
  return NULL;
}

const char *
hook_constcharptr_const_tree_const_tree_null (const_tree t0 ATTRIBUTE_UNUSED,
					      const_tree t1 ATTRIBUTE_UNUSED)
{
  return NULL;
}

const char *
hook_constcharptr_int_const_tree_null (int i ATTRIBUTE_UNUSED,
				       const_tree t0 ATTRIBUTE_UNUSED)
{
  return NULL;
}

const char *
hook_constcharptr_int_const_tree_const_tree_null (int i ATTRIBUTE_UNUSED,
						  const_tree t0 ATTRIBUTE_UNUSED,
						  const_tree t1 ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Generic hook that takes a const_tree and returns NULL_TREE.  */
tree
hook_tree_const_tree_null (const_tree t ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Generic hook that takes a rtx and an int and returns a bool.  */

bool
hook_bool_rtx_int_false (rtx insn ATTRIBUTE_UNUSED, int mode ATTRIBUTE_UNUSED)
{
  return false;
}

/* Generic hook that takes a rtx and an int and returns void.  */

void
hook_void_rtx_int (rtx insn ATTRIBUTE_UNUSED, int mode ATTRIBUTE_UNUSED)
{
}

/* Generic hook that takes a struct gcc_options * and returns void.  */

void
hook_void_gcc_optionsp (struct gcc_options *opts ATTRIBUTE_UNUSED)
{
}
