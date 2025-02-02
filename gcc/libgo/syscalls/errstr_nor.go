// errstr.go -- Error strings when there is no strerror_r.

// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package syscall

import (
	"sync"
	"unsafe"
)

func libc_strerror(int) *byte __asm__ ("strerror")

var errstr_lock sync.Mutex

func Errstr(errno int) string {
	errstr_lock.Lock()

	bp := libc_strerror(errno)
	b := (*[1000]byte)(unsafe.Pointer(bp))
	i := 0
	for b[i] != 0 {
		i++
	}
	s := string(b[:i])

	errstr_lock.Unlock()

	return s
}
