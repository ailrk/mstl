### My stl

To learn stl better.

Some materials I read alone the way.
https://stackoverflow.com/questions/1039853/why-is-the-c-stl-is-so-heavily-based-on-templates-and-not-on-interfaces


### Structure
I don't bother implement anything from `<c...>` like `<cstddef>`, since they are just c facility and has nothing to do with templates. All functions are under the namespace `mstl`, and structured in the same way as the stl does. Lot's of implemnetations are simplified, because it's a pedagogical project. Some comments are added for highlight some details.

### Some logs

Not only for stl, just some notes on c++ in general.

* I think people overly hyped stl. It's pretty generic and less verbose then full blown oop, but it does have these compile time duck typing feeling. Today we have concept, so things get much better, but before concept if you don't bother writing sfinae, your types are basically naked. Yet you see people on Stackoverflow in 2014 hyped c++ stl as if it's the ultimate abstraction method.

* With concepts I feel I can really be confident about the type safety while still have all the nice things about generic programming. Concept is just really nice.

* Tricks like `iterator_trait` provides a unified interface is pretty common in stl. But again after c++20 you just write a iterator concept and constraint on the type parameter.

* Type traits like `remove_reference` and `is_same` are still required because they provide the basic type level operations.

* Because you can pass values to c++ templates, it works as if `DataKinds` extension on in Haskell directly. Besides that, you have higher kinded type support for the entire time, it's just ugly to write: `template<tempalte<typename> typename F>`; this says you have a type `f a`.

* One important reason for c++ to be so heavily dependend on stl is to achieve zero overhead abstraction. If you provide an OOP style stand library, you can't avoid having virtual table if you want real so called "polymorphic" behavior. (comparing with the Java style "polymorphic over an inheritance chain", parametric polymorphism is much more polymorphic for me though)

* Another thing I found c++ people hyped about is template meta programming. But template are not really "meta programming" if you consider meta programming in the sense of common lisp macro. You do use tempalte to generate code, but with very limited control. The common technique with sfinae is more like helping you choose what correct code to produce or what code to reject, rather then literatly create the code for you. Constexpr does run some code at compile time, but it focus more on compute some stuffs at compile time; there is no concept like read time in common lisp (c preprocessor macro is real meta programming, but it sucks so much).  PS: An exceptoin is the varadic template, these feature really create code for you.

* RANT: why related functions are put in different headers? How `accumulate` is not part of functional? And why it's it called `accumulate` instead of fold anyway? Why `innder_product` instead of `zip`? If the less generic version of `zip` is `inner product` then why don't make them two different functions?  Why do we need `numeric` header when functions there all look like algoritm?

* Lots of funtions in the stl have the same name in common lisp. I wonder if there are any relationship. `remove_if` for example; who doesn't call it filter today...

*  Allocator also depends heavily on type traits, I wonder if it will be replaced by concept in the future.

* Some stl tricks only exist on folklore. For example, you write this type a lot: `template <typename T> struct identity { using type = T; };`, because `Identity<T>::type` will be not be deduced. Lots of idioms need to be learnt from reading the source code (I guess it's the same for all langauges.)
