#ifndef OUTPUT_H
#define OUTPUT_H

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "RegisterSubclass.h"

namespace Output {

class Base {
       public:
	class Factory {
	       public:
		virtual std::unique_ptr<Base> create_from_YAML(
		    const YAML::Node&) = 0;
		virtual ~Factory() {}
	};

       private:
	static auto& factories() {
		static std::map<std::string, std::unique_ptr<Factory>> f;
		return f;
	};
	template <typename Base, typename Child>
	friend RegisterSubclass<Base, Child>::Register::Register();

       public:
	static const auto& get_factories() { return factories(); }
	virtual void write_header(const std::vector<std::string>&) = 0;
	virtual void write_record(const std::vector<double>&) = 0;
	virtual ~Base() = default;
};

template <typename T>
class Subclass_policy : public Base, private T {
       public:
	using base_t = Base;
	explicit Subclass_policy(const T& t) : T(t) {}
	explicit Subclass_policy(T&& t) : T(std::move(t)) {}
	void write_header(const std::vector<std::string>& x) override {
		T::write_header(x);
	}
	void write_record(const std::vector<double>& x) override {
		T::write_record(x);
	}
};

template <typename T>
using Subclass = PolyphormicSubclass<T, Subclass_policy>;

class CSVFile {
       private:
	std::unique_ptr<std::ostream> out;
	bool header;

       public:
	CSVFile(const std::string& path, bool header);
	void write_header(const std::vector<std::string>&);
	void write_record(const std::vector<double>&);

	static constexpr const auto &name = "CSVFile";
	static constexpr const auto& keywords =
	    make_array<const char*>("path", "header");
	static auto factory(const std::string& path, bool header) {
		return CSVFile(path, header);
	}
};

}  // namespace Output

namespace YAML {

template <>
std::unique_ptr<Output::Base> Node::as() const;

}  // namespace YAML

#endif  // OUTPUT_H
