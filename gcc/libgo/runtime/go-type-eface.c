/* go-type-eface.c -- hash and equality empty interface functions.

   Copyright 2010 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

#include "interface.h"
#include "go-panic.h"
#include "go-type.h"

/* A hash function for an empty interface.  */

size_t
__go_type_hash_empty_interface (const void *vval,
				size_t key_size __attribute__ ((unused)))
{
  const struct __go_empty_interface *val;
  const struct __go_type_descriptor *descriptor;
  size_t size;

  val = (const struct __go_empty_interface *) vval;
  descriptor = val->__type_descriptor;
  if (descriptor == NULL)
    return 0;
  size = descriptor->__size;
  if (__go_is_pointer_type (descriptor))
    return descriptor->__hashfn (&val->__object, size);
  else
    return descriptor->__hashfn (val->__object, size);
}

/* An equality function for an empty interface.  */

_Bool
__go_type_equal_empty_interface (const void *vv1, const void *vv2,
				 size_t key_size __attribute__ ((unused)))
{
  const struct __go_empty_interface *v1;
  const struct __go_empty_interface *v2;
  const struct __go_type_descriptor* v1_descriptor;
  const struct __go_type_descriptor* v2_descriptor;

  v1 = (const struct __go_empty_interface *) vv1;
  v2 = (const struct __go_empty_interface *) vv2;
  v1_descriptor = v1->__type_descriptor;
  v2_descriptor = v2->__type_descriptor;
  if (((uintptr_t) v1_descriptor & reflectFlags) != 0
      || ((uintptr_t) v2_descriptor & reflectFlags) != 0)
    __go_panic_msg ("invalid interface value");
  if (v1_descriptor == NULL || v2_descriptor == NULL)
    return v1_descriptor == v2_descriptor;
  if (!__go_type_descriptors_equal (v1_descriptor, v2_descriptor))
    return 0;
  if (__go_is_pointer_type (v1_descriptor))
    return v1->__object == v2->__object;
  else
    return v1_descriptor->__equalfn (v1->__object, v2->__object,
				     v1_descriptor->__size);
}
