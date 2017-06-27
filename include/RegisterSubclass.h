#ifndef REGISTER_SUBCLASS_H
#define REGISTER_SUBCLASS_H

#include <memory>
#include <type_traits>

template <typename Base, typename Child>
class RegisterSubclass {
       public:
	struct Register {
		Register() {
			using maptype = typename std::remove_reference<decltype(
			    Base::factories())>::type;
			std::unique_ptr<typename Child::Factory> factory =
			    std::make_unique<typename Child::Factory>();
			Base::factories().insert((typename maptype::value_type){
			    Child::type_name, std::move(factory)});
		}
	};

       private:
	static Register reg;
};

template <typename Base, typename Child>
typename RegisterSubclass<Base, Child>::Register
    RegisterSubclass<Base, Child>::reg{};

#endif  // REGISTER_SUBCLASS_H
