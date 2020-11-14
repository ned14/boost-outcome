/* A very simple result type
(C) 2017-2020 Niall Douglas <http://www.nedproductions.biz/> (10 commits)
File Created: June 2017


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_BOOST_RESULT_HPP
#define BOOST_OUTCOME_BOOST_RESULT_HPP

#include "config.hpp"

#include "boost/system/system_error.hpp"
#include "boost/exception_ptr.hpp"
#include "boost/version.hpp"

BOOST_OUTCOME_V2_NAMESPACE_EXPORT_BEGIN

/*! AWAITING HUGO JSON CONVERSION TOOL
SIGNATURE NOT RECOGNISED
*/
namespace policy
{
  namespace detail
  {
    /* Pass through `make_error_code` function for `boost::system::error_code`.
     */
    inline boost::system::error_code make_error_code(boost::system::error_code v) { return v; }

    /* Pass through `make_exception_ptr` function for `boost::exception_ptr`.
    The reason this needs to be here, declared before the rest of Outcome,
    is that there is no boost::make_exception_ptr as Boost still uses the old
    naming boost::copy_exception. Therefore the ADL discovered make_exception_ptr
    doesn't work, hence this hacky pre-declaration here.

    I was tempted to just inject a boost::make_exception_ptr, but I can see
    Boost doing that itself at some point. This hack should keep working after.
    */
    inline boost::exception_ptr make_exception_ptr(boost::exception_ptr v) { return v; }
  }  // namespace detail
}  // namespace policy
BOOST_OUTCOME_V2_NAMESPACE_END

#include "std_result.hpp"


// ADL injection of outcome_throw_as_system_error_with_payload
namespace boost
{
  namespace system
  {
    inline void outcome_throw_as_system_error_with_payload(const error_code &error) { BOOST_OUTCOME_THROW_EXCEPTION(system_error(error)); }
    namespace errc
    {
      BOOST_OUTCOME_TEMPLATE(class Error)
      BOOST_OUTCOME_TREQUIRES(BOOST_OUTCOME_TPRED(is_error_code_enum<std::decay_t<Error>>::value || is_error_condition_enum<std::decay_t<Error>>::value))
      inline void outcome_throw_as_system_error_with_payload(Error &&error) { BOOST_OUTCOME_THROW_EXCEPTION(system_error(make_error_code(error))); }
    }  // namespace errc
  }    // namespace system
}  // namespace boost

BOOST_OUTCOME_V2_NAMESPACE_EXPORT_BEGIN

namespace detail
{
  // Customise _set_error_is_errno
  template <class State> constexpr inline void _set_error_is_errno(State &state, const boost::system::error_code &error)
  {
    if(error.category() == boost::system::generic_category()
#ifndef _WIN32
       || error.category() == boost::system::system_category()
#endif
    )
    {
      state._status.set_have_error_is_errno(true);
    }
  }
  template <class State> constexpr inline void _set_error_is_errno(State &state, const boost::system::error_condition &error)
  {
    if(error.category() == boost::system::generic_category()
#ifndef _WIN32
       || error.category() == boost::system::system_category()
#endif
    )
    {
      state._status.set_have_error_is_errno(true);
    }
  }
  template <class State> constexpr inline void _set_error_is_errno(State &state, const boost::system::errc::errc_t & /*unused*/)
  {
    state._status.set_have_error_is_errno(true);
  }

}  // namespace detail

/*! AWAITING HUGO JSON CONVERSION TOOL
SIGNATURE NOT RECOGNISED
*/
namespace trait
{
  namespace detail
  {
    // Shortcut these for lower build impact
    template <> struct _is_error_code_available<boost::system::error_code>
    {
      static constexpr bool value = true;
      using type = boost::system::error_code;
    };
    template <> struct _is_exception_ptr_available<boost::exception_ptr>
    {
      static constexpr bool value = true;
      using type = boost::exception_ptr;
    };
  }  // namespace detail

  // boost::system::error_code is an error type
  template <> struct is_error_type<boost::system::error_code>
  {
    static constexpr bool value = true;
  };
  // boost::system::error_code::errc_t is an error type
  template <> struct is_error_type<boost::system::errc::errc_t>
  {
    static constexpr bool value = true;
  };
  // boost::exception_ptr is an error types
  template <> struct is_error_type<boost::exception_ptr>
  {
    static constexpr bool value = true;
  };
  // For boost::system::error_code, boost::system::is_error_condition_enum<> is the trait we want.
  template <class Enum> struct is_error_type_enum<boost::system::error_code, Enum>
  {
    static constexpr bool value = boost::system::is_error_condition_enum<Enum>::value;
  };

}  // namespace trait


/*! AWAITING HUGO JSON CONVERSION TOOL
SIGNATURE NOT RECOGNISED
*/
template <class R, class S = boost::system::error_code, class NoValuePolicy = policy::default_policy<R, S, void>>  //
using boost_result = basic_result<R, S, NoValuePolicy>;

/*! AWAITING HUGO JSON CONVERSION TOOL
type alias template <class R, class S = boost::system::error_code> boost_unchecked. Potential doc page: `boost_unchecked<T, E = boost::system::error_code>`
*/
template <class R, class S = boost::system::error_code> using boost_unchecked = boost_result<R, S, policy::all_narrow>;

/*! AWAITING HUGO JSON CONVERSION TOOL
type alias template <class R, class S = boost::system::error_code> boost_checked. Potential doc page: `boost_checked<T, E = boost::system::error_code>`
*/
template <class R, class S = boost::system::error_code> using boost_checked = boost_result<R, S, policy::throw_bad_result_access<S, void>>;

BOOST_OUTCOME_V2_NAMESPACE_END

#endif
