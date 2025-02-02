// { dg-options "-std=gnu++0x" }

// Copyright (C) 2011 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.


#include <memory>
#include <testsuite_hooks.h>

struct Ptr
{
  typedef bool element_type;
  bool* value;

  static Ptr pointer_to(bool& b) { return Ptr{&b}; }
};

void test01()
{
  bool test = true;
  Ptr p __attribute__((unused)) {&test};

  VERIFY( std::pointer_traits<Ptr>::pointer_to(test).value == &test );
}

void test02()
{
  bool test = true;

  VERIFY( std::pointer_traits<bool*>::pointer_to(test) == &test );
}

int main()
{
  test01();
  test02();
  return 0;
}
