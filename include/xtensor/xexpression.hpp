/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEXPRESSION_HPP
#define XEXPRESSION_HPP

#include <type_traits>

#include "xutils.hpp"

namespace xt
{

    template <class D>
    class xexpression
    {

    public:

        using derived_type = D;

        derived_type& derived_cast() noexcept;
        const derived_type& derived_cast() const noexcept;

    protected:

        xexpression() = default;
        ~xexpression() = default;

        xexpression(const xexpression&) = default;
        xexpression& operator=(const xexpression&) = default;

        xexpression(xexpression&&) = default;
        xexpression& operator=(xexpression&&) = default;
    };

    /******************************
     * xexpression implementation *
     ******************************/

    template <class D>
    inline auto xexpression<D>::derived_cast() noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto xexpression<D>::derived_cast() const noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }

    template <class E>
    using is_xexpression = std::is_base_of<xexpression<E>, E>;

    template <class E, class R>
    using disable_xexpression = typename std::enable_if<!is_xexpression<E>::value, R>::type;

    template <class... E>
    using has_xexpression = or_<is_xexpression<E>...>;

    /************************
     * get_xexpression_type *
     ************************/

    template <class T>
    class xscalar;

    // Workaround for buggy error C2210 in VS2015 when using condtional_t
    template <class E>
    using get_xexpression_type = typename std::conditional<is_xexpression<E>::value, E, xscalar<E>>::type;

    /*******************
     * get_xexpression *
     *******************/

    template <class E>
    inline const E& get_xexpression(const xexpression<E>& e) noexcept
    {
        return e.derived_cast();
    }

    template <class E>
    inline disable_xexpression<E, xscalar<E>> get_xexpression(const E& e) noexcept
    {
        return xscalar<E>(e);
    }

    /******************
     * get_value_type *
     ******************/

    namespace detail
    {
        template <class E, class enable = void>
        struct get_value_type_impl
        {
            using type = E;
        };

        template <class E>
        struct get_value_type_impl<E, std::enable_if_t<is_xexpression<E>::value, void>>
        {
            using type = typename E::value_type;
        };
    }

    template <class E>
    using get_value_type = typename detail::get_value_type_impl<E>::type;
}

#endif

