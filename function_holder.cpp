#pragma once

#include <utility>
#include <type_traits>

namespace fn_base {

/**
* @brief Utility to manage resources by the appropriate way(particularly, in cases of exception's propagation).
 *            This class keeps a member-evaluator to do appropriate action during instance's destruction.
 *
 */
template <class F>
class generic_guard
{
public:
	/**
	 * @brief Constructor
	 * @param fn the action need to be done during object's destruction.
	 *
	 * @note the evaluated action should be non-throwable.
     *
     * @example
     * auto fn = [&file] () {
     *       file.close();
     * };
     * generic_guard guard<decltype(fn)>(std::move(fn));
	 */
    template <class T = F>
    generic_guard(T&& fn) : m_fn(std::forward<T>(fn))
    {
    }

    /**
     * @brief Destructor Performs appropriate actions.
     */
    ~generic_guard()
    {
        m_fn();
    }

    /// @brief Copy constructor.
    generic_guard(const generic_guard&) = default;

    /// @brief Move constructor.
    generic_guard(generic_guard&&) = default;

    /// @brief Copy assignment operator.
    generic_guard& operator=(const generic_guard&) = default;

    /// @brief Move assignment operator.
    generic_guard& operator=(generic_guard&&) = default;

private:
    F m_fn;
};


/**
 * @brief Abstract functor extension.
 *
 * @note The structure is useful for keeping a pointer to generic_functor instances with different functor types.
 *
 * @example
 * std::vector<std::unique_ptr<functor_extension>> fn_instances;
 *
 * auto fn_1 = [x = 13] () mutable { std::cout<< ++x;};
 * fn_instances.emplace_back(new generic_functor<decltype(fn_1)>(fn_1));
 *
 * auto fn_2 = [x = 3.14] () { std::cout<< x;};
 * fn_instances.emplace_back(new generic_functor<decltype(fn_2)>(fn_2));
 *
 * for (auto& fn : fn_instances) {
 *       (*fn)();
 * }
 */
class functor_extension
{
public:
	/// @brief Abstract function call operator (i.e. operator()).
    virtual void operator()() = 0;

    /// @brief Destructor
    virtual ~functor_extension() = default;
};


/**
 * @brief  General-purpose polymorphic function wrapper.
 *
 * @note This implementation allows the structure to be inherited  from polymorphic type D.
 *
 * @example generic_functor<decltype(fn), functor_extension> instance(fn);
 */
template <class F, class D = functor_extension, typename = void>
class generic_functor : public D
{
public:
	/**
	 * @brief Constructor
	 * @param fn any callable target -- functions, lambda expressions, need to be evaluated
	 */
    template <typename ... D_Args, class T = F>
    generic_functor(T&& fn, D_Args&& ... args)
            : D(std::forward<D_Args>(args) ...)
            , m_func(std::forward<T>(fn))
    {
    }

    /// @brief Overriding of function call operator(i.e. operator()).
    ///             Performs execution of callable member object.
    virtual void operator()() override
    {
        m_func();
    }

    /// @brief Copy constructor.
    generic_functor(const generic_functor&) = default;

    /// @brief Move constructor.
    generic_functor(generic_functor&&) = default;

    /// @brief Copy assignment operator.
    generic_functor& operator=(const generic_functor&) = default;

    /// @brief Move assignment operator.
    generic_functor& operator=(generic_functor&&) = default;

    /// @brief Destructor.
    virtual ~generic_functor() = default;

private:
    F m_func;
};


/**
 * @brief  General-purpose function wrapper.
 *
 * @note This implementation allows the structure to be non-polymorphic and provide more optimal implementation.
 *
 *@example generic_functor<decltype(fn), bool> instance(fn);
 */
template <class F, class D>
class generic_functor<F, D, typename std::enable_if<!std::is_polymorphic<D>::value>::type>
{
public:
	/**
	 * @brief Constructor
	 * @param fn any callable target -- functions, lambda expressions, need to be evaluated.
	 */
    template <class T = F>
    generic_functor(T&& fn) : m_func(std::forward<T>(fn))
    {
    }

    /// @brief Overriding of function call operator(i.e. operator()).
    ///             Performs execution of callable member object.
    void operator()()
    {
        m_func();
    }

    /// @brief Copy constructor.
    generic_functor(const generic_functor&) = default;

    /// @brief Move constructor.
    generic_functor(generic_functor&&) = default;

    /// @brief Copy assignment operator.
    generic_functor& operator=(const generic_functor&) = default;

    /// @brief Move assignment operator.
    generic_functor& operator=(generic_functor&&) = default;

    /// @brief Destructor.
    ~generic_functor() = default;

private:
    F m_func;
};


/**
 * @brief  General-purpose polymorphic function wrapper.
 *
 * @note This implementation allows the structure to be inherited  from polymorphic type D.
 *       The main difference from generic_functor is support to be used for function object having variadic template arguments.
 *
 * @example variadic_functor<decltype(fn), base_class, void*> instance(fn, arg1, arg2);
 */
template <class F, class D, typename ... Args>
class variadic_functor: public D
{
public:
	/**
	 * @brief Constructor
	 * @param fn any callable target -- functions, lambda expressions, need to be evaluated.
	 * @oaram args arguments to pass base class's constructor
	 */
    template <typename ... D_Args, class T = F>
    variadic_functor(T&& fn, D_Args&& ... args)
             : D(std::forward<D_Args>(args)...)
             , m_func(std::forward<T>(fn))
    {
    }

    /**
     * @brief Overriding of function call operator(i.e. operator()).
     *        Performs execution of callable member object.
     * @param args arguments to pass to callable object
     */
    virtual void operator()(Args...args) override
    {
        m_func(args...);
    }

    /// @brief Copy constructor.
    variadic_functor(const variadic_functor&) = default;

    /// @brief Move constructor.
    variadic_functor(variadic_functor&&) = default;

    /// @brief Copy assignment operator.
    variadic_functor& operator=(const variadic_functor&) = default;

    /// @brief Move assignment operator.
    variadic_functor& operator=(variadic_functor&&) = default;

    /// @brief Destructor.
    virtual ~variadic_functor() = default;

private:
    F m_func;
};

} // fn_base namespace
