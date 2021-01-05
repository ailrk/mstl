### My stl

To learn stl better.

### Structure
I don't bother implement anything from `<c...>` like `<cstddef>`, since they are just c facility and has nothing to do with templates. All functions are under the namespace `mstl`, and structured in the same way as the stl does. Lot's of implemnetations are simplified, because it's a pedagogical project. Some comments are added for highlight some details.

### Some logs

Not only for stl, just some notes on c++ in general.

- What's so good about stl? It's pretty generic and less verbose, but it does have these compile time duck typing feeling. Today we have concept, so we can do type constraint, but before concept if you don't bother writing sfinae, your types are basically naked. Yet you see people on Stackoverflow in 2014 hyped c++ stl as if it's the ultimate abstraction method. Saying duck typing is not 100% correct, because you still have type checking. If the type you pass to the tempalte doesn't implement a function it uses it will fail to compile. But in case like passing an OutputIter to `std::transform`, normally you will pass a `back_insert_iterator`, but also you can anything else that implemented `*` and `++` since the implementation only uses these two operators. Now the problem becomes, does it reflects the programmer's intention? If I pass a allocator with `*` and `++` implemneted to `std::transform`, does it make sense? (2021-01-04)

  - So the main voice in c++ community seems to be only type check when it's necessary.
  - Old school way of checking type constraint is either `enable_if` or `static_assert`.
  - This type of type checking really feels like a hack for me tbh.
  - The idea behind `iterator_traits` really is trying to access type T and see if it has certain field. If not you have a compile time error.
  - Nobody stop you from doing the things I mentioned on the root of these posts. You can pass allocator as iterator all you like as long as the function used is implemented. I think it is sketchy. People's defense for it is that it's uncommon to do such thing, which doesn't really stand. It's uncommon to double free too, why do we want to avoid that?
  - Another problem I learnt from some post, if I have code like below, if I don't call `foo` I can pass both `A` and `B`, but if I call `foo` later all the sudden it will be a compile error.
```c++
struct A {int a;}; struct B { int a; int b: };
template <typename T> class K { T n; int foo() {return n.a + n.b}};
```

- With concepts I feel I can really be confident about the type safety while still have all the nice things about generic programming. Concept is just really nice.
  - Not like typeclass, concept only exists at compile time.
  - There is no declaring instance for concepts, as it will works like normal type traits, declarations are implicit.
  - Other than behaves like a typeclass, concepts also check some properties. e.g you can check things like if number of type parameters in the varadic template is 5.

- Tricks like `iterator_trait` provides a unified interface is pretty common in stl. But again after c++20 you just write a iterator concept and constraint on the type parameter.

- Type traits like `remove_reference` and `is_same` are still required because they provide the basic type level operations.

- Because you can pass values to c++ templates, it works as if `DataKinds` extension on in Haskell directly. Besides that, you have higher kinded type support for the entire time, it's just ugly to write: `template<tempalte<typename> typename F>`; this says you have a type `f a`.

- One important reason for c++ to be so heavily dependend on stl is to achieve zero overhead abstraction. If you provide an OOP style stand library, you can't avoid having virtual table if you want real so called "polymorphic" behavior. (comparing with the Java style "polymorphic over an inheritance chain", parametric polymorphism is much more polymorphic for me though)

- Another thing I found c++ people hyped about is template meta programming. But template are not really "meta programming" if you consider meta programming in the sense of common lisp macro. You do use tempalte to generate code, but with very limited control. The common technique with sfinae is more like helping you choose what correct code to produce or what code to reject, rather then literatly create the code for you. Constexpr does run some code at compile time, but it focus more on compute some stuffs at compile time; there is no concept like read time in common lisp (c preprocessor macro is real meta programming, but it sucks so much).  PS: An exceptoin is the varadic template, these feature really create code for you.

- RANT: why related functions are put in different headers? How `accumulate` is not part of functional? And why it's it called `accumulate` instead of fold anyway? Why `innder_product` instead of `zip`? If the less generic version of `zip` is `inner product` then why don't make them two different functions?  Why do we need `numeric` header when functions there all look like algoritm? Also functor means function object, why!!

- Lots of funtions in the stl have the same name in common lisp. I wonder if there are any relationship. `remove_if` for example; who doesn't call it filter today...

-  Allocator also depends heavily on type traits, I wonder if it will be replaced by concept in the future.

- Some stl tricks only exist on folklore. For example, you write this type a lot: `template <typename T> struct identity { using type = T; };`, because `Identity<T>::type` will be not be deduced. Lots of idioms need to be learnt from reading the source code (I guess it's the same for all langauges.)

- Remember to remove cv qualifers when writing type traits. C++ is weakly typed so sometimes types with different qualifiers can convert to each others, but somteimes they are totally different types. When doing type level operations it's better to assume they are not the same.

- When writing `enable_if`, make sure you are not overloading the same default type parameters, because the compiler will treat them as the same specialization, thus a redefinition.

- You have these weird pointer to member data type `T U::*`...

- I'm starting to appreciate how much work around it takes to implemnet all those `is_...` type traits. There are lots of implementations relies on exluding cases. For example, if a type is not fundamental, then it's a compound type; if a type can specialize to `C::*` but is not a union, then it's a class etc...

- The biggest achievement of stl with generic programming might be abstract the iterator out, so the same iterface can work on anything act like iterator. With iterator, you have bring this notion of pointer to anywhere acts like a sequece. Many other components like allocator also have similar vibe, but not as impactful as iterator I guess.

- Exception is probably the only place stl uses inheritence. It makes sense for exception because you do have this hierachy between different exceptions naturally.

- One take away from stl is you can write your own type trait, and use it as interface over different typesl.

```c++
// this doesn't work, because you only have one overload really.
template <typename A, typename = enable_if_t<is_integral_v<T>, bool>>
void foo(A);

template <typename B, typename = enable_if_t<is_floating_v<T>, bool>>
void foo(B);

```

### Source

Some materials I read alone the way.

https://stackoverflow.com/questions/1039853/why-is-the-c-stl-is-so-heavily-based-on-templates-and-not-on-interfaces

https://stackoverflow.com/questions/25386765/why-is-it-bad-to-impose-type-constraints-on-templates-in-c

https://www.reddit.com/r/haskell/comments/1e9f49/concepts_in_c_template_programming_and_type/
