// Allocator traits -*- C++ -*-

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

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file ext/alloc_traits.h
 *  This file is a GNU extension to the Standard C++ Library.
 */

#ifndef _EXT_ALLOC_TRAITS_H
#define _EXT_ALLOC_TRAITS_H 1

#pragma GCC system_header

#ifdef __GXX_EXPERIMENTAL_CXX0X__
# include <bits/alloc_traits.h>
#else
# include <bits/allocator.h>  // for __alloc_swap
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
  template<typename> struct allocator;
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#ifdef __GXX_EXPERIMENTAL_CXX0X__
template<typename _Alloc>
  struct __allocator_always_compares_equal
  { static const bool value = false; };

  template<typename _Tp>
    struct __allocator_always_compares_equal<std::allocator<_Tp>>
    { static const bool value = true; };

  template<typename, typename> struct array_allocator;

  template<typename _Tp, typename _Array>
    struct __allocator_always_compares_equal<array_allocator<_Tp, _Array>>
    { static const bool value = true; };

  template<typename> struct mt_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<mt_allocator<_Tp>>
    { static const bool value = true; };

  template<typename> struct new_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<new_allocator<_Tp>>
    { static const bool value = true; };

  template<typename> struct pool_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<pool_allocator<_Tp>>
    { static const bool value = true; };
#endif

/**
 * @brief  Uniform interface to C++98 and C++0x allocators.
 * @ingroup allocators
*/
template<typename _Alloc>
  struct __alloc_traits
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  : std::allocator_traits<_Alloc>
#endif
  {
    typedef _Alloc allocator_type;
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    typedef std::allocator_traits<_Alloc>           _Base_type;
    typedef typename _Base_type::value_type         value_type;
    typedef typename _Base_type::pointer            pointer;
    typedef typename _Base_type::const_pointer      const_pointer;
    typedef typename _Base_type::size_type          size_type;
    // C++0x allocators do not define reference or const_reference
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    using _Base_type::allocate;
    using _Base_type::deallocate;
    using _Base_type::construct;
    using _Base_type::destroy;

  private:
    template<typename _Ptr>
      struct __is_custom_pointer
      : std::integral_constant<bool, std::is_same<pointer, _Ptr>::value
                                     && !std::is_pointer<_Ptr>::value>
      { };

  public:
    template<typename _Ptr, typename... _Args>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      construct(_Alloc& __a, _Ptr __p, _Args&&... __args)
      {
	_Base_type::construct(__a, std::addressof(*__p),
			      std::forward<_Args>(__args)...);
      }

    template<typename _Ptr>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      destroy(_Alloc& __a, _Ptr __p)
      { _Base_type::destroy(__a, std::addressof(*__p)); }

    static _Alloc _S_select_on_copy(const _Alloc& __a)
    { return _Base_type::select_on_container_copy_construction(__a); }

    static void _S_on_swap(_Alloc& __a, _Alloc& __b)
    { std::__alloc_on_swap(__a, __b); }

    static constexpr bool _S_propagate_on_copy_assign()
    { return _Base_type::propagate_on_container_copy_assignment::value; }

    static constexpr bool _S_propagate_on_move_assign()
    { return _Base_type::propagate_on_container_move_assignment::value; }

    static constexpr bool _S_propagate_on_swap()
    { return _Base_type::propagate_on_container_swap::value; }

    static constexpr bool _S_always_equal()
    { return __allocator_always_compares_equal<_Alloc>::value; }

    static constexpr bool _S_nothrow_move()
    { return _S_propagate_on_move_assign() || _S_always_equal(); }

    static constexpr bool _S_nothrow_swap()
    {
      using std::swap;
      return !_S_propagate_on_swap()
       	|| noexcept(swap(std::declval<_Alloc&>(), std::declval<_Alloc&>()));
    }

#else

    typedef typename _Alloc::pointer                pointer;
    typedef typename _Alloc::const_pointer          const_pointer;
    typedef typename _Alloc::value_type             value_type;
    typedef typename _Alloc::reference              reference;
    typedef typename _Alloc::const_reference        const_reference;
    typedef typename _Alloc::size_type              size_type;

    static pointer
    allocate(_Alloc& __a, size_type __n)
    { return __a.allocate(__n); }

    static void deallocate(_Alloc& __a, pointer __p, size_type __n)
    { __a.deallocate(__p, __n); }

    template<typename _Tp>
      static void construct(_Alloc& __a, pointer __p, const _Tp& __arg)
      { __a.construct(__p, __arg); }

    static void destroy(_Alloc& __a, pointer __p)
    { __a.destroy(__p); }

    static const _Alloc& _S_select_on_copy(const _Alloc& __a) { return __a; }

    static void _S_on_swap(_Alloc& __a, _Alloc& __b)
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 431. Swapping containers with unequal allocators.
      std::__alloc_swap<_Alloc>::_S_do_it(__a, __b);
    }
#endif
  };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif